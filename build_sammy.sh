#!/bin/bash

ROOTFS_PATH="/home/simone/neak-gs3-jb/ramdisk-samsung"

echo "Building SAMMY N.E.A.K. version..."

# Cleanup
./clean.sh

# Making .config
make neak_defconfig

# Compiling
./build.sh $ROOTFS_PATH
