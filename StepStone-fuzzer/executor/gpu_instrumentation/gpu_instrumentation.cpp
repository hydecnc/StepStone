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

	const std::uint64_t offset = entry_index * 0x1000ULL + 0x28ULL;

	if (offset + sizeof(std::uint32_t) > info->status_queue_iova) {
		errno = ERANGE;
		return -1;
	}
	ret = modifyMemoryRegionSimple(info->status_queue_iova, offset,
				       info->status_queue_size, elem_count);

	return ret;
}
