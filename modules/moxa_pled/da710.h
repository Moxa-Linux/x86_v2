#ifndef __DA710_H__
#define __DA710_H__

#define LED_NUM		4
#define LED_PORT	0xe4
#define LED_PATERN	"1111"
#define LED_MASK	0x0F
#define ACTIVE_LOW	1		// Active Low
#define SUPERIO_CONFIG_PORT             0x2e

#define moxainb superio_get_reg
#define moxaoutb superio_set_reg

#define moxa_platform_led_init() { \
	/* initialize: disable all pled*/ \
	superio_enter_config(); \
	superio_set_logic_device(9); \
	moxaoutb(0x0f, 0xe4); \
	superio_exit_config(); \
}

#define superio_config(x) { \
	unsigned char temp_i; \
	/* set control device */ \
	superio_enter_config(); \
	superio_set_logic_device(9); \
	/* check status */ \
	temp_i=superio_get_reg(30); \
	if( (temp_i&0x1) != 0x1) { \
		printk("pled: error, pled is disabled\r\n"); \
		superio_exit_config(); \
		return x; \
	} \
}

unsigned char superio_get_reg(u8 val) {
	outb (val, SUPERIO_CONFIG_PORT);
	//jackie use "asm out 0xeb, al" as delay, it's the same way
	outb( 0x80,0xeb); // a Small delay
	val = inb (SUPERIO_CONFIG_PORT+1);
	outb( 0x80,0xeb); // a Small delay
	return val;
}

void superio_set_reg(u8 val,u8 index) {
	outb (index, SUPERIO_CONFIG_PORT);
	outb( 0x80, 0xeb); // a Small delay
	outb (val, (SUPERIO_CONFIG_PORT+1));
	outb(0x80, 0xeb); // a Small delay
}

void superio_set_logic_device(u8 val) {
	superio_set_reg(val, 0x07);
	outb( 0x80, 0xeb); // a Small delay
}

void superio_enter_config(void) {
#ifndef __WDT_TEST__
	outb (0x87, SUPERIO_CONFIG_PORT);
	outb(0x80, 0xeb); // a Small delay
	outb (0x87, SUPERIO_CONFIG_PORT);
	outb(0x80, 0xeb); // a Small delay
#endif
}

void superio_exit_config(void) {
#ifndef __WDT_TEST__
	outb (0xaa, SUPERIO_CONFIG_PORT);
	outb( 0x80, 0xeb); // a Small delay
#endif
}

#endif // #ifdef __DA710_H__
