#include "dio_main.h"
/*
 * Field: Name, do_state, addr, di_num, do_num, gpio_method: superio/gpio, model_select
 */

/*
 * For V2616A, IO port address is located at 0x304:  bit 0~1 =DO0-1  bit 2~7 = DI0~5.
 */
static dio_dev V2616A = {"V2616A", 0x03, 0x304, 6, 2, gpio, 9};

/* 
 * dio_inb() and dio_outb() -
 * call gpio or superio denpend on mdev->method, mdev->addr 
 * order is [do d1] 
 */

u8 V2616A_dio_inb(dio_dev* mdev, struct dio_set_struct* set)
{ 
	u8 val = 0;

	if (mdev->method == superio) {
		val = superio_read(mdev->log_dev, mdev->addr);
	}
	else {
		val = inb(mdev->addr);
	}

	dp("dio_inb: %x\n", val);

	/* Shift for V2616A DIO bitmap order: bit 0~1 =DO0-1  bit 2~7 = DI0~5 */
	return (val >> (mdev->do_num+set->io_number));
}

void V2616A_dio_outb(dio_dev* mdev, struct dio_set_struct* set) 
{
	/*
	 * bitmap: bit 0~1 =DO0-1  bit 2~7 = DI0~5.
	 */
	if (set->mode_data == DIO_HIGH) {
		mdev->do_state |= (1<<set->io_number);
	} else if (set->mode_data == DIO_LOW) {
		mdev->do_state &= ~(1<<set->io_number);
	}
	
	if (mdev->method == superio) {
		superio_write(mdev->log_dev, mdev->addr, mdev->do_state);
	}
       	else {
		outb(mdev->do_state, mdev->addr);
	}
}

void V2616A_get_outb(dio_dev* mdev, struct dio_set_struct* set) {
	if (mdev->do_state & (1 << set->io_number))
		set->mode_data = 1;
	else
		set->mode_data = 0;

}

dio_dev* moxa_platform_dio_init(void){

	// Register the SuperIO/GPIO inb()/outb() callback function
	register_dio_inb(V2616A_dio_inb);
	register_dio_outb(V2616A_dio_outb);
	register_get_dout(V2616A_get_outb);

	printk("moxa-device-dio, %s: V2616A series  \n", __FUNCTION__);
	return &V2616A;
}

