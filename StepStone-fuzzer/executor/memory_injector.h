#ifndef MEMORY_INJECTOR_H
#define MEMORY_INJECTOR_H

#include <linux/types.h>

#define IOCTL_MAGIC 'a'

struct memory_injector_config {
	__u64 base;
	__u64 size;
};

struct memory_injector_req {
	__u64 buf;
	__u32 amount;
	__u32 offset;
};

#define SET_MEMORY_REGION _IOW(IOCTL_MAGIC, 1, struct memory_injector_config)
#define READ_MEMORY _IOW(IOCTL_MAGIC, 2, struct memory_injector_req)
#define WRITE_MEMORY _IOW(IOCTL_MAGIC, 3, struct memory_injector_req)

#endif
