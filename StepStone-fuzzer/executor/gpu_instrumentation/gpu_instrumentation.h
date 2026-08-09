#ifndef GPU_INSTRUMENTATION_H
#define GPU_INSTRUMENTATION_H

#include <cstdint>
#include <string>

int setElemcount(const uint64_t slotOffset, const uint32_t elemCount);
int insertPayload(std::uint8_t* buffer, const std::uint32_t bufferSize);

#endif
