This utility is for DA-820, EXPC-1519 or MC-7270-DC-CP.

## Compile and install
	
	$root@Moxa:~# make
	$root@Moxa:~# make install

## Uninstall

	$root@Moxa:~# make uninstall

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

