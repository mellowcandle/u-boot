# SPDX-License-Identifier:	GPL-2.0+
#
# (C) Copyright 2018 Ramon Fried <ramon.fried@gmail.com>
#

This specific board target is loaded directly by the SBL (Secondary boot loader)
firmware from Qualcomm and does not rely on LK for switching to 64bit mode and
initalizing the HW.
The allows completely removing the LK dependency.

Although the target (APQ8016) is Cortex A53 and the kernel runs in 64bit mode,
This version of U-boot start in 32bit and jumps to 64bit kernel through a
dedicated SCM call to Trustzone.

Build & Run instructions:
1) Setup CROSS_COMPILE to arm compiler (I use the latest released by Linaro)
2) make dragonboard410c_32b_config
3) make
4) Sign the image using signlk

Replace LK by putting the device under fastboot:
$ fastboot flash aboot u-boot-signed

What is missing:
- Reset doesn't work
- Auto detection of DDR size
- Fastboot support (USB gadget)

Android image released by Qualcomm and not standard and currently not supported.
It is possible to boot these kind of images, but you have to figure the correct
FDT offset and provide it as an argument to bootm.

The prefrred way of booting images is the new FIT format, which encapsulates both
the kernel, ramdisk (not relevant here) and FDT.
An .its file for generating such image is provided here in this folder.
Make sure to copy the FDT & kernel image to the same folder as dragonboard410c_32b.its
(or change the paths inside the .its file).

$ tools/mkimage -f dragonboard410c_32b.its uImage

