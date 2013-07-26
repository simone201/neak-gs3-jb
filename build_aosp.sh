#!/bin/bash

ROOTFS_PATH="/home/simone/neak-gs3-jb/ramdisk-aosp"

echo "Building AOSP N.E.A.K. version..."

# Cleanup
./clean.sh

# Making .config
make neak_aosp_defconfig

# Compiling
./build.sh $ROOTFS_PATH
