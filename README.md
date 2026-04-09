# StepStone

StepStone contains two main components for GPU syscall fuzzing:

- `StepStone-generator`: generates syzkaller descriptions from API schema files with the help of an LLM.
- `StepStone-fuzzer`: a modified `syzkaller` tree used to build and run GPU-enabled fuzzing targets.

## Repository Layout

```text
StepStone-latest/
├── StepStone-generator/
└── StepStone-fuzzer/
```

## Component 1: StepStone-generator

`StepStone-generator` reads API declarations line by line and generates syzkaller descriptions for the full input file.

Example input:

```c
CUresult cuKernelGetAttribute ( int* pi, CUfunction_attribute attrib, CUkernel kernel, CUdevice dev )
CUresult cuKernelGetFunction ( CUfunction* pFunc, CUkernel kernel )
CUresult cuKernelGetLibrary ( CUlibrary* pLib, CUkernel kernel )
CUresult cuKernelGetName ( const char** name, CUkernel hfunc )
CUresult cuKernelGetParamInfo ( CUkernel kernel, size_t paramIndex, size_t* paramOffset, size_t* paramSize )
CUresult cuKernelSetAttribute ( CUfunction_attribute attrib, int  val, CUkernel kernel, CUdevice dev )
CUresult cuKernelSetCacheConfig ( CUkernel kernel, CUfunc_cache config, CUdevice dev )
CUresult cuLibraryEnumerateKernels ( CUkernel* kernels, unsigned int  numKernels, CUlibrary lib )
CUresult cuLibraryGetGlobal ( CUdeviceptr* dptr, size_t* bytes, CUlibrary library, const char* name )
CUresult cuLibraryGetKernel ( CUkernel* pKernel, CUlibrary library, const char* name )
CUresult cuLibraryGetKernelCount ( unsigned int* count, CUlibrary lib )
CUresult cuLibraryGetManaged ( CUdeviceptr* dptr, size_t* bytes, CUlibrary library, const char* name )
CUresult cuLibraryGetModule ( CUmodule* pMod, CUlibrary library )
```

### Setup

```bash
cd StepStone-generator
git submodule update --init --recursive
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

The generator uses the Python package entry point in `StepStone-generator/generator/`. You can configure the LLM interactively:

```bash
cd StepStone-generator
python -m generator generate --setup
```

This setup flow can create or update `config.json` with the API key used by the generator.

### Generate Syzkaller Descriptions

Run the generator on a file that contains one API declaration per line:

```bash
cd StepStone-generator
python -m generator generate \
  --llm chatgpt \
  --engine gpt-4o \
  --api-file /path/to/apis.txt \
  --output out \
  --verbose
```

You can also generate from a single declaration with `--api`, but the normal workflow is to pass `--api-file` and let the tool process the full file line by line.

## Component 2: StepStone-fuzzer

`StepStone-fuzzer` is a modified `syzkaller` tree for GPU fuzzing.

Its Go module path is `github.com/google/syzkaller`, so it should be placed in an appropriate `GOPATH` location before building, for example:

```bash
mkdir -p "$GOPATH/src/github.com/google"
cp -r StepStone-fuzzer "$GOPATH/src/github.com/google/syzkaller"
cd "$GOPATH/src/github.com/google/syzkaller"
```

### Integrating Generated Descriptions

Copy the generated syzkaller descriptions into `StepStone-fuzzer/sys/linux/`.

This repository already includes generated GPU-related descriptions there, including Nvidia and OpenCL descriptions, and Vulkan-related description files are also present under the same directory.

## Building a GPU Fuzzing Image

To fuzz GPU interfaces, you need a target kernel with the relevant GPU modules enabled. The helper script is:

- `StepStone-fuzzer/tools/deploy-gpu-fuzz-image.sh`

The script expects:

- `--linux` / `-l`: path to the Linux kernel source tree
- `--image` / `-i`: directory used to build the guest image
- `--vendor`: GPU vendor, currently `nvidia` or `amd`

Example:

```bash
cd StepStone-fuzzer
./tools/deploy-gpu-fuzz-image.sh \
  --linux /path/to/linux \
  --image /path/to/image-dir \
  --vendor nvidia
```

Useful optional flags include:

- `--vga-id <pci-id>`
- `--adu-id <pci-id>`
- `--rebuild-gpu <patch>`
- `--skip-kernel-build`
- `--panic-on-assert`
- `--skip-on-assert`
- `-v` / `--verbose`

The deployment script prepares the guest image, installs helper scripts, enables a systemd service that builds GPU modules inside the guest, and then invokes `tools/build-kernel.sh`.

## Typical Workflow

1. Prepare `StepStone-generator` and configure LLM access.
2. Generate syzkaller descriptions from an API schema file.
3. Copy the generated descriptions into `StepStone-fuzzer/sys/linux/`.
4. Place `StepStone-fuzzer` under the proper `GOPATH` path.
5. Build a GPU-enabled kernel and image with `tools/deploy-gpu-fuzz-image.sh`.
6. Build and run the modified syzkaller environment as needed for your fuzzing setup.
