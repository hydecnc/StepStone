#!/bin/bash
# Xiaochen Zou 2024, University of California-Riverside
#
# Usage ./build-nvidia-gpu.sh build_path

set -ex

function exit_with_error() {
    error=$1
    echo "ERROR: $error"
    exit 1
}

CUDA_VERSION="12.6.1"
GPU_MODULE_VERSION="560.35.03"

version=$(echo $CUDA_VERSION | grep -o '[^-]*$')
VERSION_MAJOR=$(echo $version | cut -d. -f1)
VERSION_MINOR=$(echo $version | cut -d. -f2)

if [ ! -f "/root/.build_gpu/update" ]; then
    exit 0
fi

BUILD_PATH=$1
FLAG_PANIC_ON_ASSERT=`ls /root/.build_gpu/.panic_on_assert` || true
if [ "$BUILD_PATH" == "" ]; then
    echo "Wrong buid path \"$BUILD_PATH\""
    exit 1
fi

cd /root
mkdir -p /root/.build_gpu/.stamp || true
export HOME=/root
if [ ! -f "/root/.build_gpu/.stamp/CLONE_GPU_MODULES" ]; then
    apt-get update
    apt-get install -y git wget pciutils patch xz-utils g++ software-properties-common glslang-dev libglfw3-dev glslang-tools

    mkdir -p $BUILD_PATH
    cd $BUILD_PATH
    git config --global http.sslverify false
    git clone https://github.com/NVIDIA/open-gpu-kernel-modules.git
    touch /root/.build_gpu/.stamp/CLONE_GPU_MODULES
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_KERNEL_DEB" ]; then
    cd /root/linux-deb
    dpkg -i linux-*.deb
    cp System.map /lib/modules/`uname -r`/build
    rm linux-*
    rm System.map
    touch /root/.build_gpu/.stamp/INSTALL_KERNEL_DEB
fi

if [ ! -f "/root/.build_gpu/.stamp/BUILD_GPU_MODULES" ]; then
    cd $BUILD_PATH/open-gpu-kernel-modules
    git stash
    git checkout $GPU_MODULE_VERSION
    make clean
    cp /root/.build_gpu/gpu_module.patch ./
    patch -p1 -i ./gpu_module.patch || exit_with_error "Cannot apply patch to GPU modules"
    if [ -z "$FLAG_PANIC_ON_ASSERT" ]; then
        make modules DEBUG=1 -j`nproc` || exit_with_error "Compiling GPU modules has failed"
    else
        make modules DEBUG=1 PANIC_ON_ASSERT=1 -j`nproc` || exit_with_error "Compiling GPU modules has failed"
    fi
    make modules_install -j`nproc` || exit_with_error "Installing GPU modules has failed"
    touch /root/.build_gpu/.stamp/BUILD_GPU_MODULES
fi

if [ ! -f "/root/.build_gpu/.stamp/INSTALL_GPU_DRIVER" ]; then
    cd /tmp
    wget -nv https://download.nvidia.com/XFree86/Linux-x86_64/$GPU_MODULE_VERSION/NVIDIA-Linux-x86_64-$GPU_MODULE_VERSION.run
    chmod +x NVIDIA-Linux-x86_64-$GPU_MODULE_VERSION.run
    sh ./NVIDIA-Linux-x86_64-$GPU_MODULE_VERSION.run -s --no-kernel-modules || exit_with_error "Installing GPU drivers has failed"

    wget -nv https://developer.download.nvidia.com/compute/cuda/$CUDA_VERSION/local_installers/cuda-repo-debian12-$VERSION_MAJOR-$VERSION_MINOR-local_$CUDA_VERSION-$GPU_MODULE_VERSION-1_amd64.deb
    dpkg -i cuda-repo-debian12-$VERSION_MAJOR-$VERSION_MINOR-local_$CUDA_VERSION-$GPU_MODULE_VERSION-1_amd64.deb
    cp /var/cuda-repo-debian12-$VERSION_MAJOR-$VERSION_MINOR-local/cuda-*-keyring.gpg /usr/share/keyrings/
    add-apt-repository -y contrib
    apt-get update
    apt-get -y install cuda-toolkit-$VERSION_MAJOR-$VERSION_MINOR
    touch /root/.build_gpu/.stamp/INSTALL_GPU_DRIVER
fi

rm /root/.build_gpu/update

tar -C $BUILD_PATH -czf /tmp/open-gpu-kernel-modules.tar.gz ./open-gpu-kernel-modules
pkill -f "journalctl -fu build-gpu-modules.service" || true
exit 0