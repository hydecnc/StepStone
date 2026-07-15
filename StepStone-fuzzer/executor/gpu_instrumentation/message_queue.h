#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <cstdint>
struct StatusQueueEntry {
	std::uint8_t header[32]; // +0x00
	std::uint32_t checkSum; // +0x20
	std::uint32_t seqNum; // +0x24
	std::uint32_t elemCount; // +0x28
	std::uint32_t reserved; // +0x2c

	std::uint32_t rpc_version; // +0x30
	std::uint32_t rpc_signature; // +0x34
	std::uint32_t rpc_length; // +0x38
	std::uint32_t rpc_function; // +0x3c
	std::uint32_t rpc_result; // +0x40
	std::uint32_t rpc_result_private; // +0x44
	std::uint32_t rpc_sequence; // +0x48
	std::uint32_t rpc_union; // +0x4c

	std::uint8_t rpc_payload[4016]; // +0x50
};

inline uint32_t calculate_checksum(const void* data, uint32_t len)
{
	const uint64_t* p = reinterpret_cast<const uint64_t*>(data);
	const uint64_t* pEnd = reinterpret_cast<const uint64_t*>(
	    reinterpret_cast<uintptr_t>(data) + len);
	uint64_t checkSum = 0;

	// XOR all 8-byte words
	while (p < pEnd) {
		checkSum ^= *p++;
	}

	// Fold 64-bit result to 32-bit (XOR high with low)
	const uint32_t high = static_cast<uint32_t>(checkSum >> 32);
	const uint32_t low = static_cast<uint32_t>(checkSum & 0xFFFFFFFFUL);

	return high ^ low;
}

inline void set_valid_checksum(StatusQueueEntry& entry)
{
	entry.checkSum = 0;
	const std::uint32_t checksum_range{0x30 + entry.rpc_length};
	const std::uint32_t calculated{calculate_checksum(&entry,
							  checksum_range)};
	entry.checkSum = calculated;
}

#endif
