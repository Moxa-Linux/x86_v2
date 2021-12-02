The Moxa UART interface driver is for moxa DA-820 embedded computer. It creates the /dev/moxa_interface_8250 the entry point for setting the UART RS-232/422/485 interface. 
You can use the setuartmode to set the working mode.
This is the architecture of the moxa_uart_mode driver.


user sapce	setuartmode

		/dev/moxa_interface_8250
		-----------------------------------------------------------------
kernel space	| moxa_v2100_wdt.ko  |  moxa_device_dio.ko |  moxa_uart_mode.ko  |
		-----------------------------------------------------------------
		| moxa_v2100_superio.ko                                          | 
		-----------------------------------------------------------------


## Setting the path to your kernel source tree in Makefile.

	To compile this driver, you should modify the KDIR to the path of your kernel source. 
	Modify the KERNEL_VERSION to your target's kernel version.
	Modify the EXTRA_CFLAGS to include moxa_v2100_superio driver.
	We configure the KDIR, KERNEL_VERSION and EXTRA_CFLAGS in Makefile for configuring the path of the kernel source.

	The original value

	KDIR:=$(ROOTDIR)/$(LINUXDIR)
	KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`
	EXTRA_CFLAGS+=-isystem $(ROOTDIR)/modules/moxa_v2100_superio

	The new value

	KDIR=/lib/modules/`uname -r`/build
	KERNEL_VERSION=`uname -r`
	EXTRA_CFLAGS+=-isystem $(PWD)/../moxa_v2100_superio

## Compile the moxa_v2100_superio kernel module before making this module.

	First, you should reference moxa_v2100_wdt/README to build the moxa_v2100_superio.ko.

	$ cd /home/moxa/moxa_v2100_superio
	$ make
	  Building modules, stage 2.
	  MODPOST 2 modules
	  CC      /home/moxa/moxa_v2100_wdt/moxa_v2100_superio.mod.o
	  LD [M]  /home/moxa/moxa_v2100_wdt/moxa_v2100_superio.ko

## Compile the driver after your kernel source has compiled.

	Then build the device driver

	$ cd /home/moxa/moxa_uart8250_mode
	$ make
	  Building modules, stage 2.
	  MODPOST 2 modules
	  CC      /home/moxa/moxa_uart8250_mode/moxa_uart_mode.o
	  LD [M]  /home/moxa/moxa_uart8250_mode/moxa_uart_mode.ko

## Load this driver manually

	You can copy it to target and load it.

	$ insmod /PATH/TO/moxa_uart_mode.ko

## Install the moxa_v2100_superio drivers and load it at boot time.

	Build and install the superio driver
	$ cd /home/moxa/moxa_v2100_superio
	$ make install

	Install the moxa_uart_mode.ko driver
	$ cd /home/moxa/moxa_uart8250_mode
	$ make install

## Uset the setuartmode utility to set/get the UART mode

	To set the first serial port as RS-232 mode

	root@Moxa:~# setuartmode -n 1 -s 0

	To set the first serial port as RS-485-2W mode

	root@Moxa:~# setuartmode -n 1 -s 1

	To set the first serial port as RS-422/485-4W mode

	root@Moxa:~# setuartmode -n 1 -s 2

	To get the first serial port mode.

	root@Moxa:~# setuartmode  -n 1 -g

	To set the second serial port as RS-232 mode
	
	root@Moxa:~# setuartmode -n 2 -s 0

	To set the second serial port as RS-485-2W mode

	root@Moxa:~# setuartmode -n 2 -s 1

	To set the second serial port as RS-422/485-4W mode

	root@Moxa:~# setuartmode -n 2 -s 2

	To get the second serial port mode.

	root@Moxa:~# setuartmode  -n 2 -g

