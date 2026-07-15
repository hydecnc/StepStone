#ifndef INSTRUMENT_UTILITIES_H
#define INSTRUMENT_UTILITIES_H

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

struct GspMsgQueueInfo {
	std::uint64_t status_queue_iova{};
	std::uint64_t status_queue_offset{};
	std::uint64_t status_queue_size{};
	std::uint32_t rxSeqNum{};
};

inline std::uint32_t bufToU32(const std::uint8_t* const buf)
{
	return static_cast<std::uint32_t>(buf[0]) |
	       static_cast<std::uint32_t>(buf[1] << 8) |
	       static_cast<std::uint32_t>(buf[2] << 16) |
	       static_cast<std::uint32_t>(buf[3] << 24);
}

inline std::array<std::uint8_t, 4> u32ToBuf(std::uint32_t num)
{
	return {
	    static_cast<std::uint8_t>(num >> 0),
	    static_cast<std::uint8_t>(num >> 8),
	    static_cast<std::uint8_t>(num >> 16),
	    static_cast<std::uint8_t>(num >> 24),
	};
}

std::optional<GspMsgQueueInfo> getGspMsgQueueInfo(void);
int modifyMemoryRegionSimple(const std::uint64_t base,
			     const std::uint64_t offset,
			     const std::uint64_t size,
			     const std::uint32_t value);
std::optional<std::vector<std::uint8_t>>
dumpMemoryRegion(const uint64_t base, const uint64_t offset,
		 const uint64_t size);
int modifyMemoryRegion(const std::uint64_t base, const std::uint64_t offset,
		       const std::vector<std::uint8_t>& buffer);

#endif
