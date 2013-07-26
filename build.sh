#!/bin/bash

# Set Default Path
TOP_DIR=$PWD
KERNEL_PATH="/home/simone/neak-gs3-jb"

# Set toolchain and root filesystem path
TOOLCHAIN="/home/simone/android-toolchain-eabi-4.7/bin/arm-eabi-"
ROOTFS_PATH=$1

# Exports
export KBUILD_BUILD_VERSION="N.E.A.K-S3-3.1.2x"
export KERNELDIR=$KERNEL_PATH
export USE_SEC_FIPS_MODE=true

# First compile
make -j`grep 'processor' /proc/cpuinfo | wc -l` ARCH=arm CROSS_COMPILE=$TOOLCHAIN >> compile.log 2>&1 || exit -1

# Copying kernel modules
find -name '*.ko' -exec cp -av {} $ROOTFS_PATH/lib/modules/ \;

# Recompile to make modules working
make -j`grep 'processor' /proc/cpuinfo | wc -l` ARCH=arm CROSS_COMPILE=$TOOLCHAIN || exit -1

# Copy Kernel Image
rm -f $KERNEL_PATH/releasetools/tar/$KBUILD_BUILD_VERSION.tar
rm -f $KERNEL_PATH/releasetools/zip/$KBUILD_BUILD_VERSION.zip
cp -f $KERNEL_PATH/arch/arm/boot/zImage .

# Create ramdisk.cpio archive
cd $ROOTFS_PATH
find . | cpio -o -H newc > ../ramdisk.cpio
cd ..

# Make boot.img
./mkbootimg --kernel zImage --ramdisk ramdisk.cpio --board smdk4x12 --base 0x10000000 --pagesize 2048 --ramdiskaddr 0x11000000 -o $KERNEL_PATH/boot.img

# Copy boot.img
cp boot.img $KERNEL_PATH/releasetools/zip
cp boot.img $KERNEL_PATH/releasetools/tar

# Creating flashable zip and tar
cd $KERNEL_PATH
cd releasetools/zip
zip -0 -r $KBUILD_BUILD_VERSION.zip *
cd ..
cd tar
tar cf $KBUILD_BUILD_VERSION.tar boot.img && ls -lh $KBUILD_BUILD_VERSION.tar

# Cleanup
rm $KERNEL_PATH/releasetools/zip/boot.img
rm $KERNEL_PATH/releasetools/tar/boot.img
rm $KERNEL_PATH/zImage
