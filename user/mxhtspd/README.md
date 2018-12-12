The mxhtspd is the hotswap daemon monitors both of the SATA-A, SATA-B slots and the button on V2616A-LX. It monitor the hotswap entry, /dev/hotswap, provided by the moxa_hotswap.ko device driver.


## The files of the hotswap daemon:
	mxhtsp_lx.c - source of library
	mxhtspd.c mxhtspd.h - source of mxhtspd daemon 
	mxsetled.c - source code to setup led status
	mxhtsp_ioctl.h - define ioctl number and structure negotiating with driver

## Compile and install the hotswap daemon

	$ sudo make
	$ sudo make install

	Note: The hotswap daemon use systemd udev-rules for auto mount. make install will copy /usr/lib/systemd/system/systemd-udevd.service to /etc/systemd/system/systemd-udevd.service and comment out the MountFlags=slaved item. Your should reboot after make install because the udev is not intended to invoke for long-running process.

## Run the mxhtspd to monitor the disk hotswap event

	Manually to start it:

		In systemd-based sytem:

		$ systemctl start mxhtspd

		In sysvinit-based sytem:

		$ /etc/init.d/mxhtspd.sh start

	Manually stop it:

		In systemd-based sytem:

		$ systemctl stop mxhtspd

		In sysvinit-based sytem:

		$ /etc/init.d/mxhtspd.sh stop

	You can enable the mxhtspd service in next boot by the enable command
	
		In systemd-based sytem:

		$ systemctl enable mxhtspd

		In sysvinit-vased sytem:

		$ insserv -d mxhtspd.sh

	Then reboot the system, the mxhtspd daemon should run in background.

	$ reboot

