This is the serial driver for DA-682A, DA-820-LX embedded computer.
Following describes how to compile this driver.

## Setting the path to your kernel source tree in Makefile.

	The KDIR is variable is used for the path of kernel source.
	If your kernel is located at another location, please set the KDIR depends on your making environment.

	KDIR:=/lib/modules/$(shell uname -r)/build

	or other location of your kernel source

	KDIR=/media/Home/work/V2616A/linux-3.2.x

## Compile your kernel before makeing the module.

## Compile the driver after your kernel source has compiled.

	$ cd /home/jared_wu/git/DA683/modules/mxser
	$ make
	  Building modules, stage 2.
	  MODPOST 2 modules
	  CC      /home/jared_wu/git/DA682A/modules/mxser/mxser.mod.o
	  LD [M]  /home/jared_wu/git/DA682A/modules/mxser/mxser.ko
	  CC      /home/jared_wu/git/DA682A/modules/mxser/mxupcie.mod.o
	  LD [M]  /home/jared_wu/git/DA682A/modules/mxser/mxupcie.ko

## Load the serial port drivers manually.

	$ insmod /PATH/TO/mxser.ko
	$ insmod /PATH/TO/mxupcie.ko

## Install the serial drivers and load it at boot time.

	Install the driver at /lib/modules/`uname -r`/...
	$ make install

## Using the serial port

	The mxser.ko device driver is for serial ports /dev/ttyM0, /dev/ttyM1, ....
        You can program these device files for serial communication.

	The mxupcie.ko device driver is for serial ports /dev/ttyMUE0, /dev/ttyMUE1, ....
        You can program these device files for serial communication.

