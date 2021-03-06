The IRIG-B driver is for moxa DA-682A, DA-820 embedded computer. /dev/moxa_irigb is the entry point controlling the IRIG-B device.

## Setting the path to your kernel source tree in Makefile.

	To compile this driver, you should modify the KDIR to the path of your kernel source. We configure the KDIR in Makefile for configuring the path of the kernel source.

	The original KDIR

	KDIR:=$(ROOTDIR)/$(LINUXDIR)
	KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`

	The new KDIR

	KDIR:=/lib/modules/`uname -r`/build
	KERNEL_VERSION=`uname -r`

## Compile the driver after your kernel source has compiled.

	root@Moxa:/media/ramdisk/irigb/moxa_IRIG-B# make
	Making modules moxa_irigb.ko ...
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
	make[1]: Entering directory `/usr/src/linux-headers-3.2.0-4-amd64'
	  CC [M]  /media/ramdisk/irigb/moxa_IRIG-B/moxa_irigb.o
	 Building modules, stage 2.
	 MODPOST 1 modules
	 CC      /media/ramdisk/irigb/moxa_IRIG-B/moxa_irigb.mod.o
	 LD [M]  /media/ramdisk/irigb/moxa_IRIG-B/moxa_irigb.ko

## Install and load the driver

	You can install the driver
	root@Moxa: make install

	Or install the driver manually.

	root@Moxa: mkdir /lib/modules/`uname -r`/kernel/drivers/misc
	root@Moxa: cp -a moxa_irigb.ko /lib/modules/`uname -r`/kernel/drivers/misc/

	Load the driver manually
	root@Moxa: insmod /lib/modules/`uname -r`/kernel/drivers/misc/moxa_irigb.ko

## Load the driver in the initial script

	It will be loaded from /etc/init.d/mx_irigb.sh which is located at user/irigb/fakeroot/etc/init.d/mx_irigb.sh
	So you should install the driver at this path, /lib/modules/`uname -r`/kernel/drivers/misc/.

	load_irigb_driver() {

        if [ "`lsmod|grep -c moxa_irigb`" = "0" ]; then
                # If the driver has not been loaded, load it
                insmod /lib/modules/`uname -r`/kernel/drivers/misc/moxa_irigb.ko
        fi
	}


