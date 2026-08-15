#include "utilities.h"
#include "memory_injector.h"
#include <cerrno>
#include <cstdint>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <sys/ioctl.h>
#include <system_error>
#include <unistd.h>
#include <vector>

inline constexpr std::string_view DEVICE{"/dev/memory-injector"};

namespace fs = std::filesystem;

namespace
{
enum class AddressType {
	phys,
	kva,
};

class Injector
{
      private:
	int m_fd{};
	std::uint64_t m_regionSize{};

	bool inRegion(const std::uint64_t offset, const std::uint64_t size) const
	{
		if (m_regionSize == 0) {
			errno = EINVAL;
			return false;
		}
		if (size > m_regionSize || offset > m_regionSize - size) {
			errno = ERANGE;
			return false;
		}
		return true;
	}

      public:
	Injector(const std::string_view injector)
	    : m_fd{open(injector.data(), O_RDWR)}
	{
	}
	Injector(const Injector&) = delete;
	Injector& operator=(const Injector&) = delete;
	~Injector()
	{
		if (m_fd != -1) {
			close(m_fd);
		}
	}

	bool isOpen(void) const
	{
		return m_fd != -1;
	}

	bool setRegion(const std::uint64_t base, const std::uint64_t size,
		       const AddressType type)
	{
		if (!isOpen()) {
			fprintf(stderr,
				"[GPU INSTRUMENTATION] could not open %s\n",
				DEVICE.data());
			return false;
		}

		struct memory_injector_config config{base, size};

		unsigned long callType{};
		switch (type) {
		case AddressType::phys:
			callType = SET_MEMORY_REGION_PHYS;
			break;
		case AddressType::kva:
			callType = SET_MEMORY_REGION_KVA;
		}

		if (ioctl(m_fd, callType, &config) == -1) {
			perror("ioctl SET_MEMORY_REGION");
			return false;
		}

		m_regionSize = size;
		return true;
	}

	std::optional<std::vector<std::uint8_t>> readAt(const std::uint64_t offset,
							const std::uint64_t size)
	{
		if (!inRegion(offset, size)) {
			return std::nullopt;
		}

		std::vector<std::uint8_t> buf(size);
		struct memory_injector_req req{
		    reinterpret_cast<std::uintptr_t>(buf.data()),
		    size,
		    offset,
		};

		if (ioctl(m_fd, READ_MEMORY, &req) == -1) {
			perror("ioctl READ_MEMORY");
			return std::nullopt;
		}
		return buf;
	}

	bool writeAt(const std::uint64_t offset, const void* buffer,
		     const std::uint64_t size)
	{
		if (!inRegion(offset, size)) {
			return false;
		}

		struct memory_injector_req req{
		    reinterpret_cast<std::uintptr_t>(buffer),
		    size,
		    offset,
		};

		if (ioctl(m_fd, WRITE_MEMORY, &req) == -1) {
			perror("ioctl WRITE_MEMORY");
			return false;
		}
		return true;
	}
};

Injector g_injector{DEVICE};

std::optional<std::uint64_t> parseNumber(const std::string& str)
{
	try {
		return static_cast<std::uint64_t>(std::stoull(str, nullptr, 0));
	} catch (std::exception&) {
		return std::nullopt;
	}
}
} // namespace

std::optional<GspMsgQueue::Info> getGspMsgQueueInfo(void)
{
	std::error_code ec;
	for (auto const& dir_entry : fs::directory_iterator(
		 fs::path("/proc/driver/nvidia/gpus"), ec)) {
		if (dir_entry.path().filename().string().front() == '.') {
			continue;
		}

		std::ifstream file{dir_entry.path() / "instrumentation"};
		if (!file) {
			continue;
		}

		//
		// /proc/driver/nvidia/gpus/*/instrumentation provides one key & value pair
		// per line, formatted as key=value.
		//
		GspMsgQueue::Info info{};
		for (std::string line{}; std::getline(file, line);) {
			const auto eq{line.find('=')};
			if (eq == std::string::npos) {
				continue;
			}
			const auto value{parseNumber(line.substr(eq + 1))};
			if (!value) {
				continue;
			}

			const std::string_view key{line.data(), eq};
			if (key == "shared_mem_kva") {
				info.shared_mem_kva = *value;
			} else if (key == "shared_mem_size") {
				info.shared_mem_size = *value;
			} else if (key == "cmd_queue_offset") {
				info.cmd_queue_offset = *value;
			} else if (key == "cmd_queue_size") {
				info.cmd_queue_size = *value;
			} else if (key == "status_queue_offset") {
				info.status_queue_offset = *value;
			} else if (key == "status_queue_size") {
				info.status_queue_size = *value;
			}
		}

		if (info.shared_mem_kva == 0 || info.shared_mem_size == 0) {
			fprintf(stderr,
				"[GPU INSTRUMENTATION] instrumentation is missing "
				"shared_mem_kva - driver predates the "
				"non-contiguous fix, rebuild it\n");
			return std::nullopt;
		}
		return info;
	}
	return std::nullopt;
}

bool injectorSetMemoryKVA(const GspMsgQueue::Info& info)
{
	if (info.shared_mem_kva == 0 || info.shared_mem_size == 0) {
		errno = ENODEV;
		return false;
	}
	if (!g_injector.setRegion(info.shared_mem_kva, info.shared_mem_size,
				  AddressType::kva)) {
		return false;
	}
	fprintf(stderr,
		"[GPU INSTRUMENTATION] injector region: kva 0x%llx size 0x%llx\n",
		static_cast<unsigned long long>(info.shared_mem_kva),
		static_cast<unsigned long long>(info.shared_mem_size));
	return true;
}

std::optional<std::vector<std::uint8_t>>
injectorReadMemory(const std::uint64_t offset, const std::uint64_t size)
{
	return g_injector.readAt(offset, size);
}

bool injectorWriteMemory(const std::uint64_t offset,
			 const std::vector<std::uint8_t>& buffer)
{
	return g_injector.writeAt(offset, buffer.data(), buffer.size());
}

bool injectorWriteMemoryRaw(const std::uint64_t offset, const void* buffer,
			    const std::uint64_t size)
{
	return g_injector.writeAt(offset, buffer, size);
}

std::optional<std::uint32_t> injectorReadMemoryU32(std::uint64_t offset)
{
	const auto buf{injectorReadMemory(offset, 4)};
	if (!buf) {
		return std::nullopt;
	}
	return bufToU32(*buf);
}

bool injectorWriteMemoryU32(std::uint64_t offset, std::uint32_t value)
{
	return injectorWriteMemory(offset, u32ToBuf(value));
}

const GspMsgQueue::Info* prepareInjection(void)
{
	static std::optional<GspMsgQueue::Info> boundInfo;

	const auto info{getGspMsgQueueInfo()};
	if (!info) {
		boundInfo.reset();
		errno = ENODEV;
		return nullptr;
	}

	if (boundInfo &&
	    boundInfo->shared_mem_kva == info->shared_mem_kva &&
	    boundInfo->shared_mem_size == info->shared_mem_size) {
		return &*boundInfo;
	}

	boundInfo.reset();

	if (!injectorSetMemoryKVA(*info)) {
		errno = ENODEV;
		return nullptr;
	}

	boundInfo = info;
	return &*boundInfo;
}
