
This source tree supports following x86 model.

## For V2201

	modules/moxa_v2100_superio
	modules/moxa_v2100_wdt
	modules/moxa_x86_dio
	modules/moxa_pled
	modules/moxa_v2200_uart

	user/v2201_setinterface
	user/mx_v2201_usb_power

## For V2616A, use the following drivers and utilities

	modules/moxa_v2100_superio
	modules/moxa_v2100_wdt
	modules/moxa_x86_dio
	modules/moxa_v2616A_minipcie_reset
	modules/mxser
	modules/moxa_v2616A_hostswap (optional)

	user/muestty_utility
	user/mx_pcie_reset
	user/mxhtspd (optional)

## For V2403, use the following drivers and utilities

	modules/moxa_v2100_superio
	modules/moxa_v2100_wdt
	modules/moxa_x86_dio
	modules/moxa_v2403_mxpciepower
	modules/mxu11x0
	modules/r8168 (optional)

	user/mx_dio_control
	user/v2403_setinterface

## For V2406A/V2426A, use the following drivers and utilities

	modules/moxa_v2100_superio
	modules/moxa_v2100_wdt
	modules/moxa_x86_dio
	modules/mxser
	modules/v2400a-epm3438
	modules/v2400a-epm3112
	modules/v2400a-dkcontrol
	modules/r8168 (optional)

	user/setinterface
	user/muestty_utility

## For DA-683/685, use the following drivers and utilities

	modules/moxa_v2100_superio
	modules/moxa_v2100_wdt
	modules/moxa_x86_dio
	modules/moxa_pled
	modules/r8168 (optional)

	user/setinterface

## For DA-681A, DA-682A, use the following drivers and utilities

	modules/moxa_v2100_superio
	modules/moxa_v2100_wdt
	modules/mxser
	modules/moxa_pled
	modules/moxa_da681a_relay (optional)
	modules/r8168 (optional)
	modules/moxa_IRIG-B (optional in DA-682A)

	user/setinterface
	user/irigb (optional in DA-682A)

## For DA-820, use the following drivers and utilities

	modules/moxa_v2100_superio
	modules/moxa_v2100_wdt
	modules/moxa_da820_usb_power
	modules/moxa_da820_uartmode
	modules/mxser
	modules/moxa_pled
	modules/r8168 (optional)
	modules/moxa_IRIG-B (optional)

	user/setinterface
	user/setuartmode
	user/muestty
	user/mx_usb_power
	user/irigb (optional)

