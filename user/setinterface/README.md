This utility is for DA-683, DA-685, TC-6000 and DA-682A MU860 uart RS-232/422/485 mode switch.

## Compile and install
	
	$root@Moxa:~# make
	$root@Moxa:~# make install

## Uninstall

	$root@Moxa:~# make uninstall

## Use the setinterface utility to set/get the UART mode

	Usage: /usr/bin/setinterface device-node [interface-no]
	device-node     - /dev/ttySn, n = 0, 1, ... . 
	interface-no    - following:
	none - to view now setting
	0 - set to RS232 interface
	1 - set to RS485-2WIRES interface
	2 - set to RS422 or RS485-4WIRES interface

