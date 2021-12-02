the superio driver is for moxa v2101, da-682, da-683, da-685, da-682a serials embedded computer.
It's the base driver moxa_device_dio.ko and moxa_v2100_wdt.ko.
The moxa_v2100_superio.ko should be loaded into kernel before the moxa_v2100_wdt.ko and moxa_device_dio.ko are loaded.
this is the architecture of the super IO, watchdog and DIO driver.


user sapce	watchdog daemon         dio control program

		/dev/watchdog           /dev/dio
		--------------------------------------------
kernel space	| moxa_v2100_wdt.ko  |  moxa_device_dio.ko |
		--------------------------------------------
		| moxa_v2100_superio.ko                    |
		--------------------------------------------


## Setting the path to your kernel source tree in Makefile.

	The KDIR is variable is used for the path of kernel source.
	If your kernel is located at another location, please set the KDIR depends on your making environment.

	KDIR:=/lib/modules/$(shell uname -r)/build

	or other location of your kernel source

	KDIR=/media/Home/work/V2616A/linux-3.2.x

## Compile your kernel before makeing the module.

## Compile the driver after your kernel source has compiled.

	$ cd /home/jared_wu/git/DA683/modules/moxa_v2100_superio
	$ make
	  Building modules, stage 2.
	  MODPOST 2 modules
	  CC      /home/jared_wu/git/DA683/modules/moxa_v2100_wdt/moxa_v2100_superio.mod.o
	  LD [M]  /home/jared_wu/git/DA683/modules/moxa_v2100_wdt/moxa_v2100_superio.ko

## Load the DIO drivers.

	$ insmod /PATH/TO/moxa_v2100_superio.ko
	$ insmod /PATH/TO/moxa_device_dio.ko

## You can reference the user menu to programming the dio driver.

