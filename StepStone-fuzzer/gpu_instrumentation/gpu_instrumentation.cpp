#include "gpu_instrumentation.h"
#include "utilities.h"
#include <cstdint>
#include <cstring>
#include <vector>

// Helper to insert U32 to a buffer
void putU32(std::vector<std::uint8_t>& buf, const std::uint64_t offset,
	    const std::uint32_t value)
{
	const auto bytes{u32ToBuf(value)};
	std::memcpy(buf.data() + offset, bytes.data(), bytes.size());
}

/**
 * insertBuffer: insert a random buffer at a specific region of the message queue; intentionally unsafe to serve as a quick example/primitive
 * @buffer: the buffer to be inserted
 * @size: the size of the buffer, in bytes
 * @offset: offset from the kernel virtual address of the message queue base
 */
int insertBuffer(std::uint8_t* buffer, const std::uint32_t size, const std::uint64_t offset)
{
	if (buffer == nullptr || size == 0) {
		errno = EINVAL;
		return -1;
	}

	const auto info{prepareInjection()};
	if (!info) {
		errno = ENODEV;
		return -1;
	}

	// create a message
	std::vector<std::uint8_t> message(buffer, buffer + size);

	if (!injectorWriteMemory(offset, message)) {
		errno = EIO;
		return -1;
	}

	return 0;
}
