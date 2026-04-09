#!/bin/bash
# Xiaochen Zou 2024, University of California-Riverside
#
# Usage ./build-kernel.sh

function exit_with_error() {
    error=$1
    echo "ERROR: $error"
    exit 1
}

function check_config() {
    config=$1
    grep $config=y .config || (echo "$config is not enabled" && exit 1)
}
SKIP_BUILD=0
VENDOR=""
while true; do
    if [ $# -eq 0 ];then
	break
    fi
    case "$1" in
        -h | --help)
            display_help
            exit 0
            ;;
        -l | --linux)
	        LINUX=$2
            shift 2
            ;;
        -i | --image)
	        IMAGE=$2
            shift 2
            ;;
        --vendor)
	        VENDOR=$2
            shift 2
            ;;
        -v | --verbose)
	        set -ex
            shift 1
            ;;
        --skip-build)
            SKIP_BUILD=1
            shift 1
            ;;
        --vga-id)
            VGA_ID=$2
            shift 2
            ;;
        --adu-id)
            ADU_ID=$2
            shift 2
            ;;
        -*)
            echo "Error: Unknown option: $1" >&2
            exit 1
            ;;
        *)  # No more options
            break
            ;;
    esac
done

if [ "$LINUX" == "" ]; then
    echo "use -l or --linux to specifiy a Linux source directory"
    exit 0
fi

if [ "$IMAGE" == "" ]; then
    echo "use -i or --image to specifiy a filesystem image path"
    exit 0
else
    qemu-img info $IMAGE || exit_with_error "filesystem image \"$IMAGE\" seems to be broken"
fi

if [ "$VENDOR" == "nvidia" ]; then
    RELEASE=bookworm
fi

if [ "$VENDOR" == "amd" ]; then
    RELEASE=noble
fi

if [ "$RELEASE" == "" ]; then
    echo "use --vendor to specifiy a vendor [nvidia, amd]"
    exit 0
fi

if [ "$SKIP_BUILD" == "0" ]; then
    PARENT_D=$(dirname "$LINUX")
    echo "This script will delete all linux-* files in the parent folder(\"$PARENT_D\")."
    echo "Move Linux source folder to a safe 2nd level directory for safety concerns."

    read -p "Do you want to proceed? (Y/N): " choice

    choice=$(echo "$choice" | tr '[:lower:]' '[:upper:]')
    if [ "$choice" == "" ]; then
        choice="Y"
    fi

    if [ "$choice" != "Y" ]; then
        exit 0
    fi

    grep "Linux kernel" $LINUX/README || exit_with_error "$LINUX is not a Linux source directory"
    cd $PARENT_D

    echo "Cleaning previous built package in $PARENT_D"
    rm linux-* || true

    cd $LINUX
    make olddefconfig
    
    check_config "CONFIG_KASAN"
    check_config "CONFIG_KCOV"
    check_config "CONFIG_KPROBES"
    check_config "CONFIG_DEBUG_INFO"

    echo "Building Linux kernel packages"
    sleep 1
    make deb-pkg -j`nproc` || exit_with_error "Building kernel encounters errors"

    sudo mkdir -p /mnt/chroot
    sudo mount -o loop $IMAGE /mnt/chroot
    sudo mkdir -p /mnt/chroot/root/linux-deb
    sudo cp -a ../linux-* /mnt/chroot/root/linux-deb
    sudo cp -a ./System.map /mnt/chroot/root/linux-deb
    sudo touch /mnt/chroot/root/.build_gpu/update
    sudo rm /mnt/chroot/root/.build_gpu/.stamp/BUILD_GPU_MODULES || true
    sudo umount /mnt/chroot
fi

nohup sudo qemu-system-x86_64 -m 8G -smp 8 \
    -net nic,model=e1000 -net user,host=10.0.2.10,hostfwd=tcp::3777-:22 \
    -display none -serial stdio -no-reboot \
    -device vfio-pci,host=$VGA_ID,multifunction=on,x-vga=on -device vfio-pci,host=$ADU_ID \
    -enable-kvm -cpu host,migratable=off \
    -hda $IMAGE -kernel $LINUX/arch/x86_64/boot/bzImage \
    -append "root=/dev/sda console=ttyS0 earlyprintk=serial" &

IMAGE_DIR=$(dirname "$IMAGE")
KEY=$IMAGE_DIR/$RELEASE.id_rsa
SSH_CMD="ssh -F /dev/null -o UserKnownHostsFile=/dev/null -o BatchMode=yes -o \
    IdentitiesOnly=yes -o StrictHostKeyChecking=no -i $KEY \
    -p 3777 root@localhost"
SCP_CMD="scp -F /dev/null -o UserKnownHostsFile=/dev/null -o BatchMode=yes \
    -o IdentitiesOnly=yes -o StrictHostKeyChecking=no -i $KEY \
    -P 3777"

for i in {1..20}
do
    sleep 10
    $SSH_CMD "/root/.build_gpu/monitor-service.sh" || continue
    break
done

if [ $VENDOR == "nvidia" ]; then
    for i in {1..10}
    do
        sleep 10
        $SCP_CMD root@localhost:/tmp/open-gpu-kernel-modules.tar.gz $LINUX || continue
        break
    done

    set -ex
    cd $LINUX
    rm -rf ./open-gpu-kernel-modules
    tar xf open-gpu-kernel-modules.tar.gz
    rm open-gpu-kernel-modules.tar.gz
fi

if [ $VENDOR == "amd" ]; then
    cd $LINUX
    KERNEL_VERSION=`find ./debian -maxdepth 1 -name 'linux-image-*-dbg' | awk -F'-' '{print $3}'`
    
    cp drivers/gpu/drm/amd/amdgpu/amdgpu.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/amd/amdgpu/amdgpu.ko
    cp drivers/gpu/drm/amd/amdxcp/amdxcp.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/amd/amdxcp/amdxcp.ko
    cp drivers/gpu/drm/drm_exec.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/drm_exec.ko
    cp drivers/gpu/drm/drm_suballoc_helper.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/drm_suballoc_helper.ko
    cp drivers/gpu/drm/radeon/radeon.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/radeon/radeon.ko
    cp drivers/gpu/drm/scheduler/gpu-sched.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/scheduler/gpu-sched.ko

    cp drivers/gpu/drm/amd/amdgpu/amdgpu.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/amd/amdgpu/amdgpu.ko
    cp drivers/gpu/drm/amd/amdxcp/amdxcp.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/amd/amdxcp/amdxcp.ko
    cp drivers/gpu/drm/drm_exec.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/drm_exec.ko
    cp drivers/gpu/drm/drm_suballoc_helper.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/drm_suballoc_helper.ko
    cp drivers/gpu/drm/radeon/radeon.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/radeon/radeon.ko
    cp drivers/gpu/drm/scheduler/gpu-sched.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/scheduler/gpu-sched.ko
fi

if [ $VENDOR == "intel" ]; then
    cd $LINUX
    KERNEL_VERSION=`find ./debian -maxdepth 1 -name 'linux-image-*-dbg' | awk -F'-' '{print $3}'`
    
    cp drivers/gpu/drm/i915/i915.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/i915/i915.ko
    cp drivers/gpu/drm/xe/xe.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/xe/xe.ko
    cp drivers/gpu/drm/drm_gpuvm.ko debian/linux-image-$KERNEL_VERSION-dbg/usr/lib/debug/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/drm_gpuvm.ko

    cp drivers/gpu/drm/i915/i915.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/i915/i915.ko
    cp drivers/gpu/drm/xe/xe.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/xe/xe.ko
    cp drivers/gpu/drm/drm_gpuvm.ko debian/linux-image-$KERNEL_VERSION/lib/modules/$KERNEL_VERSION/kernel/drivers/gpu/drm/drm_gpuvm.ko
fi

$SSH_CMD "shutdown -h now"