This utility is for DA-820, EXPC-1519 or MC-7270-DC-CP.

## Compile and install
	
	$root@Moxa:~# make
	$root@Moxa:~# make install

## Uninstall

	$root@Moxa:~# make uninstall

## Use the setuartmode utility to set/get the UART mode

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

