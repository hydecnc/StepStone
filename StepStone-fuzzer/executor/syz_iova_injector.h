#ifndef SYZ_IOVA_INJECTOR_H
#define SYZ_IOVA_INJECTOR_H

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "iova_injector.h"

#define STEPSTONE_IOVA_ELEMCOUNT_OFFSET 0x1028

static inline void stepstone_put_u32_le(unsigned char buf[4], uint32_t num)
{
	buf[0] = (unsigned char)(num >> 0);
	buf[1] = (unsigned char)(num >> 8);
	buf[2] = (unsigned char)(num >> 16);
	buf[3] = (unsigned char)(num >> 24);
}

static const uint32_t stepstone_iova_interesting_values[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    7,
    8,
    15,
    16,
    17,
    31,
    32,
    63,
    64,
    127,
    // 128,
    // 255,
    // 256,
    // 1023,
    // 1024,
    // 4095,
    // 4096,
    // 0x7fffffff,
    // 0x80000000,
    // 0xffffffff,
};

static uint64_t stepstone_iova_inject_counter;

static inline void gpu_iova_inject_after_nvidia_init(void)
{
	static int dev = -1;

	if (dev < 0) {
		dev = open("/dev/iova-injector", O_RDWR | O_CLOEXEC);
		if (dev < 0) {
			debug("[IOVA injector] open failed: errno=%d\n", errno);
			return;
		}
	}

	uint32_t value = stepstone_iova_interesting_values[stepstone_iova_inject_counter %
							   (sizeof(stepstone_iova_interesting_values) /
							    sizeof(stepstone_iova_interesting_values[0]))];
	stepstone_iova_inject_counter++;

	unsigned char elemCountBuf[4];
	stepstone_put_u32_le(elemCountBuf, value);

	struct iova_injector_req write_req;
	memset(&write_req, 0, sizeof(write_req));
	write_req.buf = elemCountBuf;
	write_req.amount = sizeof(elemCountBuf);
	write_req.offset = STEPSTONE_IOVA_ELEMCOUNT_OFFSET;

	errno = 0;
	int retval = ioctl(dev, WRITE_IOVA, &write_req);

	debug("[IOVA injector] WRITE_IOVA off=0x%llx amount=%llu value=0x%x ret=%d errno=%d\n",
	      (unsigned long long)write_req.offset,
	      (unsigned long long)write_req.amount,
	      value, retval, errno);
}

#endif
