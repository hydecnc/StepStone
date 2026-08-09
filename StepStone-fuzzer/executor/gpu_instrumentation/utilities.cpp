#include "utilities.h"
#include "memory_injector.h"
#include <cerrno>
#include <cstdint>
#include <cstdio>
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

	bool writeAt(const std::uint64_t offset,
		     const std::vector<std::uint8_t>& buffer)
	{
		if (!inRegion(offset, buffer.size())) {
			return false;
		}

		struct memory_injector_req req{
		    reinterpret_cast<std::uintptr_t>(buffer.data()),
		    buffer.size(),
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
	//
	// The error_code overload: the throwing one raises filesystem_error when
	// the nvidia module is not loaded, which would kill the executor.
	//
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
			} else if (key == "rx_seq_num") {
				info.rxSeqNum = static_cast<std::uint32_t>(*value);
			} else if (key == "staging_kva") {
				info.staging_kva = *value;
			} else if (key == "staging_size") {
				info.staging_size = *value;
			} else if (key == "staging_isolated") {
				info.staging_isolated = *value;
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

void reportStagingBuffer(const GspMsgQueue::Info& info)
{
	constexpr std::uint64_t DIRECT_MAP_BASE{0xffff888000000000ULL};
	constexpr std::uint64_t VMALLOC_BASE{0xffffc90000000000ULL};
	constexpr std::uint64_t VMALLOC_END{0xffffe90000000000ULL};

	const char* origin{"unrecognised range"};
	if (info.staging_kva >= VMALLOC_BASE && info.staging_kva < VMALLOC_END) {
		origin = "vmalloc, needs CONFIG_KASAN_VMALLOC=y";
	} else if (info.staging_kva >= DIRECT_MAP_BASE &&
		   info.staging_kva < VMALLOC_BASE) {
		origin = "kmalloc, redzone expected";
	}

	fprintf(stderr,
		"[GPU INSTRUMENTATION] staging buffer: kva 0x%llx size 0x%llx "
		"first oob byte 0x%llx [%s]%s\n",
		static_cast<unsigned long long>(info.staging_kva),
		static_cast<unsigned long long>(info.staging_size),
		static_cast<unsigned long long>(info.staging_kva +
						info.staging_size),
		origin,
		info.staging_isolated == 0
		    ? " built without GPU_INSTRUMENTATION - the overrun has "
		      "nothing to hit"
		    : "");
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
	return g_injector.writeAt(offset, buffer);
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

std::optional<GspMsgQueue::Layout> queueLayout(const GspMsgQueue::Info& info)
{
	GspMsgQueue::Layout layout{};
	layout.statusBase = info.status_queue_offset;
	layout.cmdBase = info.cmd_queue_offset;

	const auto msgCount{injectorReadMemoryU32(layout.statusBase +
						  GspMsgQueue::TXHDR_MSGCOUNT_OFF)};
	const auto entryOff{injectorReadMemoryU32(layout.statusBase +
						  GspMsgQueue::TXHDR_ENTRYOFF_OFF)};
	if (!msgCount || !entryOff) {
		return std::nullopt;
	}

	layout.msgCount = *msgCount;
	layout.entryOff = *entryOff;

	if (layout.msgCount == 0) {
		return std::nullopt;
	}
	if (layout.entryOff + static_cast<std::uint64_t>(layout.msgCount) *
				  GspMsgQueue::ELEM_SIZE_MIN >
	    info.status_queue_size) {
		fprintf(stderr,
			"[GPU INSTRUMENTATION] ring does not fit in the status "
			"queue - layout skew\n");
		return std::nullopt;
	}

	fprintf(stderr,
		"[GPU INSTRUMENTATION] status queue @+0x%llx cmd queue @+0x%llx: "
		"msgCount=%u entryOff=0x%x readPtr@+0x%llx\n",
		static_cast<unsigned long long>(layout.statusBase),
		static_cast<unsigned long long>(layout.cmdBase), layout.msgCount,
		layout.entryOff,
		static_cast<unsigned long long>(layout.readPtrOffset()));
	return layout;
}

std::optional<std::uint32_t>
awaitIdleReadPtr(const GspMsgQueue::Layout& layout)
{
	constexpr int attempts{200};

	for (int i = 0; i < attempts; ++i) {
		const auto readPtr{injectorReadMemoryU32(layout.readPtrOffset())};
		const auto writePtr{injectorReadMemoryU32(layout.writePtrOffset())};
		if (!readPtr || !writePtr) {
			return std::nullopt;
		}

		if (*readPtr == *writePtr) {
			usleep(1000);
			const auto again{injectorReadMemoryU32(layout.readPtrOffset())};
			if (again && *again == *readPtr) {
				return *readPtr;
			}
			continue;
		}
		usleep(1000);
	}

	return std::nullopt;
}

const GspMsgQueue::Layout* ensureGspReady(void)
{
	static std::optional<GspMsgQueue::Info> boundInfo;
	static std::optional<GspMsgQueue::Layout> boundLayout;

	//
	// procfs is re-read on every entry rather than cached, because
	// shared_mem_kva is only valid while the queues live. GspMsgQueuesCleanup
	// calls clearGspMsgQueueInfo() before memdescUnmap(), so procfs going
	// quiet - or reporting a different base - is the signal that a cached
	// region now points at freed memory. Only the ioctl is skipped when the
	// region is unchanged.
	//
	const auto info{getGspMsgQueueInfo()};
	if (!info) {
		boundInfo.reset();
		boundLayout.reset();
		errno = ENODEV;
		return nullptr;
	}

	if (boundLayout && boundInfo &&
	    boundInfo->shared_mem_kva == info->shared_mem_kva &&
	    boundInfo->shared_mem_size == info->shared_mem_size) {
		return &*boundLayout;
	}

	boundInfo.reset();
	boundLayout.reset();

	if (!injectorSetMemoryKVA(*info)) {
		errno = ENODEV;
		return nullptr;
	}
	reportStagingBuffer(*info);

	boundLayout = queueLayout(*info);
	if (!boundLayout) {
		errno = EIO;
		return nullptr;
	}
	boundInfo = info;
	return &*boundLayout;
}
