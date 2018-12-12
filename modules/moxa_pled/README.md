The programmable LED driver is for moxa DA-683, DA-685, DA-710, DA-820, DA-682A, MC-7200 and V2100 embedded computer. /dev/pled is the entry point for trun on/off the programmable LED. You can use the `echo 00000001` to turn on the first LED. Or write your program used open(), write() and close() system call to control the LED. Following describes how to compile this driver.

## Setting the path to your kernel source tree in Makefile.

	To compile this driver, you should modify the KDIR to the path of your kernel source.
	Modify the KERNEL_VERSION to your target's kernel version.
	We configure the KDIR and KERNEL_VERSION in Makefile for configuring the path of the kernel source.

	The original value

	KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
	KDIR:=$(ROOTDIR)/$(LINUXDIR)

	The new value

	KERNEL_VERSION=`uname -r`
	KDIR=/lib/modules/$(KERNEL_VERSION)/build

## Compile the driver.

	$ cd /home/moxa/moxa_pled
	$ make
	  Building modules, stage 2.
	  MODPOST 2 modules
	  CC      /home/moxa//moxa_pled/moxa-pled.o
	  LD [M]  /home/moxa/moxa_pled/moxa-pled.ko

## Install this driver manually

	$ insmod /home/moxa/moxa_pled/moxa-pled.ko

## Install this driver and load it at boot time.

	Install this driver
	$ cd /home/moxa/moxa_pled/
	$ make install

## Turn on/of the LED

	Turn on the first LED.

	$ echo "00000001" >/dev/pled

	Turn off the first LED.

	$ echo "00000000" >/dev/pled

