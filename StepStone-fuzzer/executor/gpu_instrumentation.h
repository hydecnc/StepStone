#ifndef GPU_INSTRUMENTATION_H
#define GPU_INSTRUMENTATION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int instrument_gpu(void);
int instrument_gpu_elemcount(const uint64_t entry_index, const uint32_t elem_count);

#ifdef __cplusplus
}
#endif

#endif
