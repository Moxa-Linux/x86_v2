The watchdog driver is for moxa v2101, da-682, da-683, da-685, da-682a, v2616a and da-820 serials embedded computer.
the watchdog driver is comprised of moxa_v2100_superio.ko and moxa_v2100_wdt.ko.
the corresponding source are moxa_v2100_superio.c and moxa_v2100_wdt.c
this is the architecture of the watchdog driver.


user sapce	debian watchdog daemon

		/dev/watchdog
		-----------------------------------------------------------------
kernel space	| moxa_v2100_wdt.ko  |  moxa_device_dio.ko |  moxa_uart_mode.ko  |
		-----------------------------------------------------------------
		| moxa_v2100_superio.ko                                          | 
		-----------------------------------------------------------------

Following describes how to compile the watchdog driver.


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

	KDIR=/media/Home/work/x86/kernel/linux-source-3.2
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

## Compile the module. The Makefile will make moxa_v2100_wdt.ko.

	Then build the device driver

	$ cd /home/moxa/moxa_v2100_wdt
	$ make
	...
	  Building modules, stage 2.
	  MODPOST 2 modules
	  CC      /home/moxa/moxa_v2100_wdt/moxa_v2100_wdt.mod.o
	  LD [M]  /home/moxa/moxa_v2100_wdt/moxa_v2100_wdt.ko

## Load the watchdog drivers manually.

	The moxa_v2100_wdt.ko is dependent on moxa_v2100_superio.ko. 
	You should load the moxa_v2100_superio.ko at first. Then load the moxa_v2100_wdt.ko.

	$ insmod /home/moxa/moxa_v2100_superio.ko
	$ insmod /home/moxa/moxa_v2100_wdt.ko

## Install the watchdog drivers and load it at boot time.

	Build and install the superio driver
	$ cd /PATH/TO/moxa_v2100_superio
	$ make install

	Install the watchdog driver
	$ cd /PATH/TO/moxa_v2100_wdt
	$ make install

## Install and run the watchdog daemon to touch the hardware watchdog in 60 secords.

	For Debian/Ubuntu, you can install the watchdog daemon by apt-get
	$ apt-get install watchdog

	For Redhat, you can install the watchdog daemon by yum
	$ yum install watchdog

	or download and install it manually in Redhat Enterprise 5.9 X64
	
	$ wget ftp://ftp.pbone.net/mirror/ftp.pramberger.at/systems/linux/contrib/rhel5/archive/x86_64/watchdog-5.8-1.el5.pp.x86_64.rpm
	$ rpm -i watchdog-5.8-1.el5.pp.x86_64.rpm 

	Replace the softdog with moxa_v2100_wdt in /etc/sysconfig/watchdog
	$ vi /etc/sysconfig/watchdog
	# Watchdog device driver (w/o extension)
	DRIVER="moxa_v2100_wdt"

	You can compile the watchdog from the source code. The watchdog daemon source is available from debian's page.

## Install and run the watchdog daemon to touch the hardware watchdog in 60 secords.

	For Debian/Ubuntu, you can install the watchdog daemon by apt-get
	$ apt-get install watchdog

	For Redhat, you can install the watchdog daemon by yum
	$ yum install watchdog

	or download and install it manually in Redhat Enterprise 5.9 X64
	
	$ wget ftp://ftp.pbone.net/mirror/ftp.pramberger.at/systems/linux/contrib/rhel5/archive/x86_64/watchdog-5.8-1.el5.pp.x86_64.rpm
	$ rpm -i watchdog-5.8-1.el5.pp.x86_64.rpm 

	Replace the softdog with moxa_v2100_wdt in /etc/sysconfig/watchdog
	$ vi /etc/sysconfig/watchdog
	# Watchdog device driver (w/o extension)
	DRIVER="moxa_v2100_wdt"

	You can compile the watchdog from the source code. The watchdog daemon source is available from debian's page.
	http://packages.debian.org/wheezy/watchdog

## Then reboot the system and check if the watchdog daemon is working in the background.

	$ reboot

	Check if the drivers are loaded and if the watchdog daemon is running
	
	$ lsmod|grep wdt
	moxa_v2100_wdt         39056  0 
	moxa_v2100_superio     36232  1 moxa_v2100_wdt

	$ ps aux|grep watchdog
	root         4  0.0  0.0      0     0 ?        S<   21:54   0:00 [watchdog/0]
	root      3077  0.0  0.2   3884  1832 ?        SLs  21:57   0:00 /usr/sbin/watchdog
	root      3085  0.0  0.0  61228   736 pts/1    R+   21:57   0:00 grep watchdog

## Other reference about Linux watchdog

	http://www.linuxjournal.com/article/217
	http://www.sat.dundee.ac.uk/~psc/watchdog/watchdog-testing.html

