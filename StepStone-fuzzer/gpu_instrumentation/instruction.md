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

## How you are invoked

You are started once per round by `main.py`, non-interactively, with `claude -p`,
and your working directory is the loop root. There is no second turn. I cannot
answer a question, and nothing you say reaches me before the next fuzzing run
starts. So do not ask — state the assumption you are working under, mark it as an
assumption, and proceed. Every change you intend to make must be on disk before
you finish, and your final message is the round report.

The loop root contains:

```
instruction.md                  this file
main.py                         the loop driver
instances/instance-N/log        round N's executor log of function calls
instances/instance-N/report     round N's crash report, if there was one
syzkaller/                      the fuzzer (StepStone-fuzzer/, StepStone-generator/)
ogkm/                           the NVIDIA driver source, 560.35.03
```

Appended after this prompt are three lines identifying the round:

```
Round: N
Log: instances/instance-N/log
Report: instances/instance-N/report      (or the literal text below)
```

**If the text after `Report:` is exactly `Does not exist.`, the run produced no
crash.** That is how you tell the two cases below apart — not by size, and not by
the presence of scary-looking lines in the log.

Those are paths, not contents. Read the log from disk: it can be hundreds of
megabytes and millions of lines, so use grep, awk and short scripts, and never
read it end to end.

You are invoked in either of two situations, and you perform steps 1 to 3 in both:

- the fuzzer reported a crash (a `report` file exists), or
- the fuzzer ran for a long stretch without crashing and without reaching anything new (no `report` file).

The second case is the ordinary one in early rounds, because the first constraints are about reaching the target code at all rather than about breaking it. A quiet run is a round like any other; it is not a failed round and it is not a reason to tell me to wait longer.

I run the fuzzer. It lives on a remote workstation, so you cannot execute it, build kernel modules, or read its workdir. Both source trees here are yours to read and edit.

## Which crashes you constrain away, and which you do not

Two kinds of crash come out of this fuzzer and they are treated in opposite ways.

**Liveness failures.** The injection left the GPU or the driver unable to continue: a hang, a device reset, RPC timeouts, a queue that never recovers, the machine dying with no sanitizer output. These stop the fuzzer from exploring and they are what step 2 exists to eliminate.

**Memory-safety reports.** KASAN, general protection fault, BUG, UBSAN — anything carrying a kernel stack trace through driver code. These are the product. Never add a constraint whose effect is to stop one of these from happening. Report it and change nothing.

If you cannot decide which one you are holding, treat it as a memory-safety report and change nothing. A missed round costs an hour; a constraint that quietly suppresses the finding costs the whole experiment.

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
For help in syntax, read descriptions already written for other pseudo system calls under `syzkaller/StepStone-fuzzer/sys/linux/`; they are the reference for what the syntax supports.
Some constraints will be static and others will be dynamic.

**static** — the constraint follows from the semantics of the source code and holds regardless of runtime state: accepted value sets, sizes, alignments, offsets, anything you can write directly into the syzkaller description.

**dynamic** — the constraint still comes from the source code, but its value is only knowable at runtime and has to be read live. The source tells you the relationship the value must satisfy; only the running machine tells you the value. The harness reads it and the fuzzer does not control it.

**mixed** — a single pseudo system call carrying both, which is what you get once a call has static structure wrapped around fields that must track live state.

For every pseudo system call that you create, you must add a prefix of `static` or `dynamic` to easily distinguish between the two types of constraints.
If in some case there is a mixture, explicitly label which parts are static and dynamic in the source code, with the function prefix being `mixed`.

Prefer static. A dynamic constraint takes a field away from the fuzzer for good, so it needs a reason that a static range cannot cover.

## Where things go

All paths are relative to the loop root, which is your working directory.

| what | where |
|---|---|
| syzkaller descriptions | `syzkaller/StepStone-fuzzer/gpu_instrumentation/gpu_instrumentation.txt` |
| pseudo system call definitions | `syzkaller/StepStone-fuzzer/gpu_instrumentation/gpu_instrumentation.cpp` |
| their declarations | `syzkaller/StepStone-fuzzer/gpu_instrumentation/gpu_instrumentation.h` |
| executor-side wrapper, guarded by `SYZ_EXECUTOR_NVIDIA` | `syzkaller/StepStone-fuzzer/executor/syz_nvidia.h` |
| seed programs, syzkaller program syntax | `syzkaller/StepStone-fuzzer/gpu_instrumentation/*.prog` |
| driver source | `ogkm/` |

`executor/gpu_instrumentation` and `sys/linux/gpu_instrumentation.txt` are symlinks into that one `gpu_instrumentation/` directory. Edit the real files listed above, not the symlinked views.

**I commit for you.** After you finish, the loop runs `git add -A` and commits in both `syzkaller/` and `ogkm/`. Two consequences: do not run git commands that change state, and do not leave scratch files inside either repo — write anything temporary to the loop root or `/tmp`, or it lands in the round's commit.

Adding or removing a pseudo system call requires regenerating the descriptions before the fuzzer will build. Say so when a change needs that. Also say so when a change removes or renames a call, because saved corpus entries referencing it will be dropped on the next run.

## The ledger

Each round starts a fresh agent with no memory of the previous ones. The tree records what was added; nothing records what was tried and reverted. That is what `syzkaller/StepStone-fuzzer/gpu_instrumentation/LEDGER.md` is for. Before you finish, do two things to it:

1. Fill in the `outcome` field of the last line, which is the constraint the previous round added and this round's log is the verdict on. It is one of: `reached new code` / `no change` / `reverted`.
2. Append one line for the round you just analysed, with `outcome` left as `pending`:

```
round | blocker (file:line) | constraint added | static/dynamic/mixed | outcome
```

No prose in the ledger. Prose is how a record turns back into a hypothesis that the next round then spends itself confirming. If the file does not exist yet, create it with that header line.

## What to hand back each round

1. What the round was: a crash, or a quiet run.
   - **Crash** — the root cause, with `file:line` into the driver or kernel source, and which of the two crash kinds it is.
   - **Quiet run** — the earliest point at which the injection stops making progress: which arguments actually varied, which values survived into the corpus, and the first check on the path from the injection site to the target code that turns the input away. Estimate the probability that random bytes pass that check. The estimate is what separates a real bottleneck from an assumed one, so give it even when it is rough.
2. The constraint you are adding, in what form, and whether it is static, dynamic, or mixed.
3. Which blocker or crash it removes, and your reasoning for why it does not also remove anything else.
4. The seed program, if one is needed, and what state you expect it to reach.
5. Anything you inferred rather than confirmed from source, marked as such — including any assumption you had to make because you could not ask.
6. The ledger lines you wrote.

Keep the analysis grounded in code you have actually read. If the log is not enough to identify a cause, say that and say what additional output would settle it, rather than guessing.

## Scope

You modify the harness, the descriptions, and the seed programs. You do not patch the driver and you do not propose driver fixes; a bug that survives is the goal, not a defect to repair.

Work only from: this file, everything under `syzkaller/StepStone-fuzzer/gpu_instrumentation/`, the syzkaller descriptions under `syzkaller/StepStone-fuzzer/sys/linux/`, the executor sources, this round's log and report, and the NVIDIA and Linux kernel source trees. Reading the driver source is expected and encouraged.

Do not save anything to memory. Each round is meant to start from the tree and
this round's log alone; a memory written in one round is loaded by the next and
becomes exactly the accumulated hypothesis this design removes. The ledger is the
only thing that carries across rounds.

Do not read git history or commit messages in `ogkm/`. It is stock vendor source and its history is not part of this experiment. `syzkaller/` is the opposite case: its history is the loop's own record, one commit per round, and reading `git log` and `git diff` there is expected.
