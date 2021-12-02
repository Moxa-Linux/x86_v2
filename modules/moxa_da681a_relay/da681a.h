#ifndef __DA681A_H__
#define __DA681A_H__

#define RELAY_NUM		1
#define RELAY_PORT	0x304
#define RELAY_PATERN	"1"
#define RELAY_MASK	0x01
#define ACTIVE_LOW	1		// Active High

#define moxainb inb
#define moxaoutb outb

/* Because DA-682A BIOS has configured the superio, 
 * we don't need to configure it again. Turn the LED off.
 */
#define moxa_platform_led_init() {		\
	if ( ACTIVE_LOW )			\
		moxaoutb(RELAY_MASK, RELAY_PORT);	\
	else					\
		moxaoutb(~RELAY_MASK, RELAY_PORT);		\
}

#define superio_config(x)

#endif // ifndef __DA681A_H__
