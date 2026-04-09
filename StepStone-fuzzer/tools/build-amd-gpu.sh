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

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_AMDGPU" ]; then
    apt-get update
    apt-get install -y build-essential cmake curl g++-multilib gcc-multilib git ninja-build pkg-config python3 python3-jinja2 python3-ruamel.yaml
    apt-get install -y libssl-dev libx11-dev libxcb1-dev x11proto-dri2-dev libxcb-dri3-dev libxcb-dri2-0-dev libxcb-present-dev libxshmfence-dev libxrandr-dev libwayland-dev
    apt-get install -y wget libgl1-mesa-dri libglx-mesa0 mesa-vulkan-drivers xserver-xorg-video-all pciutils libvulkan1 libvulkan-dev ocl-icd-opencl-dev

    wget https://sdk.lunarg.com/sdk/download/1.3.239.0/linux/vulkansdk-linux-x86_64-1.3.239.0.tar.gz

    touch /root/.build_gpu/.stamp/INSTALL_AMDGPU
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_KERNEL_DEB" ]; then
    cd /root/linux-deb
    dpkg -i linux-*.deb
    cp System.map /lib/modules/`uname -r`/build
    rm linux-*
    rm System.map
    touch /root/.build_gpu/.stamp/INSTALL_KERNEL_DEB
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_AMDGPU" ]; then
    apt-get update
    apt-get install -y build-essential git wget pciutils patch xz-utils g++ g++-multilib gcc-multilib software-properties-common curl libc6-dev
    apt-get install -y mesa-opencl-icd opencl-headers ocl-icd-opencl-dev

    touch /root/.build_gpu/.stamp/INSTALL_AMDGPU
fi

if [ ! -f "/root/.build_gpu/.stamp/UPDATE_FIRMWARE" ]; then
    cd /tmp
    wget https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git/snapshot/linux-firmware-20230310.tar.gz
    tar xf linux-firmware-20230310.tar.gz
    mv /tmp/linux-firmware-20230310 /lib/firmware

    touch /root/.build_gpu/.stamp/UPDATE_FIRMWARE
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

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_OPENGL" ]; then
    apt-get update
    apt-get install -y mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev
    apt-get install -y libglew-dev libglfw3-dev libglm-dev libgbm-dev
    apt-get install -y libao-dev libmpg123-dev

    touch /root/.build_gpu/.stamp/INSTALL_OPENGL
fi

rm /root/.build_gpu/update
pkill -f "journalctl -fu build-gpu-modules.service" || true
exit 0