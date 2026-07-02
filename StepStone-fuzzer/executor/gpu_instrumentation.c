#include "gpu_instrumentation.h"
#include "memory_injector.h"
#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

struct GspMsgQueueInfo {
	uint64_t status_queue_iova;
	uint64_t status_queue_offset;
	uint64_t status_queue_size;
};

static struct GspMsgQueueInfo* getGspMsgQueueInfo(void)
{
	// TODO: Consider adding better error handling
	struct GspMsgQueueInfo* info = malloc(sizeof(struct GspMsgQueueInfo));
	if (!info) {
		return NULL;
	}

	// read information from /proc/driver/nvidia/gpus/*/instrumentation
	DIR* dir = opendir("/proc/driver/nvidia/gpus");
	if (!dir) {
		free(info);
		return NULL;
	}

	// only read information from one GPU driver
	char path[512];
	FILE* file;
	struct dirent* dirent;
	while ((dirent = readdir(dir)) != NULL) {
		if (dirent->d_name[0] == '.')
			continue;

		snprintf(path, sizeof(path),
			 "/proc/driver/nvidia/gpus/%s/instrumentation",
			 dirent->d_name);

		file = fopen(path, "r");
		if (!file)
			continue;

		if (fscanf(file, "%*[^=]=%lx", &info->status_queue_iova) != 1 ||
		    // fscanf(file, "%*[^=]=%lx", &info->status_queue_offset) != 1 ||
		    fscanf(file, "%*[^=]=%lx", &info->status_queue_size) != 1) {
			fclose(file);
			closedir(dir);
			free(info);
			return NULL;
		}
		info->status_queue_offset = 0x1028;

		fclose(file);
		closedir(dir);
		return info;
	}

	closedir(dir);
	free(info);
	return NULL;
}

static inline void u32ToBuf(unsigned char buf[4], uint32_t num)
{
	buf[0] = (uint8_t)(num >> 0);
	buf[1] = (uint8_t)(num >> 8);
	buf[2] = (uint8_t)(num >> 16);
	buf[3] = (uint8_t)(num >> 24);
}

static int modifyIOVARegion(const uint64_t base, const uint64_t offset,
			    const uint64_t size, const uint32_t value)
{
	// TODO: consider adding logs only when debug flag is set
	const uint64_t addr = base + offset;
	uint8_t valueBuf[4];
	u32ToBuf(valueBuf, value);

	int dev = open("/dev/memory-injector", O_RDWR);
	if (dev == -1) {
		perror("[GPU INSTUMENTATION] open");
		return -1;
	}

	int retval;

	struct memory_injector_config config = {
	    .base = base,
	    .size = size,
	};

	void* buf = malloc(sizeof(valueBuf));
	struct memory_injector_req read_req = {
	    .buf = (uint64_t)buf,
	    .amount = sizeof(valueBuf),
	    .offset = offset,
	};
	struct memory_injector_req write_req = {
	    .buf = (uint64_t)valueBuf,
	    .amount = sizeof(valueBuf),
	    .offset = offset,
	};

	fprintf(stderr, "[GPU INSTUMENTATION] Setting memory region\n");
	retval = ioctl(dev, SET_MEMORY_REGION, &config);
	if (retval == -1) {
		fprintf(stderr,
			"[GPU INSTUMENTATION] Memory Region Setting Failed.\n");
		free(buf);
		close(dev);
		return -1;
	}

	fprintf(stderr,
		"[GPU INSTUMENTATION] Reading current value of variable at %lx\n",
		addr);
	retval = ioctl(dev, READ_MEMORY, &read_req);
	if (retval == -1) {
		perror("[GPU INSTUMENTATION] ioctl");
		free(buf);
		close(dev);
		return -1;
	}
	if (fwrite(buf, 1, sizeof(valueBuf), stdout) != sizeof(valueBuf)) {
		perror("[GPU INSTUMENTATION] fwrite");
	}

	fprintf(stderr,
		"[GPU INSTUMENTATION] Writing value %u to variable at %lx\n",
		value, addr);
	retval = ioctl(dev, WRITE_MEMORY, &write_req);
	if (retval == -1) {
		perror("[GPU INSTUMENTATION] ioctl");
		free(buf);
		close(dev);
		return -1;
	}

	fprintf(stderr, "[GPU INSTUMENTATION] Reading new variable at %lx\n",
		addr);
	retval = ioctl(dev, READ_MEMORY, &read_req);
	if (retval == -1) {
		perror("[GPU INSTUMENTATION] ioctl");
		free(buf);
		close(dev);
		return -1;
	}
	if (fwrite(buf, 1, sizeof(valueBuf), stdout) != sizeof(valueBuf)) {
		perror("[GPU INSTUMENTATION] fwrite");
	}

	free(buf);
	close(dev);
	return 0;
}

int instrument_gpu(void)
{
	int ret;
	struct GspMsgQueueInfo* info = getGspMsgQueueInfo();
	if (!info)
		return -1;
	ret = modifyIOVARegion(info->status_queue_iova,
			       info->status_queue_offset,
			       info->status_queue_size, 17);
	free(info);

	return ret;
}
