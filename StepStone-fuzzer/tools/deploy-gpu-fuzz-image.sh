#!/bin/bash
# Xiaochen Zou 2024, University of California-Riverside
#
# Usage ./deploy-gpu-fuzz-image.sh

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
            DEBUG="-v"
	        set -ex
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
        --rebuild-gpu)
            GPU_SRC_PATCH=$2
            SKIP_BUILD="--skip-build"
            shift 2
            ;;
	--skip-kernel-build)
	    SKIP_BUILD="--skip-build"
	    shift 1
	    ;;
        --panic-on-assert)
            FLAG_PANIC_ON_ASSERT=1
            shift 1
            ;;
        --skip-on-assert)
            FLAG_SKIP_ON_ASSERT=1
            shift 1
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
    echo "use -i or --image to specifiy a directory for generating filesystem image"
    exit 0
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

SYZ_TOOLS=`dirname $0`
mkdir -p $IMAGE
HOME_DIR=`pwd`
mkdir -p $IMAGE/.stamp
if [ ! -f "$IMAGE/.stamp/IMAGE_SETUP" ]; then
    mkdir -p $IMAGE
    cp $SYZ_TOOLS/create-$VENDOR-image.sh $IMAGE
    cd $IMAGE
    chmod +x ./create-$VENDOR-image.sh
    ./create-$VENDOR-image.sh -s 30720 -f full -d $RELEASE
    touch $IMAGE/.stamp/IMAGE_SETUP
else
    echo "Found $IMAGE/.stamp/IMAGE_SETUP"
fi

if [ ! -f "$IMAGE/.stamp/GUEST_SCRIPT_READY" ]; then
    cd $HOME_DIR
    sudo mkdir -p /mnt/chroot
    sudo mount -o loop $IMAGE/$RELEASE.img /mnt/chroot
    sudo mkdir -p /mnt/chroot/root/.build_gpu

    cat << EOF > /tmp/build-gpu-modules.service
[Unit]
Description=Build GPU modules

[Service]
ExecStart=sh -c "/root/.build_gpu/build-$VENDOR-gpu.sh $LINUX"
Type=oneshot
RemainAfterExit=no

[Install]
WantedBy=multi-user.target
EOF

    sudo cp /tmp/build-gpu-modules.service /mnt/chroot/etc/systemd/system/build-gpu-modules.service

    cat << EOF > /tmp/monitor-service.sh
#!/bin/bash

journalctl -fu build-gpu-modules.service || true
exit 0
EOF

    sudo cp /tmp/monitor-service.sh /mnt/chroot/root/.build_gpu
    sudo chmod 644 /mnt/chroot/etc/systemd/system/build-gpu-modules.service
    sudo cp $SYZ_TOOLS/build-$VENDOR-gpu.sh /mnt/chroot/root/.build_gpu
    sudo cp $SYZ_TOOLS/gpu_module.patch /mnt/chroot/root/.build_gpu
    sudo chmod +x /mnt/chroot/root/.build_gpu/build-$VENDOR-gpu.sh
    sudo chmod +x /mnt/chroot/root/.build_gpu/monitor-service.sh
    sudo touch /mnt/chroot/root/.build_gpu/update

    sudo mkdir -p /mnt/chroot/root/.syzkaller_features/
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_NETDEV
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_FAULT
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_LEAK
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_ANDROID_SANDBOX
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_NET_INJECTION
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_USB
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_VHCI
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_KCSAN
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_DEVLINK
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_NICVF
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_WIFI
    sudo touch /mnt/chroot/root/.syzkaller_features/DISABLE_802154

    sudo ln -s /mnt/chroot/etc/systemd/system/build-gpu-modules.service /mnt/chroot/etc/systemd/system/multi-user.target.wants/build-gpu-modules.service
    sudo umount /mnt/chroot
    touch $IMAGE/.stamp/GUEST_SCRIPT_READY
else
    echo "Found $IMAGE/.stamp/GUEST_SCRIPT_READY"
fi

if [ "$GPU_SRC_PATCH" != "" ]; then
    sudo mkdir -p /mnt/chroot
    sudo mount -o loop $IMAGE/$RELEASE.img /mnt/chroot
    sudo touch /mnt/chroot/root/.build_gpu/update
    sudo rm /mnt/chroot/root/.build_gpu/.stamp/BUILD_GPU_MODULES || true
    if [ -f "$GPU_SRC_PATCH" ]; then
        sudo cp $GPU_SRC_PATCH /mnt/chroot/root/.build_gpu/gpu_module.patch
    else
        echo "$GPU_SRC_PATCH does not exist"
    fi
    sudo umount /mnt/chroot
fi

if [ "$FLAG_PANIC_ON_ASSERT" != "" ]; then
    sudo mkdir -p /mnt/chroot
    sudo mount -o loop $IMAGE/$RELEASE.img /mnt/chroot
    sudo touch /mnt/chroot/root/.build_gpu/.panic_on_assert
    sudo touch /mnt/chroot/root/.build_gpu/update
    sudo rm /mnt/chroot/root/.build_gpu/.stamp/BUILD_GPU_MODULES || true
    sudo umount /mnt/chroot
fi

if [ "$FLAG_SKIP_ON_ASSERT" != "" ]; then
    sudo mkdir -p /mnt/chroot
    sudo mount -o loop $IMAGE/$RELEASE.img /mnt/chroot
    sudo rm /mnt/chroot/root/.build_gpu/.panic_on_assert || true
    sudo touch /mnt/chroot/root/.build_gpu/update
    sudo rm /mnt/chroot/root/.build_gpu/.stamp/BUILD_GPU_MODULES || true
    sudo umount /mnt/chroot
fi

cd $HOME_DIR
chmod +x $SYZ_TOOLS/build-kernel.sh
$SYZ_TOOLS/build-kernel.sh -i $IMAGE/$RELEASE.img -l $LINUX --vga-id $VGA_ID --adu-id $ADU_ID $DEBUG $SKIP_BUILD --vendor $VENDOR
echo "All set"
