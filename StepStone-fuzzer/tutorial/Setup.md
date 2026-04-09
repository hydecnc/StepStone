# GPU Passthrough

## Identify GPU vendor id and device id
```
lspci -nnk

2b:00.0 VGA compatible controller [0300]: NVIDIA Corporation GA106 [Geforce RTX 3050] [10de:2507] (rev a1)
        Subsystem: Micro-Star International Co., Ltd. [MSI] GA106 [Geforce RTX 3050] [1462:c978]
        Kernel driver in use: vfio-pci
        Kernel modules: nouveau
2b:00.1 Audio device [0403]: NVIDIA Corporation GA106 High Definition Audio Controller [10de:228e] (rev a1)
        Subsystem: Micro-Star International Co., Ltd. [MSI] GA106 High Definition Audio Controller [1462:c978]
        Kernel driver in use: vfio-pci
        Kernel modules: snd_hda_intel
``` 
In my setup, the ID of the VGA is `10de:2507` and the ID of the Audio device is `10de:228e`. Their IOMMU group is `2b:00.0` and `2b:00.1` respectively.

## Isolated the GPU using VFIO

Writing the following configuration to `/etc/modprobe.d/vfio.conf` to blacklist the original driver and bind the GPU to the vfio-pci driver.
Replace the ids with the ids of the GPU you want to isolate.
```
blacklist nouveau
blacklist snd_hda_intel
options vfio-pci ids=10de:2507,10de:228e
```

# Environment Setup

A Linux based host mahcine is required to run our customized syzkaller. The following steps will guide you through the setup process.
## Prepare Linux Kernel and Image
Clone the Linux Stable Branch and select a stable kernel version (v6.12.X seems not supported by Nvidia yet)

```
cd ~
mkdir nvidia_bug_finding && cd nvidia_bug_finding
mkdir linux-deb && cd linux-deb

git clone https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git -b v6.8

mkdir image
```

## Prepare Kernel Configuration

Bring your own kernel configuration or use the one we provided.
The kernel config must enable `CONFIG_KASAN` and `CONFIG_KCOV` for kernel fuzzing.

```
cp ./tutorial/kernel_config ~/nvidia_bug_finding/linux-deb/linux/.config
```

## Build Kernel and GPU Driver

We were testing the latest Nvidia GPU driver at the time which is `560.35.03`, you are allowed to change the version in `tools/build-nvidia-gpu.sh` (Search for `GPU_MODULE_VERSION`).

Then locate the isolated VGA and Audio device IOMMU group, in our example, `2b:00.0` and `2b:00.1`.

Next, run the building script:
```
tools/deploy-gpu-fuzz-image.sh -l ~/nvidia_bug_finding/linux-deb/linux -i ~/nvidia_bug_finding/image/bookworm-kasan --vga-id 2b:00.0 --adu-id 2b:00.1 --vendor nvidia -v
```

This script generates a debian bookworm image as kernel filesystem image, and compile the kernel with given configuration. Before compiling the kernel, the script asks if it can delete the built deb package under `~/nvidia_bug_finding/linux-deb`, type `Y` or enter to continue.
After the kernel compilation is finished, the script boots up the kernel and install the Nvidia GPU drivers.

We provided a patch (`tools/gpu_module.patch`) to instrument the `open-gpu-kernel-modules` source code. It enables address sanitizer and kernel coverage during driver compilation, and we also insert an new compilation flag `PANIC_ON_ASSERT` to selectively panic on assertion failure. Additionally, we notice an Out-of-bound bug may happen when loading the nvidia modules, so we developed a patch to solve such OOB bug (We also submitted a bug report to Nvidia, titled "KASAN: global-out-of-bounds Read in memcmp").

The detailed GPU compilation script can be found at `tools/build-nvidia-gpu.sh`

# Build Syzkaller
Follow the official syzkaller instruction to setup Go environment: https://github.com/google/syzkaller/blob/master/docs/linux/setup_ubuntu-host_qemu-vm_x86-64-kernel.md

Use the following commands to build the customized syzkaller, make sure docker is installed on your machine.
Note that the CUDA library used to compile with syzkaller is v12.6.1.
This process involving downloading a ~10GB docker image, and may take a while to finish.

```
make clean
tools/syz-env make generate
tools/syz-env make nvidia
```

# Run Syzkaller
Need root user to allow GPU passthrough to the virtual machine, using `sudo` in this case. Copy `tutorial/cuda.cfg` to a work directory, replacing the path with your local setup, make changes to the "pci_video_device" and "pci_audio_device". If using multiple GPUs, change "count" to allow more fuzzing instance and adding corresponding GPU IOMMU group to "pci_video_device" and "pci_audio_device".
Run the following command to start the syzkaller.

```
sudo bin/syz-manager --config=workdir/cuda.cfg
```