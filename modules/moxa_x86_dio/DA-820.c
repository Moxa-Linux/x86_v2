#include "dio_main.h"
/*
 * Field: Name, do_state, addr, di_num, do_num, gpio_method: superio/gpio, model_select
 */

/*
 * For DA-820, IO port address is located at 0x304:  bit 0 =DO0 bit 6~7 = DI0~1
 */
static dio_dev DA820 = {"DA-820", 0, 0x304, 2, 6, gpio, 0};

/* 
 * dio_inb() and dio_outb() -
 * call gpio or superio denpend on mdev->method, mdev->addr 
 * order is [do d1] 
 */

u8 DA820_dio_inb(dio_dev* mdev, struct dio_set_struct* set)
{ 
	u8 val = 0;

	/* shift to hardware order: [di do] */
	if (mdev->method == superio) {
		val = superio_read(mdev->log_dev, mdev->addr);
	}
	else {
		val = inb(mdev->addr);
	}

	dp("dio_inb: %x\n", val);

	/* Shift for DA-820 DIO bitmap order: bit 0 =DO0 bit 6~7 = DI0~1 */
	return (val >> mdev->do_num);
}

void DA820_dio_outb(dio_dev* mdev, struct dio_set_struct* set) 
{
	/*
	 * bitmap: bit 0 =DO0 bit 6~7 = DI0~1.
	 */
	if (set->mode_data == DIO_HIGH) {
		mdev->do_state |= (1<<set->io_number);
	} else if (set->mode_data == DIO_LOW) {
		mdev->do_state &= ~(1<<set->io_number);
	}
	
	if (mdev->method == superio) {
		superio_write(mdev->log_dev, mdev->addr,  mdev->do_state);
	}
       	else {
		outb(mdev->do_state, mdev->addr);
	}
}

void DA820_get_outb(dio_dev* mdev, struct dio_set_struct* set) {
	if (mdev->do_state & (1 << set->io_number))
		set->mode_data = 1;
	else
		set->mode_data = 0;

}

dio_dev* moxa_platform_dio_init(void){

	// Register the SuperIO/GPIO inb()/outb() callback function
	register_dio_inb(DA820_dio_inb);
	register_dio_outb(DA820_dio_outb);
	register_get_dout(DA820_get_outb);

	printk("moxa-device-dio, %s: DA820 series  \n", __FUNCTION__);
	return &DA820;
}

