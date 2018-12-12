The hotswap driver is for moxa V2616A embedded computer. "/dev/hotswap" is the entry point for tracking the disks hotswapping. You can use the 'mxhtspd' to monitor and programming the button and harddisk hotswap. The mxhtspd provides a callback script for stopping your program and hotswapping your disk in run time. Following describes how to compile this driver.

## Setting the path to your kernel source tree in Makefile.

	To compile this driver, you should modify the KDIR to the path of your kernel source. We configure the KDIR in Makefile for configuring the path of the kernel source.

	The original KDIR

	KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
	KDIR:=$(ROOTDIR)/$(LINUXDIR)

	The new KDIR

	KERNEL_VERSION=`uname -r`
	KDIR:=/lib/modules/$(KERNEL_VERSION)/build

## Compile your kernel before makeing the module.

## Compile the driver after your kernel source has compiled.

	$ cd /home/moxa/git/V2616A/modules/moxa_v2616A_hotswap
	$ make
	  Building modules, stage 2.
	  ...
	  LD [M]  /home/moxa/git/V2616A/modules/moxa_v2616A_hotswap/moxa_hotswap.ko

## Load this driver

	You can copy it to target and load it.

	$ insmod /PATH/TO/moxa_hotswap.ko

## Use the mxhtspd to monitor the disk hotswap event

	To start it:

	$ /etc/init.d/mxhtspd.sh start

	To stop it:

	$ /etc/init.d/mxhtspd.sh stop

