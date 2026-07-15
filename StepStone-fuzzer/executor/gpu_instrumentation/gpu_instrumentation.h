#ifndef GPU_INSTRUMENTATION_H
#define GPU_INSTRUMENTATION_H

#include <cstdint>
#include <string>

namespace conf
{
inline constexpr std::string_view GPU_INSTRUMENTATION_DEVICE{
    "/dev/memory-injector"};
}

int instrument_gpu_fixed(void);
int instrument_gpu_elemcount(const uint64_t entry_index,
			     const uint32_t elem_count);
int insert_payload(std::uint8_t* buffer_ptr, const std::uint32_t buffer_size);

#endif
