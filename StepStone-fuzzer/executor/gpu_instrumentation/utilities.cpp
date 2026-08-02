#include "utilities.h"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <optional>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

//
// Injection backend.
//
// Previously every read/write opened /dev/memory-injector and issued
// SET_MEMORY_REGION (memremap) + READ/WRITE_MEMORY + close *per field access*.
// At fuzzing throughput that per-call remap dominates wall-clock and is racy
// against the live GSP. The GSP status queue is ordinary host system RAM
// (ADDR_SYSMEM, NV_MEMORY_CACHED - see message_queue_cpu.c), so we mmap its
// physical pages once through /dev/mem and reuse a direct, WB-cached,
// DMA-coherent pointer for the whole executor lifetime.
//
// PREREQUISITE: the target kernel must be built with CONFIG_STRICT_DEVMEM=n so
// /dev/mem can map System RAM. If that cannot be disabled, add a .mmap handler
// to the injector module (remap_pfn_range over the same physical base) and open
// that device here instead - the logic below is identical either way.
//
#define PHYS_MEM_DEVICE "/dev/mem"

namespace
{

std::uint8_t* g_map{nullptr}; // usable base (points at status_queue_iova)
std::uint8_t* g_map_raw{nullptr}; // page-aligned mmap base (for munmap)
std::size_t g_map_raw_len{0};
std::uint64_t g_map_base{0}; // physical address g_map corresponds to
std::uint64_t g_map_size{0}; // usable region size
int g_mem_fd{-1};

//
// Establish (once) a persistent mapping of the full status-queue region and
// return the base pointer, or nullptr on failure. Cached across calls; only
// remaps if the region base changes (should not happen within a boot).
//
std::uint8_t* ensureMapped(const std::uint64_t base)
{
	if (g_map != nullptr && base == g_map_base) {
		return g_map;
	}

	const auto info{getGspMsgQueueInfo()};
	if (!info || info->status_queue_iova != base ||
	    info->status_queue_size == 0) {
		errno = ENODEV;
		return nullptr;
	}

	if (g_map_raw != nullptr) {
		munmap(g_map_raw, g_map_raw_len);
		g_map_raw = nullptr;
		g_map = nullptr;
	}

	if (g_mem_fd == -1) {
		g_mem_fd = open(PHYS_MEM_DEVICE, O_RDWR); // no O_SYNC: keep WB
		if (g_mem_fd == -1) {
			perror("[GPU INSTRUMENTATION] open " PHYS_MEM_DEVICE);
			return nullptr;
		}
	}

	const std::uint64_t pageSize{
	    static_cast<std::uint64_t>(sysconf(_SC_PAGESIZE))};
	const std::uint64_t aligned{base & ~(pageSize - 1)};
	const std::uint64_t delta{base - aligned};
	const std::size_t len{
	    static_cast<std::size_t>(info->status_queue_size + delta)};

	void* p{mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_SHARED, g_mem_fd,
		    static_cast<off_t>(aligned))};
	if (p == MAP_FAILED) {
		perror("[GPU INSTRUMENTATION] mmap " PHYS_MEM_DEVICE);
		fprintf(stderr,
			"[GPU INSTRUMENTATION] hint: kernel needs CONFIG_STRICT_DEVMEM=n\n");
		return nullptr;
	}

	g_map_raw = static_cast<std::uint8_t*>(p);
	g_map_raw_len = len;
	g_map = g_map_raw + delta;
	g_map_base = base;
	g_map_size = info->status_queue_size;
	return g_map;
}

//
// Bounds-checked pointer into the mapped region for an absolute base + offset.
//
std::uint8_t* regionPtr(const std::uint64_t base, const std::uint64_t offset,
			const std::uint64_t size)
{
	std::uint8_t* const map{ensureMapped(base)};
	if (map == nullptr) {
		return nullptr;
	}
	if (base < g_map_base) {
		errno = ERANGE;
		return nullptr;
	}
	const std::uint64_t rel{(base - g_map_base) + offset};
	if (size > g_map_size || rel > g_map_size - size) {
		errno = ERANGE;
		return nullptr;
	}
	return map + rel;
}

} // namespace

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
			     const std::uint64_t /*size*/,
			     const std::uint32_t value)
{
	std::uint8_t* const dst{regionPtr(base, offset, sizeof(value))};
	if (dst == nullptr) {
		return -1;
	}
	const std::array<std::uint8_t, 4> valueBuf{u32ToBuf(value)};
	std::memcpy(dst, valueBuf.data(), valueBuf.size());
	__sync_synchronize();
	return 0;
}

std::optional<std::vector<std::uint8_t>>
dumpMemoryRegion(const uint64_t base, const uint64_t offset, const uint64_t size)
{
	const std::uint8_t* const src{regionPtr(base, offset, size)};
	if (src == nullptr) {
		return std::nullopt;
	}
	//
	// Observe the GSP's latest DMA writes. On x86 WB memory is snoop-coherent
	// with the device, so a fence suffices - no clflush needed.
	//
	__sync_synchronize();
	std::vector<std::uint8_t> buf(size);
	std::memcpy(buf.data(), src, size);
	return buf;
}

int modifyMemoryRegion(const std::uint64_t base, const std::uint64_t offset,
		       const std::vector<std::uint8_t>& buffer)
{
	if (base > UINT64_MAX - offset) {
		errno = EOVERFLOW;
		return -1;
	}
	std::uint8_t* const dst{regionPtr(base, offset, buffer.size())};
	if (dst == nullptr) {
		return -1;
	}
	std::memcpy(dst, buffer.data(), buffer.size());
	//
	// Publish these stores before the caller's next write (notably the
	// writePtr bump that arms the message) and before the driver reads.
	//
	__sync_synchronize();
	return 0;
}
