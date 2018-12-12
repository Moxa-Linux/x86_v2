#ifndef __V2201_H__
#define __V2201_H__

#define LED_NUM		1
#define LED_PORT	0x301
#define LED_PATERN	"0"
#define LED_MASK	0x01
#define ACTIVE_LOW	1		// Active High

/* Harry, Add for mini-PCIE power-on/off control */
#define IOCTL_POWER_OFF _IOW(MOXA_PLED_MINOR,0,int)
#define IOCTL_POWER_ON  _IOW(MOXA_PLED_MINOR,1,int)

struct slot_info {
        int     num;
        int     value;  // 1 for on, 0 for off
};

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

#endif // ifndef __V2201_H__
