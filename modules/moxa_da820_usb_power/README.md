The moxa_usb_power.ko driver is for enable/disable the front-end or back-end USB ports ob DA-820 embedded computer. The 'mx_usb_power' is the utility to enable or disable the front-end/back-end USB ports. Following are the steps to compile this driver.

## Setting the path to your kernel source tree in Makefile.

	To compile this driver, you should modify the KDIR to the path of your kernel source. We configure the KDIR in Makefile for configuring the path of the kernel source.

	The original KDIR

	KDIR:=$(ROOTDIR)/$(LINUXDIR)

	The new KDIR

	KDIR=/media/Home/work/x86/kernel/linux-source-3.2

## Compile your kernel before makeing the module.

## Compile the driver after your kernel source has compiled.

	$ cd /home/moxa/git/DA-820/modules/moxa_DA820_usb_power
	$ make
	  Building modules, stage 2.
	  ...
	  LD [M]  /home/moxa/git/DA-820/modules/moxa_DA820_usb_power/moxa_usb_power.ko

## Install this driver

	You can copy it to target and load it.

	$ cp -a /home/moxa/moxa_pled/moxa_usb_power.ko /lib/modules/`uname -r`/kernel/drivers/misc/
	$ insmod /lib/modules/`uname -r`/kernel/drivers/misc/moxa_usb_power.ko
	$ depmod -a

	Or install this driver by Makefile.

	$ cd /home/moxa/moxa_pled/
	$ make install


## Use the mx_usb_enable or mx_usb_disable to enable or disable the front-end and-back end USB ports.

	The mx_usb_power man menu.
	  mx_usb_power -r|-f  [-e|-d] [-h]
	    Show the Front-end, Rear-end USB ports enable/disable status if no argument apply.
	    -h: Show this information
	    -r: Rear USB ports. You shall use one of the -b or -f option in the mx_usb_power command.
	    -f: Front-end USB ports. You shall use one of the -b or -f option in the mx_usb_power command.
	    -d: Disable the USB ports
	    -e: Enable the USB ports

	To enable the front-end USB ports
	$ mx_usb_power -f -e

	To enable the back-end USB ports
	$ mx_usb_power -b -e

	To disable the front-end USB ports
	$ mx_usb_power -f -d

	To disable the back-end USB ports
	$ mx_usb_power -b -d

	
