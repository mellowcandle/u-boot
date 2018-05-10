#!/bin/bash

set -e

export PATH=$PATH:$(pwd)/../gcc-linaro-7.2.1-2017.11-x86_64_aarch64-elf/bin
export CROSS_COMPILE=aarch64-elf-

make dragonboard410c_defconfig
make -j8
python ../alona/common/scripts/SecImage/sectools.py secimage -i ./u-boot ./ -g appsbl -c ../alona/common/scripts/SecImage/config/integration/secimage.xml -s
cp /local/mnt/workspace/alona/common/scripts/SecImage/secimage_output/default/appsbl/u-boot /tftpboot
#touch rd
#../skales/dtbTool -o dt.img arch/arm/dts
#../skales/mkbootimg --kernel=u-boot-dtb.bin --output=u-boot.img --dt=dt.img \
#	--pagesize 2048 --base 0x80080000 --ramdisk=rd --cmdline=""
