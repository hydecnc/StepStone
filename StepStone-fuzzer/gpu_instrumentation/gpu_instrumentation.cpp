#include "gpu_instrumentation.h"
#include "utilities.h"
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <vector>

//
// Layout constants. Every one of these is a static fact read out of the driver
// source; see gpu_instrumentation.txt for the derivation.
//

// msgq.c:msgqTxCreate with hdrAlign = GSP_MSG_QUEUE_HEADER_ALIGN = 4:
//   tx.rxHdrOff = ALIGN_UP(sizeof(msgqTxHeader) = 32, 16) = 0x20
//   tx.entryOff = ALIGN_UP(0x20 + sizeof(msgqRxHeader) = 4, 4096) = 0x1000
static constexpr std::uint64_t SQ_WRITE_PTR_OFF{0x10}; // msgqTxHeader.writePtr
static constexpr std::uint64_t SQ_ENTRY_OFF{0x1000};   // msgqTxHeader.entryOff

// GSP_MSG_QUEUE_ELEMENT_SIZE_MIN == RM_PAGE_SIZE.
static constexpr std::uint64_t SQ_ELEMENT_SIZE{0x1000};

// (0x40000 - 0x1000) / 0x1000, i.e. msgqTxHeader.msgCount.
static constexpr std::uint32_t SQ_MSG_COUNT{63};

// GSP_MSG_QUEUE_ELEMENT field offsets (message_queue_priv.h:43):
//   authTagBuffer[16] @ 0, aadBuffer[16] @ 16, checkSum @ 32, seqNum @ 36,
//   elemCount @ 40, 4 bytes of alignment padding, rpc @ 48.
static constexpr std::uint64_t ELEM_CHECKSUM_OFF{32};
// GSP_MSG_QUEUE_ELEMENT_HDR_SIZE == NV_OFFSETOF(GSP_MSG_QUEUE_ELEMENT, rpc).
static constexpr std::uint64_t ELEM_HDR_SIZE{48};
// rpc_message_header_v.length is the third NvU32 of the rpc header.
static constexpr std::uint64_t ELEM_RPC_LENGTH_OFF{ELEM_HDR_SIZE + 8};

//
// GspMsgQueueReceiveStatus checks msgLen = HDR_SIZE + rpc.length against
// [sizeof(GSP_MSG_QUEUE_ELEMENT) = 80, GSP_MSG_QUEUE_ELEMENT_SIZE_MAX], so
// rpc.length must be at least 32. Keeping HDR_SIZE + length inside one element
// (<= 0x1000) is what makes the checksum a self-contained function of the
// bytes this call writes, so the upper bound is 0x1000 - 48 = 4048.
//
static constexpr std::uint32_t RPC_LENGTH_MIN{32};
static constexpr std::uint32_t RPC_LENGTH_MAX{4048};

namespace
{
std::uint32_t loadU32(const std::uint8_t* p)
{
	return static_cast<std::uint32_t>(p[0]) |
	       static_cast<std::uint32_t>(p[1]) << 8 |
	       static_cast<std::uint32_t>(p[2]) << 16 |
	       static_cast<std::uint32_t>(p[3]) << 24;
}

void storeU32(std::uint8_t* p, const std::uint32_t v)
{
	p[0] = static_cast<std::uint8_t>(v);
	p[1] = static_cast<std::uint8_t>(v >> 8);
	p[2] = static_cast<std::uint8_t>(v >> 16);
	p[3] = static_cast<std::uint8_t>(v >> 24);
}

std::uint64_t loadU64(const std::uint8_t* p)
{
	std::uint64_t v{};
	for (int i{7}; i >= 0; --i) {
		v = (v << 8) | p[i];
	}
	return v;
}

//
// Reimplementation of message_queue_priv.h:_checkSum32(). The driver XORs
// NvU64s while (p < pEnd), so it reads ceil(uLen / 8) whole words, then folds
// the result to 32 bits.
//
std::uint32_t checkSum32(const std::uint8_t* data, const std::uint64_t uLen)
{
	std::uint64_t sum{};
	for (std::uint64_t off{0}; off < uLen; off += 8) {
		sum ^= loadU64(data + off);
	}
	return static_cast<std::uint32_t>(sum >> 32) ^
	       static_cast<std::uint32_t>(sum);
}
} // namespace

/**
 * sqWrite: raw write inside the GSP status queue
 * @buffer: bytes to write, passed to the injector unread
 * @size: byte count; the write is clamped so it cannot leave the status queue
 * @offset: offset from the status queue base
 */
int sqWrite(const void* buffer, const std::uint64_t size,
	    const std::uint64_t offset)
{
	if (buffer == nullptr || size == 0) {
		errno = EINVAL;
		return -1;
	}

	const auto info{prepareInjection()};
	if (!info || info->status_queue_size == 0) {
		errno = ENODEV;
		return -1;
	}

	if (offset >= info->status_queue_size) {
		errno = ERANGE;
		return -1;
	}

	// Static confinement: never spill past the end of the status queue.
	const std::uint64_t room{info->status_queue_size - offset};
	const std::uint64_t amount{size < room ? size : room};

	if (!injectorWriteMemoryRaw(info->status_queue_offset + offset, buffer,
				    amount)) {
		errno = EIO;
		return -1;
	}

	return 0;
}

/**
 * sqSetWritePtr: publish msgqTxHeader.writePtr for the status queue
 * @writePtr: new value; the only header word the driver re-reads live
 *            (msgq.c:600, pWriteIncoming)
 */
int sqSetWritePtr(const std::uint32_t writePtr)
{
	const auto info{prepareInjection()};
	if (!info || info->status_queue_size == 0) {
		errno = ENODEV;
		return -1;
	}

	if (!injectorWriteMemoryU32(info->status_queue_offset + SQ_WRITE_PTR_OFF,
				    writePtr)) {
		errno = EIO;
		return -1;
	}

	return 0;
}

/**
 * sqWriteElement: write one queue element into a status queue entry slot
 * @slot: entry index, [0, msgCount)
 * @element: GSP_MSG_QUEUE_ELEMENT_SIZE_MIN bytes
 * @fixCheckSum: recompute checkSum (and fold rpc.length into range) so the
 *               element clears the checksum reject in GspMsgQueueReceiveStatus
 */
int sqWriteElement(const std::uint32_t slot, const void* element,
		   const bool fixCheckSum)
{
	if (element == nullptr || slot >= SQ_MSG_COUNT) {
		errno = EINVAL;
		return -1;
	}

	const auto info{prepareInjection()};
	if (!info || info->status_queue_size == 0) {
		errno = ENODEV;
		return -1;
	}

	const std::uint64_t offset{SQ_ENTRY_OFF + slot * SQ_ELEMENT_SIZE};
	if (offset + SQ_ELEMENT_SIZE > info->status_queue_size) {
		errno = ERANGE;
		return -1;
	}

	if (!fixCheckSum) {
		// Verbatim: the fuzzer keeps every byte, checksum included.
		if (!injectorWriteMemoryRaw(info->status_queue_offset + offset,
					    element, SQ_ELEMENT_SIZE)) {
			errno = EIO;
			return -1;
		}
		return 0;
	}

	//
	// Copying here is what makes the raw pointer safe to dereference: the
	// description hands us a fixed-size struct, so SQ_ELEMENT_SIZE bytes
	// are readable.
	//
	std::vector<std::uint8_t> buf(
	    static_cast<const std::uint8_t*>(element),
	    static_cast<const std::uint8_t*>(element) + SQ_ELEMENT_SIZE);

	//
	// Fold rpc.length into the window where the checksum stays a function
	// of this element alone. Outside it the driver would checksum bytes
	// from the following slots and no fixup here could be correct.
	//
	const std::uint32_t rawLen{loadU32(buf.data() + ELEM_RPC_LENGTH_OFF)};
	const std::uint32_t len{RPC_LENGTH_MIN +
				rawLen % (RPC_LENGTH_MAX - RPC_LENGTH_MIN + 1)};
	storeU32(buf.data() + ELEM_RPC_LENGTH_OFF, len);

	//
	// checkSum sits in the low half of the NvU64 at offset 32, so zero it,
	// fold the rest, and store the fold back: the driver's XOR then cancels
	// to zero. uLen >= 48 always covers that word.
	//
	storeU32(buf.data() + ELEM_CHECKSUM_OFF, 0);
	const std::uint64_t uLen{ELEM_HDR_SIZE + len};
	storeU32(buf.data() + ELEM_CHECKSUM_OFF, checkSum32(buf.data(), uLen));

	if (!injectorWriteMemory(info->status_queue_offset + offset, buf)) {
		errno = EIO;
		return -1;
	}

	return 0;
}
