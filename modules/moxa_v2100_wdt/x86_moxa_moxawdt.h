#ifndef __X86__MOXAWDT__
#define __X86__MOXAWDT__

#define	SUPERIO_CONFIG_PORT		0x2e

#define DEFAULT_WATCHDOG_TIME	(60)	// 60 seconds
#define WATCHDOG_MIN_TIME	(2)	//  2 seconds
#define WATCHDOG_MAX_TIME	(255)	//255 seconds

#define IT8783F_WDT_CONF	0x72	/* Watchdog timer configuration register */
#define IT8783F_WDT_WTV_0_7	0x73	/* Watchdog timer Time-out value (LSB) register */
#define IT8783F_WDT_WTV_8_15	0x74	/* Watchdog timer Time-out value (MSB) register */

#define IT8783F_WTVS	0x80	/* 1: second; 0: minute */
#define IT8783F_WOKE	0x40	/* WDT Output through KRST (Pulse) Enable (WOKE). 1: enable; 0: disable */
#define IT8783F_WTVES	0x20	/* 1: 64ms x WDT Timer-out value (default=4s); 0: Determined by WDT Time-out Value Select 1 */
#define IT8783F_WOPE	0x10	/* WDT Output through PWROK (Pulse) Enable (WOPE). 1: enable; 0: disable */

struct swtd_set_struct {
	int		mode;
	unsigned long	time;
};

#endif	//__X86__MOXAWDT__
