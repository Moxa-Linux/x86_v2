#ifndef __DA683_H__
#define __DA683_H__

#define LED_NUM		8
#define LED_PORT	0x301
#define LED_PATERN	"11111111"
#define LED_MASK	0xFF
#define ACTIVE_LOW	1		// Active High

#define moxainb inb
#define moxaoutb outb

/* Because DA-682A BIOS has configured the superio, 
 * we don't need to configure it again. Turn the LED off.
 */
#define moxa_platform_led_init() {		\
	if ( ACTIVE_LOW )			\
		moxaoutb(LED_MASK, LED_PORT);	\
	else					\
		moxaoutb(~LED_MASK, LED_PORT);		\
}

#define superio_config(x)

#endif // ifndef __DA683_H__
