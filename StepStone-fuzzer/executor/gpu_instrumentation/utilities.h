#ifndef UTILITIES_H
#define UTILITIES_H

#include <cstdint>
#include <optional>
#include <vector>

inline std::uint32_t bufToU32(const std::vector<std::uint8_t>& buf)
{
	return static_cast<std::uint32_t>(buf[0]) |
	       static_cast<std::uint32_t>(buf[1]) << 8 |
	       static_cast<std::uint32_t>(buf[2]) << 16 |
	       static_cast<std::uint32_t>(buf[3]) << 24;
}

inline std::vector<std::uint8_t> u32ToBuf(const std::uint32_t num)
{
	return {
	    static_cast<std::uint8_t>(num),
	    static_cast<std::uint8_t>(num >> 8),
	    static_cast<std::uint8_t>(num >> 16),
	    static_cast<std::uint8_t>(num >> 24),
	};
}

namespace GspMsgQueue
{

struct Info {
	std::uint64_t shared_mem_kva{};
	std::uint64_t shared_mem_size{};
	std::uint64_t cmd_queue_offset{};
	std::uint64_t cmd_queue_size{};
	std::uint64_t status_queue_offset{};
	std::uint64_t status_queue_size{};
	std::uint32_t rxSeqNum{};
	std::uint64_t staging_kva{};
	std::uint64_t staging_size{};
	std::uint64_t staging_isolated{};
};

inline constexpr std::uint64_t TXHDR_MSGCOUNT_OFF{12};
inline constexpr std::uint64_t TXHDR_WRITEPTR_OFF{16};
inline constexpr std::uint64_t TXHDR_ENTRYOFF_OFF{28};

inline constexpr std::uint64_t RXHDR_READPTR_OFF{32};

inline constexpr std::uint64_t ELEM_CHECKSUM_OFF{32};
inline constexpr std::uint64_t ELEM_SEQNUM_OFF{36};
inline constexpr std::uint64_t ELEM_ELEMCOUNT_OFF{40};
inline constexpr std::uint64_t ELEM_HDR_SIZE{48};
inline constexpr std::uint64_t ELEM_RPCLENGTH_OFF{56};
inline constexpr std::uint64_t ELEM_SIZE_MIN{4096};
inline constexpr std::uint64_t ELEM_SIZE_MAX{ELEM_SIZE_MIN * 16};

class Layout
{
      public:
	std::uint64_t statusBase{};
	std::uint64_t cmdBase{};
	std::uint32_t msgCount{};
	std::uint32_t entryOff{};

	std::uint64_t elementOffset(const std::uint32_t slot) const
	{
		return statusBase + entryOff +
		       static_cast<std::uint64_t>(slot % msgCount) * ELEM_SIZE_MIN;
	}
	std::uint64_t writePtrOffset(void) const
	{
		return statusBase + TXHDR_WRITEPTR_OFF;
	}

	//
	// In the COMMAND queue, not the status queue. msgqTxCreate is called with
	// MSGQ_FLAGS_SWAP_RX, which puts each side's read cursor in its own backing
	// store rather than in the one it consumes. The readPtr at statusBase + 0x20
	// is the GSP's cursor over the command queue - a plausible-looking but
	// unrelated counter.
	//
	std::uint64_t readPtrOffset(void) const
	{
		return cmdBase + RXHDR_READPTR_OFF;
	}
};
} // namespace GspMsgQueue

std::optional<GspMsgQueue::Info> getGspMsgQueueInfo(void);
void reportStagingBuffer(const GspMsgQueue::Info& info);

bool injectorSetMemoryKVA(const GspMsgQueue::Info& info);
std::optional<std::vector<std::uint8_t>>
injectorReadMemory(const std::uint64_t offset, const std::uint64_t size);
bool injectorWriteMemory(const std::uint64_t offset,
			 const std::vector<std::uint8_t>& buffer);

std::optional<std::uint32_t> injectorReadMemoryU32(std::uint64_t offset);
bool injectorWriteMemoryU32(std::uint64_t offset, std::uint32_t value);

std::optional<GspMsgQueue::Layout> queueLayout(const GspMsgQueue::Info& info);
std::optional<std::uint32_t> awaitIdleReadPtr(const GspMsgQueue::Layout& layout);

//
// Pseudo-syscalls are entry points with no shared state, so the region binding
// and layout resolution live here. Re-arms the injector whenever procfs reports
// a different shared_mem_kva, and returns nullptr with errno set when the
// driver has torn the queues down.
//
const GspMsgQueue::Layout* ensureGspReady(void);

#endif
