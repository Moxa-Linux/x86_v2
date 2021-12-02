# programmable relay driver

The programmable relay driver is for moxa DA-681A embedded computer. /dev/relay is the entry point for trun on/off the relay. You can use the `echo 1` to turn on the relay. Or write a C program used open(), write() and close() system call to control the relay. Following describes how to compile this driver.

---

** Setting the path to your kernel source tree in Makefile.

	To compile this driver, you should modify the KDIR to the path of your kernel source.
	Modify the KERNEL_VERSION to your target's kernel version.
	We configure the KDIR, KERNEL_VERSION in Makefile for configuring the path of the kernel source.

	The original value

	KDIR:=$(ROOTDIR)/$(LINUXDIR)
	KERNEL_VERSION=`cat $(ROOTDIR)/$(LINUXDIR)/include/config/kernel.release`

	The new value

	KDIR=/lib/modules/`uname -r`/build
	KERNEL_VERSION=`uname -r`

** Compile the driver.

	$ cd /home/moxa/moxa_relay
	$ make
	  Building modules, stage 2.
	  MODPOST 2 modules
	  CC      /home/moxa/moxa_DA681A_relay/moxa-relay.o
	  LD [M]  /home/moxa/moxa_DA681A_relay/moxa-relay.ko

## Install this driver manually

	$ insmod /home/moxa/moxa_DA681A_relay/moxa-relay.ko

## Install this driver and load it at boot time.

	Install this driver
	$ cd /home/moxa/moxa_DA681A_relay/
	$ make install

## Control the relay

	Turn on the relay.

	$ echo "1" >/dev/relay

	Turn off the relay.

	$ echo "0" >/dev/relay
