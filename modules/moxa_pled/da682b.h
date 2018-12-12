#ifndef __DA682B_H__
#define __DA682B_H__

#define LED_NUM		4
#define LED_PORT	0x206
#define LED_PATERN	"1111"
#define LED_MASK	0x0F
#define ACTIVE_LOW	1		// Active High
#define SUPERIO_CONFIG_PORT             0x2e

#define moxainb inb
#define moxaoutb outb

/* Because DA-682B BIOS has configured the superio, 
 * we don't need to configure it again. Turn the LED off.
 */
#define moxa_platform_led_init() {		\
	if ( ACTIVE_LOW )			\
		moxaoutb(LED_MASK, LED_PORT);	\
	else					\
		moxaoutb(~LED_MASK, LED_PORT);		\
}

#define superio_config(x)

#endif // ifndef __DA682B_H__
