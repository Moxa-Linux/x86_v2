The superio driver is for moxa da-682, da-683, da-685, da-820, da-682a, expc-1519, mc-7100-mp, mc-7200-mp, mc-7270-dc-cp, tc-6100, v2101, v2201, v2416a and v2616a serials embedded computer.
It's the base driver moxa_device_dio.ko and moxa_v2100_wdt.ko.
The moxa_v2100_superio.ko should be loaded into kernel before the moxa_v2100_wdt.ko, moxa_uart_mode.ko and moxa_device_dio.ko are loaded.
this is the architecture of the super IO, watchdog, UART mode and DIO driver.


user sapce	debian watchdog daemon

		/dev/watchdog
		-----------------------------------------------------------------
kernel space	| moxa_v2100_wdt.ko  |  moxa_device_dio.ko |  moxa_uart_mode.ko  |
		-----------------------------------------------------------------
		| moxa_v2100_superio.ko                                          | 
		-----------------------------------------------------------------


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

## Install the kernel development environment or compile the kernel before making the module.

## Compile the driver after your kernel source has compiled.

	[root@localhost moxa_v2100_superio]# make
	Making modules moxa_v2100_superio.ko ...
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
	make[1]: Entering directory `/usr/src/kernels/`uname -r`'
	  CC [M]  /root/wdt/wdt_driver/moxa_v2100_superio/moxa_v2100_superio.o
	  Building modules, stage 2.
	  MODPOST
	  CC      /root/wdt/wdt_driver/moxa_v2100_superio/moxa_v2100_superio.mod.o
	  LD [M]  /root/wdt/wdt_driver/moxa_v2100_superio/moxa_v2100_superio.ko

## Load the superio driver drivers.

	Load the driver manually.

	$ insmod ./moxa_v2100_superio.ko

	You can install the driver manually

	For Redhat Enterprise 6.x/7.x, CentOS 6.x/7.x
	$ cp -a ./moxa_v2100_superio.ko  /lib/modules/`uname -r`/kernel/drivers/char/watchdog/

	For Ubuntu/Debian
	$ cp -a ./moxa_v2100_superio.ko  /lib/modules/`uname -r`/kernel/drivers/watchdog/

	Install the super I/O driver by Makefile
	$ make install
