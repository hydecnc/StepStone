#!/bin/bash
# Xiaochen Zou 2024, University of California-Riverside
#
# Usage ./build-amd-gpu.sh

set -ex
if [ ! -f "/root/.build_gpu/update" ]; then
    exit 0
fi

netplan apply
cd /root
mkdir -p /root/.build_gpu/.stamp || true

if [ ! -f "/root/.build_gpu/.stamp/UPDATE_FIRMWARE" ]; then
    cd /tmp
    wget https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git/snapshot/linux-firmware-20241210.tar.gz
    tar xf linux-firmware-20241210.tar.gz
    mv /tmp/linux-firmware-20241210 /lib/firmware

    touch /root/.build_gpu/.stamp/UPDATE_FIRMWARE
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_PACKAGES" ]; then
    apt-get update
    apt-get install -y build-essential cmake curl g++-multilib gcc-multilib git ninja-build pkg-config python3 python3-jinja2 python3-ruamel.yaml
    apt-get install -y libssl-dev libx11-dev libxcb1-dev x11proto-dri2-dev libxcb-dri3-dev libxcb-dri2-0-dev libxcb-present-dev libxshmfence-dev libxrandr-dev libwayland-dev
    apt-get install -y wget libgl1-mesa-dri libglx-mesa0 mesa-vulkan-drivers xserver-xorg-video-all pciutils libvulkan1 libvulkan-dev ocl-icd-opencl-dev
    apt-get install -y libatspi2.0-0 xdg-utils libnss3 libglib2.0-bin libgbm1

    touch /root/.build_gpu/.stamp/INSTALL_PACKAGES
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_KERNEL_DEB" ]; then
    cd /root/linux-deb
    dpkg -i linux-*.deb
    cp System.map /lib/modules/`uname -r`/build
    rm linux-*
    rm System.map
    touch /root/.build_gpu/.stamp/INSTALL_KERNEL_DEB
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_OPENCL" ]; then
    apt-get update
    apt-get install -y build-essential git wget pciutils patch xz-utils g++ g++-multilib gcc-multilib software-properties-common curl libc6-dev
    apt-get install -y opencl-headers ocl-icd-opencl-dev
    wget https://github.com/intel/intel-graphics-compiler/releases/download/v2.5.6/intel-igc-core-2_2.5.6+18417_amd64.deb
    wget https://github.com/intel/intel-graphics-compiler/releases/download/v2.5.6/intel-igc-opencl-2_2.5.6+18417_amd64.deb
    wget https://github.com/intel/compute-runtime/releases/download/24.52.32224.5/intel-level-zero-gpu-dbgsym_1.6.32224.5_amd64.ddeb
    wget https://github.com/intel/compute-runtime/releases/download/24.52.32224.5/intel-level-zero-gpu_1.6.32224.5_amd64.deb
    wget https://github.com/intel/compute-runtime/releases/download/24.52.32224.5/intel-opencl-icd-dbgsym_24.52.32224.5_amd64.ddeb
    wget https://github.com/intel/compute-runtime/releases/download/24.52.32224.5/intel-opencl-icd_24.52.32224.5_amd64.deb
    wget https://github.com/intel/compute-runtime/releases/download/24.52.32224.5/libigdgmm12_22.5.5_amd64.deb
    dpkg -i *.deb

    touch /root/.build_gpu/.stamp/INSTALL_OPENCL
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_VULKAN" ]; then
    apt-get install -y vulkan-tools
    wget https://sdk.lunarg.com/sdk/download/1.3.239.0/linux/vulkansdk-linux-x86_64-1.3.239.0.tar.gz
    tar xf vulkansdk-linux-x86_64-1.3.239.0.tar.gz
    cp -r 1.3.239.0/x86_64/include/vulkan/ /usr/local/include/
    cp -r 1.3.239.0/x86_64/include/vk_video/ /usr/local/include/
    cp -P 1.3.239.0/x86_64/lib/libvulkan.so* /usr/local/lib/
    cp 1.3.239.0/x86_64/lib/libVkLayer_*.so /usr/local/lib/
    mkdir -p /usr/local/share/vulkan/explicit_layer.d
    cp 1.3.239.0/x86_64/etc/vulkan/explicit_layer.d/VkLayer_*.json /usr/local/share/vulkan/explicit_layer.d
    ldconfig

    touch /root/.build_gpu/.stamp/INSTALL_VULKAN
fi

rm /root/.build_gpu/update
pkill -f "journalctl -fu build-gpu-modules.service" || true
exit 0