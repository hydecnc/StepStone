#ifndef IOVA_INJECTOR_H
#define IOVA_INJECTOR_H

#define IOCTL_MAGIC 'a'

struct iova_injector_req {
	void* buf;
	size_t amount;
	size_t offset;
};

#define READ_IOVA _IOW(IOCTL_MAGIC, 1, struct iova_injector_req*)
#define WRITE_IOVA _IOW(IOCTL_MAGIC, 2, struct iova_injector_req*)

#endif
