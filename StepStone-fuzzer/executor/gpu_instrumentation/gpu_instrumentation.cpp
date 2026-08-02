#include "gpu_instrumentation.h"
#include "utilities.h"
#include <cerrno>
#include <cstdint>
#include <dirent.h>
#include <fcntl.h>
#include <optional>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

int instrument_gpu_elemcount(const std::uint64_t entry_index,
			     const std::uint32_t elem_count)
{
	int ret;
	if (entry_index >= 63) {
		errno = EINVAL;
		return -1;
	}

	const auto info{getGspMsgQueueInfo()};
	if (!info) {
		errno = ENODEV;
		return -1;
	}

	//
	// Poke the elemCount field (+0x28) of ring element `entry_index`.
	// Entries start at entryOff (0x1000, one header page in) - the previous
	// code omitted entryOff and mis-bounded against status_queue_iova.
	//
	const auto entryOffBuf{
	    dumpMemoryRegion(info->status_queue_iova, 0x1cULL, 4)};
	if (!entryOffBuf) {
		return -1;
	}
	const std::uint32_t entryOff{bufToU32(entryOffBuf->data())};

	const std::uint64_t offset =
	    entryOff + entry_index * 0x1000ULL + 0x28ULL;

	if (offset + sizeof(std::uint32_t) > info->status_queue_size) {
		errno = ERANGE;
		return -1;
	}
	ret = modifyMemoryRegionSimple(info->status_queue_iova, offset,
				       info->status_queue_size, elem_count);

	return ret;
}
