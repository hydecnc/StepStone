#ifndef GPU_INSTRUMENTATION_H
#define GPU_INSTRUMENTATION_H

#include <cstdint>
#include <string>

//
// GSP status queue injection primitives.
//
// All three are "mixed": the status queue base address is resolved live per
// call (dynamic), while every offset/slot bound below is a static fact about
// the layout described in gpu_instrumentation.txt.
//
// None of them can reach the shared-memory page table or the command queue.
// Those are GSP-side inputs; injecting into them faults the GSP (Xid 120,
// "GSP task exception") and takes the device out for the rest of the run.
//

// Raw write of `size` bytes at status_queue_base + offset. Clamped to the
// status queue. `buffer` is handed to the injector unread, so a size that does
// not match the mapping yields EFAULT rather than faulting the executor.
int sqWrite(const void* buffer, const std::uint64_t size, const std::uint64_t offset);

// Store `writePtr` into msgqTxHeader.writePtr at status_queue_base + 0x10.
int sqSetWritePtr(const std::uint32_t writePtr);

// Write one 0x1000-byte GSP_MSG_QUEUE_ELEMENT into entry slot `slot`.
// `element` must point at GSP_MSG_QUEUE_ELEMENT_SIZE_MIN readable bytes; the
// syzkaller description guarantees that by using a fixed-size struct.
int sqWriteElement(const std::uint32_t slot, const void* element,
		   const bool fixCheckSum);

#endif
