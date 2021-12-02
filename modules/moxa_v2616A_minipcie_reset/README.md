The moxa_usb_gipo driver is for reseting the mini-PCIE module of V2616A embedded computer. "/dev/moxa_usb_gpio" is the entry point for turning on/off the mini-PCIE module on the slot. You can use the 'mx_pcie_reset' to reset the mini-PCIE module. Following describes how to compile this driver.

## Setting the path to your kernel source tree in Makefile.

	To compile this driver, you should modify the KDIR to the path of your kernel source. We configure the KDIR in Makefile for configuring the path of the kernel source.

	The original KDIR

	KDIR:=$(ROOTDIR)/$(LINUXDIR)

	The new KDIR

	KDIR=/media/Home/work/x86/kernel/linux-source-3.2

## Compile your kernel before makeing the module.

## Compile the driver after your kernel source has compiled.

	$ cd /home/moxa/git/V2616A/modules/moxa_v2616A_minipcie_reset
	$ make
	  Building modules, stage 2.
	  ...
	  LD [M]  /home/moxa/git/V2616A/modules/moxa_v2616A_minipcie_reset/moxa_usb_gpio.ko

## Load this driver

	You can copy it to target and load it.

	$ insmod /PATH/TO/moxa_usb_gpio.ko

## Use the mx_pcie_reset to reset the mini-PCIE module

	$ /etc/init.d/mx_pcie_reset

