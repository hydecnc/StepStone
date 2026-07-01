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
	// Struct offsets provided as convienence. Can be obtained via scanning the entire IOVA region and analyzing the source code.
	std::uint64_t shared_mem_kva{};
	std::uint64_t shared_mem_size{};
	std::uint64_t cmd_queue_offset{};
	std::uint64_t cmd_queue_size{};
	std::uint64_t status_queue_offset{};
	std::uint64_t status_queue_size{};
};
} // namespace GspMsgQueue

std::optional<GspMsgQueue::Info> getGspMsgQueueInfo(void);
const GspMsgQueue::Info* prepareInjection(void);

bool injectorSetMemoryKVA(const GspMsgQueue::Info& info);
std::optional<std::vector<std::uint8_t>>
injectorReadMemory(const std::uint64_t offset, const std::uint64_t size);
bool injectorWriteMemory(const std::uint64_t offset,
			 const std::vector<std::uint8_t>& buffer);

std::optional<std::uint32_t> injectorReadMemoryU32(std::uint64_t offset);
bool injectorWriteMemoryU32(std::uint64_t offset, std::uint32_t value);

#endif
