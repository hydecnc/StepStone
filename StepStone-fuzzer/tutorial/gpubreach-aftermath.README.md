# Aftermath track: exploring where the overflowed bytes go

Goal: after the GSP status-queue overflow corrupts `pMetaData`, discover the
downstream driver paths the poisoned bytes reach (beyond the paper's
`pReadOutgoing`/`rxReadPtr` 4-byte write).

## Build the guest DIFFERENTLY from the reproduction build

- **`GPU_INSTRUMENTATION` must be OFF** (the `#else` branch in
  `message_queue_cpu.c:135`, original `pWorkArea` layout). Only then does the
  overflow actually land on `pMetaData` (in-bounds within `pWorkArea`). The
  de-embed/redzone build used for reproduction *blocks* the aftermath — the
  bytes hit the redzone and never reach `pMetaData`.
- **Keep generic KASAN ON.** It stays silent on the in-bounds overflow but
  becomes the *downstream* oracle: it fires when the corrupted `pMetaData`
  pointers are later used out of bounds (or you get a GPF/panic).
- Keep `elemCount = 17` (the seed uses 16 pages). A larger elemCount overruns
  past `pWorkArea`'s end and trips a redzone there instead of corrupting
  `pMetaData` — a different bug, not this track.

## Why the config is wide (opposite of gpubreach.cfg)

`gpubreach.cfg` is trimmed so the fuzzer reliably *forms* inject+trigger.
Here the seed pins that prefix, so a wide `enable_syscalls` is intentional:
after the corruption, the extra CUDA ops (memcpy / kernel launch / streams /
events / alloc) drive further GSP queue processing, carrying the poisoned
`pMetaData` into diverse driver code. Uses a separate workdir + http port so
its (seeded, wide) corpus does not mix with the focused track's.

## The seed program

`gpubreach-aftermath.prog`:
1. `cuDeviceGet` -> `cuDevicePrimaryCtxRetain` -> `cuCtxSetCurrent` (GPU/GSP alive)
2. `syz_gpu_insert_payload(16 pages)` -> writes 17 elements + bumps writePtr =>
   the 17th element (the last fuzzer-controlled page) overwrites `pMetaData`.
3. `cuDeviceGetAttribute` x2 -> provoke the driver to service the queue.

Pack it into the aftermath workdir before starting syz-manager:

```
mkdir -p /tmp/gsp-seed && cp gpubreach-aftermath.prog /tmp/gsp-seed/
bin/syz-db pack /tmp/gsp-seed <workdir-aftermath>/corpus.db
```

VALIDATE FIRST (no go toolchain was available where this was authored): run the
program through syzkaller's deserializer (e.g. `syz-db unpack` round-trip, or
`syz-execprog -collide=0 gpubreach-aftermath.prog`). The line most likely to
need adjustment is the `insert_payload` buffer: `[{""} x16]` assumes syzkaller
zero-pads each fixed `array[int8, 0x1000]`. If rejected, either expand to
explicit zero bytes, or — more robust — **harvest the exact serialized program
from a reproduction run** (syz-manager logs executed programs) and reuse that.

## Stronger alternative: VM snapshot after injection

A seed is only a *starting point* — the fuzzer can mutate the injection prefix
away and drift off the corrupted state. A QEMU snapshot taken right after
`insert_payload` corrupts `pMetaData` is a *guarantee*: `loadvm` back to that
state each iteration and fuzz only the suffix. syzkaller does not do this out of
the box, so it is custom harnessing (savevm/loadvm loop around the executor),
but it decouples "reach the corrupted state" from "fuzz the consequences" and is
worth doing once hardware is available. Keep both: seed now, snapshot later.
