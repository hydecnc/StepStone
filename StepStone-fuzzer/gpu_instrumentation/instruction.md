Your task is to provide incremental constraints to be used to discover new bugs in Nvidia's open gpu kernel modules.
The bugs that you will be looking for is a specific class of bugs, which assumes that the attacker has a malicious GPU.
That is, the attacker is assumed to have full read and write memory access to GPU memory (GDDR).
In this case, the attacker can attempt attacks on the host side utilizing GPU DMA.
Using GPU DMA, the attacker can access the IOVA region allowed by the IOMMU.
Critically, the IOVA region contains structs such as the message queue.
The message queue is a struct that the GSP and the driver utilizes for communication between them.
Focusing on the communication from the GSP to the driver, the GSP creates a message in what's called a status queue, which is 63 entries long where each entries are 4096 bytes large.
This time, we will focus on the status queue as well as its headers that stores information about the status queue itself.

As any fuzzer's goal is, your ultimate goal is to achieve deep states in the driver.
The versions of the driver used for this is 560.35.03 and linux kernel version 6.8.0.

Your job consists of three simple steps:

1. Analyze the run. Either identify the cause of the crash by parsing Nvidia OGKM code or linux source code, or, if the run produced no crash, identify the earliest point at which the injection stops making progress.
2. Utilize the primitives present under `gpu_instrumentation/` to create or modify a pseudo-system call which removes that blocker, hence allowing the fuzzer to explore more states.
3. Make a seed program, if necessary, to encourage the fuzzer to reach a similar state as before.

The three steps will be repeated as much as possible until we discover a crash/bug that is exploitable by the attacker.
I will type "!redo" in either of two situations, and you perform steps 1 to 3 in both:

- the fuzzer reported a crash, or
- the fuzzer ran for a long stretch without crashing and without reaching anything new.

The second case is the ordinary one in early rounds, because the first constraints are about reaching the target code at all rather than about breaking it. A quiet run is a round like any other; it is not a failed round and it is not a reason to ask me to wait longer.

On every "!redo" I will give you the executor's log of function calls, and the crash report if there is one.
If no crash report comes with the log, the log is from the quiet case. That is how you tell the two apart, not by size.
Either kind of log can be very large — hundreds of megabytes and millions of lines. Search with grep, awk and short scripts rather than reading end to end.
I will run the fuzzer; it lives on a remote workstation and you cannot execute it, build kernel modules, or read its workdir.

## Which crashes you constrain away, and which you do not

Two kinds of crash come out of this fuzzer and they are treated in opposite ways.

**Liveness failures.** The injection left the GPU or the driver unable to continue: a hang, a device reset, RPC timeouts, a queue that never recovers, the machine dying with no sanitizer output. These stop the fuzzer from exploring and they are what step 2 exists to eliminate.

**Memory-safety reports.** KASAN, general protection fault, BUG, UBSAN — anything carrying a kernel stack trace through driver code. These are the product. Never add a constraint whose effect is to stop one of these from happening. Report it to me and stop.

If you cannot decide which one you are holding, treat it as a memory-safety report and stop. A missed round costs an hour; a constraint that quietly suppresses the finding costs the whole experiment.

## Constraint discipline

The value of this method comes from adding as little as possible, as late as possible. So:

- Every constraint you add must cite the specific crash in the logs that motivated it.
- It must be the weakest constraint that removes that crash. If a range works, do not use a fixed value. If one field needs pinning, do not pin two.
- Never add a constraint speculatively, or because you believe a field "should" hold a particular value. If no crash motivated it, it does not go in.
- When you remove a degree of freedom from the fuzzer, say so explicitly and say what can no longer be reached.

You are not trying to reach a target you already have in mind. You are removing the reasons the fuzzer keeps dying, one at a time, and letting it tell you where it goes.

## Setup

The fuzzer is a modified instance of stock syzkaller, designed to utilize userspace libraries over pure system calls (e.g. `ioctl`) in hopes of more effectively achieving states that are:

- Actually achievable from function calls, not an obscure path that only a highly specific set of system calls can get to
- Solve dependency/constraints posed on the system calls

Many constraints will have to come in forms of a syzkaller description.
For help in syntax, read descriptions already written for other pseudo system calls under `sys/linux/`; they are the reference for what the syntax supports.
Some constraints will be static and others will be dynamic.

**static** — the constraint follows from the semantics of the source code and holds regardless of runtime state: accepted value sets, sizes, alignments, offsets, anything you can write directly into the syzkaller description.

**dynamic** — the constraint still comes from the source code, but its value is only knowable at runtime and has to be read live. The source tells you the relationship the value must satisfy; only the running machine tells you the value. The harness reads it and the fuzzer does not control it.

**mixed** — a single pseudo system call carrying both, which is what you get once a call has static structure wrapped around fields that must track live state.

For every pseudo system call that you create, you must add a prefix of `static` or `dynamic` to easily distinguish between the two types of constraints.
If in some case there is a mixture, explicitly label which parts are static and dynamic in the source code, with the function prefix being `mixed`.

Prefer static. A dynamic constraint takes a field away from the fuzzer for good, so it needs a reason that a static range cannot cover.

## Where things go

All syzkaller descriptions must go in `gpu_instrumentation/gpu_instrumentation.txt`.
All pseudo system call definitions and declarations must go in `gpu_instrumentation/gpu_instrumentation.cpp` and `gpu_instrumentation/gpu_instrumentation.h` respectively.
The executor-side wrapper for each pseudo system call goes in `executor/syz_nvidia.h`, guarded by `SYZ_EXECUTOR_NVIDIA`.
Seed programs go in `gpu_instrumentation/` as `.prog` files, in syzkaller program syntax.

Adding or removing a pseudo system call requires regenerating the descriptions before the fuzzer will build. Tell me when a change needs that. Also tell me when a change removes or renames a call, because saved corpus entries referencing it will be dropped on the next run.

## What to hand back each round

1. The root cause of the crash, with `file:line` into the driver or kernel source, and which of the two crash kinds it is.
2. The constraint you are adding, in what form, and whether it is static, dynamic, or mixed.
3. Which crash it removes, and your reasoning for why it does not also remove anything else.
4. The seed program, if one is needed, and what state you expect it to reach.
5. Anything you inferred rather than confirmed from source, marked as such.

Keep the analysis grounded in code you have actually read. If the logs are not enough to identify a cause, say that and tell me what additional output would settle it, rather than guessing.

## Scope

You modify the harness, the descriptions, and the seed programs. You do not patch the driver and you do not propose driver fixes; a bug that survives is the goal, not a defect to repair.

Work only from: this file, everything under `gpu_instrumentation/`, the syzkaller descriptions under `sys/linux/`, the executor sources, the logs and reports I give you, and the NVIDIA and Linux kernel source trees. Reading the driver source is expected and encouraged.

Do not read git history or commit messages.
