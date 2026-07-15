#include "utilities.h"
#include "gpu_instrumentation.h"
#include "memory_injector.h"
#include <array>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <optional>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

std::optional<GspMsgQueueInfo> getGspMsgQueueInfo(void)
{
	for (auto const& dir_entry :
	     fs::directory_iterator(fs::path("/proc/driver/nvidia/gpus"))) {
		if (dir_entry.path().filename().string().front() == '.') {
			continue;
		}
		fs::path infoPath{dir_entry.path() / "instrumentation"};

		std::unique_ptr<std::FILE, int (*)(FILE*)> file{
		    std::fopen(infoPath.c_str(), "r"), &std::fclose};
		if (!file) {
			continue;
		}

		GspMsgQueueInfo info{};

		//
		// /proc/driver/nvidia/gpus/*/instrumentation provides information in the following format
		// status_queue_iova=0x1ae9b6000
		// status_queue_offset=0x41000
		// status_queue_size=0x40000
		// rx_seq_num=259
		//
		if (std::fscanf(file.get(), "%*[^=]=%lx",
				&info.status_queue_iova) != 1 ||
		    std::fscanf(file.get(), "%*[^=]=%lx",
				&info.status_queue_offset) != 1 ||
		    std::fscanf(file.get(), "%*[^=]=%lx",
				&info.status_queue_size) != 1 ||
		    std::fscanf(file.get(), "%*[^=]=%d", &info.rxSeqNum) != 1) {
			continue;
		}
		return info;
	}

	return std::nullopt;
}

int modifyMemoryRegionSimple(const std::uint64_t base,
			     const std::uint64_t offset,
			     const std::uint64_t size,
			     const std::uint32_t value)
{
	// TODO: consider adding logs only when debug flag is set
	const uint64_t addr = base + offset;
	std::array<std::uint8_t, 4> valueBuf = u32ToBuf(value);

	int dev = open(conf::GPU_INSTRUMENTATION_DEVICE.data(), O_RDWR);
	if (dev == -1) {
		perror("[GPU INSTRUMENTATION] open");
		return -1;
	}

	int retval;

	struct memory_injector_config config = {
	    .base = base,
	    .size = size,
	};

	void* buf = malloc(sizeof(valueBuf));
	if (!buf) {
		close(dev);
		return -1;
	}
	struct memory_injector_req read_req = {
	    .buf = (uint64_t)buf,
	    .amount = sizeof(valueBuf),
	    .offset = offset,
	};
	struct memory_injector_req write_req = {
	    .buf = static_cast<std::uint8_t>(
		reinterpret_cast<std::uintptr_t>(valueBuf.data())),
	    .amount = sizeof(valueBuf),
	    .offset = offset,
	};

	fprintf(stderr, "[GPU INSTRUMENTATION] Setting memory region\n");
	retval = ioctl(dev, SET_MEMORY_REGION, &config);
	if (retval == -1) {
		fprintf(stderr,
			"[GPU INSTRUMENTATION] Memory Region Setting Failed.\n");
		free(buf);
		close(dev);
		return -1;
	}

	fprintf(stderr,
		"[GPU INSTRUMENTATION] Reading current value of variable at %lx\n",
		addr);
	retval = ioctl(dev, READ_MEMORY, &read_req);
	if (retval == -1) {
		perror("[GPU INSTRUMENTATION] ioctl");
		free(buf);
		close(dev);
		return -1;
	}
	fprintf(stderr, "[GPU INSTRUMENTATION] New value: %d\n",
		bufToU32(valueBuf.data()));

	fprintf(stderr,
		"[GPU INSTRUMENTATION] Writing value %u to variable at %lx\n",
		value, addr);
	retval = ioctl(dev, WRITE_MEMORY, &write_req);
	if (retval == -1) {
		perror("[GPU INSTRUMENTATION] ioctl");
		free(buf);
		close(dev);
		return -1;
	}

	fprintf(stderr, "[GPU INSTRUMENTATION] Reading new variable at %lx\n",
		addr);
	retval = ioctl(dev, READ_MEMORY, &read_req);
	if (retval == -1) {
		perror("[GPU INSTRUMENTATION] ioctl");
		free(buf);
		close(dev);
		return -1;
	}
	fprintf(stderr, "[GPU INSTRUMENTATION] New value: %d\n",
		bufToU32(valueBuf.data()));

	free(buf);
	close(dev);
	return 0;
}

std::optional<std::vector<std::uint8_t>>
dumpMemoryRegion(const uint64_t base, const uint64_t offset,
		 const uint64_t size)
{
	const uint64_t addr{base + offset};

	const int fd{open(conf::GPU_INSTRUMENTATION_DEVICE.data(), O_RDWR)};
	if (fd == -1) {
		perror("[GPU INSTRUMENTATION] open");
		return std::nullopt;
	}

	std::vector<std::uint8_t> buf(size);

	struct memory_injector_config config{
	    .base = base,
	    .size = size,
	};
	struct memory_injector_req read_req{
	    .buf = reinterpret_cast<std::uint64_t>(buf.data()),
	    .amount = size,
	    .offset = offset,
	};

	fprintf(stderr, "[GPU INSTRUMENTATION] Setting memory region\n");
	if (ioctl(fd, SET_MEMORY_REGION, &config) == -1) {
		perror("[GPU INSTRUMENTATION] ioctl");
		close(fd);
		return std::nullopt;
	}

	fprintf(stderr,
		"[GPU INSTRUMENTATION] Reading current value of variable at %lx\n",
		addr);
	if (ioctl(fd, READ_MEMORY, &read_req) == -1) {
		perror("[GPU INSTRUMENTATION] ioctl");
		close(fd);
		return std::nullopt;
	}
	close(fd);
	return buf;
}

int modifyMemoryRegion(const std::uint64_t base, const std::uint64_t offset,
		       const std::vector<std::uint8_t>& buffer)
{
	if (base > UINT64_MAX - offset) {
		errno = EOVERFLOW;
		return -1;
	}

	const uint64_t addr{base + offset};
	const uint64_t size{buffer.size() * sizeof(std::uint8_t)};

	const int fd = open(conf::GPU_INSTRUMENTATION_DEVICE.data(), O_RDWR);
	if (fd == -1) {
		perror("[GPU INSTRUMENTATION] open");
		return -1;
	}

	struct memory_injector_config config = {
	    .base = addr,
	    .size = size,
	};

	struct memory_injector_req write_req = {
	    .buf = reinterpret_cast<std::uint64_t>(buffer.data()),
	    .amount = size,
	    .offset = 0,
	};

	fprintf(stderr, "[GPU INSTRUMENTATION] Setting memory region\n");
	if (ioctl(fd, SET_MEMORY_REGION, &config) == -1) {
		perror("[GPU INSTRUMENTATION] ioctl");
		close(fd);
		return -1;
	}

	fprintf(stderr, "[GPU INSTRUMENTATION] Writing buffer at %lx\n", addr);
	if (ioctl(fd, WRITE_MEMORY, &write_req) == -1) {
		perror("[GPU INSTRUMENTATION] ioctl");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}
