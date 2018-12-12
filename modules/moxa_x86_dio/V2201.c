#include "dio_main.h"
/*
 * Field: Name, do_state, addr, di_num, do_num, gpio_method: superio/gpio, model_select
 */

/*
 * For V2201, IO port address is located at 0x300:  bit 0~3 =DI0~3  bit 4~7 = DO4~7.
 */
static dio_dev V2201 = {"V2201", 0, 0x305, 4, 4, gpio, 9};

/* 
 * dio_inb() and dio_outb() -
 * call gpio or superio denpend on mdev->method, mdev->addr 
 * order is [do d1] 
 */

u8 V2201_dio_inb(dio_dev* mdev, struct dio_set_struct* set)
{ 
	u8 val = 0;

	if (mdev->method == superio) {
		val = superio_read(mdev->log_dev, mdev->addr);
	}
	else {
		val = inb(mdev->addr);
	}

	dp("dio_inb: %x, %x\n", val);

	/* Shift for V2201 DIO bitmap order: bit 0~3 =DI0~3  bit 4~7 = DO4~7 */
	return ((val >> mdev->do_num) >> set->io_number);
}

void V2201_dio_outb(dio_dev* mdev, struct dio_set_struct* set) 
{ 
	/*
	 * bitmap: bit 0~3 =DI0~3  bit 4~7 = DO4~7.
	 */
	if (set->mode_data == DIO_HIGH) {
		mdev->do_state |= (1<<(set->io_number));
	} else if (set->mode_data == DIO_LOW) {
		mdev->do_state &= ~(1<<(set->io_number));
	}

	if (mdev->method == superio) {
		superio_write(mdev->log_dev, mdev->addr,  mdev->do_state);
	}
       	else {
		dp("dio_outb: %x\n", mdev->do_state);
		outb(mdev->do_state, mdev->addr);
	}
}

void V2201_get_outb(dio_dev* mdev, struct dio_set_struct* set) {
	if (mdev->do_state & (1 << set->io_number))
		set->mode_data = 1;
	else
		set->mode_data = 0;

}

dio_dev* moxa_platform_dio_init(void){

	// Register the SuperIO/GPIO inb()/outb() callback function
	register_dio_inb(V2201_dio_inb);
	register_dio_outb(V2201_dio_outb);
	register_get_dout(V2201_get_outb);

	printk("moxa-device-dio, %s: V2201 series  \n", __FUNCTION__);
        return &V2201;
}

