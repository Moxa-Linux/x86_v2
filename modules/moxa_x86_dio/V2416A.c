#include "dio_main.h"
/*
 * Field: Name, do_state, addr, di_num, do_num, gpio_method: superio/gpio, model_select
 */

/*
 * For V2416A, IO port address is located at 0x305:  bit 0~5 =DI0~5  bit 6~7 = DO6~7.
 */
static dio_dev V2416A = {"V2416A", 0, 0x305, 6, 2, gpio, 9};

/* 
 * dio_inb() and dio_outb() -
 * call gpio or superio denpend on mdev->method, mdev->addr 
 * order is [do d1] 
 */

u8 V2416A_dio_inb(dio_dev* mdev, struct dio_set_struct* set)
{ 
	u8 val = 0;

	if (mdev->method == superio) {
		val = superio_read(mdev->log_dev, mdev->addr);
	}
	else {
		val = inb(mdev->addr);
	}

	dp("dio_inb: %x\n", val);

	/* Shift for V2616A DIO bitmap order: bit 0~5 =DI0~5  bit 6~7 = DO6~7 */
	return (((val << mdev->do_num) >> mdev->do_num) >> set->io_number);
}

void V2416A_dio_outb(dio_dev* mdev, struct dio_set_struct* set) 
{ 
	/*
	 * bitmap: bit 0~5 =DI0~5  bit 6~7 = DO6~7.
	 */
	if (set->mode_data == DIO_HIGH) {
		mdev->do_state |= (1<<(set->io_number + mdev->di_num));
	} else if (set->mode_data == DIO_LOW) {
		mdev->do_state &= ~(1<<(set->io_number + mdev->di_num));
	}

	if (mdev->method == superio) {
		superio_write(mdev->log_dev, mdev->addr,  mdev->do_state);
	}
       	else {
		dp("dio_outb: %x\n", mdev->do_state);
		outb(mdev->do_state, mdev->addr);
	}
}

void V2416A_get_outb(dio_dev* mdev, struct dio_set_struct* set) {
	if (mdev->do_state & (1 << set->io_number + mdev->di_num))
		set->mode_data = 1;
	else
		set->mode_data = 0;

}

dio_dev* moxa_platform_dio_init(void){

	// Register the SuperIO/GPIO inb()/outb() callback function
	register_dio_inb(V2416A_dio_inb);
	register_dio_outb(V2416A_dio_outb);
	register_get_dout(V2416A_get_outb);

	printk("moxa-device-dio, %s: V2416A series  \n", __FUNCTION__);
        return &V2416A;
}

