#ifndef __X86__SUPERIO__
#define __X86__SUPERIO__

#define	SUPERIO_CONFIG_PORT		0x2e

unsigned char superio_get_reg(u8 val) {
	outb (val, SUPERIO_CONFIG_PORT);
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
/*
#ifndef __WDT_TEST__
#if defined(DA681)
	outb (0x87, SUPERIO_CONFIG_PORT);
	outb(0x80, 0xeb); // a Small delay
	outb (0x87, SUPERIO_CONFIG_PORT);
	outb(0x80, 0xeb); // a Small delay
#elif defined(V21XX)
*/
	outb (0x87, SUPERIO_CONFIG_PORT);
	outb (0x01, SUPERIO_CONFIG_PORT);
	outb (0x55, SUPERIO_CONFIG_PORT);
	outb (0x55, SUPERIO_CONFIG_PORT);
/*
#endif
#endif
*/
}

void superio_exit_config(void) {
/*
#ifndef __WDT_TEST__
#if defined(DA681)
	outb (0xaa, SUPERIO_CONFIG_PORT);
	outb( 0x80, 0xeb); // a Small delay
#elif defined(V21XX)
*/
	outb(0x02, SUPERIO_CONFIG_PORT);
        outb( 0x80, 0xeb); // a Small delay
	outb(0x02, SUPERIO_CONFIG_PORT+1);
/*
#endif
#endif
*/
}

#endif	//__X86__SUPERIO__
