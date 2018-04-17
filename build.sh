#!/bin/bash

set -e

export PATH=$PATH:$(pwd)/../gcc-linaro-7.2.1-2017.11-x86_64_arm-eabi/bin
export CROSS_COMPILE=arm-eabi-

make dragonboard410c_32b_defconfig
make -j8
cp u-boot u-boot.elf
signlk/signlk.sh -i=u-boot.elf -o=u-boot-signed.elf
cp u-boot-signed.elf /tftpboot/
