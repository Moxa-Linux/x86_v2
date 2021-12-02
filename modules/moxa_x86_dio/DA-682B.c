#include "dio_main.h"
/*
 * Field: Name, do_state, addr, di_num, do_num, gpio_method: superio/gpio, model_select
 */

/*
 * For DA-682B, IO port address is located at 0x206:  bit 6~7 =DO0-1.
 */
static dio_dev DA_682B_DO = {"DA-682B", 0xFF, 0x206, 0, 8, gpio, 9};

/* 
 * dio_inb() and dio_outb() -
 * call gpio or superio denpend on mdev->method, mdev->addr 
 * order is [do d1] 
 */

u8 DA_682B_DO_dio_inb(dio_dev* mdev, struct dio_set_struct* set)
{ 
	u8 val = 0;

	if (mdev->method == superio) {
		val = superio_read(mdev->log_dev, mdev->addr);
	}
	else {
		val = inb(mdev->addr);
	}

	dp("dio_inb: %x\n", val);

	/* Shift for DA-682B DIO bitmap order: bit 6~7 =DO0-1 */
	return (val >> set->io_number);
}

void DA_682B_DO_dio_outb(dio_dev* mdev, struct dio_set_struct* set) 
{
	/*
	 * bitmap: bit 6~7 =DO0, DO1  bit 0~3 = PLED0 ,PLED1, PLED2, PLED3.
	 */

	/* Jared, 2016/11/10, keep the PLED status */
	/*mdev->do_state |= inb(mdev->addr) & 0x0F;*/
	mdev->do_state = inb(mdev->addr);

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

void DA_682B_DO_get_outb(dio_dev* mdev, struct dio_set_struct* set) {
	if (mdev->do_state & (1 << set->io_number))
		set->mode_data = 1;
	else
		set->mode_data = 0;

}

dio_dev* moxa_platform_dio_init(void){

	// Register the SuperIO/GPIO inb()/outb() callback function
	register_dio_inb(DA_682B_DO_dio_inb);
	register_dio_outb(DA_682B_DO_dio_outb);
	register_get_dout(DA_682B_DO_get_outb);

	/* Jared, 2016/11/10                                */
	/* The PLED and DO are at the same IO port address. */
	/* So we have to keep the original DO status.       */

	DA_682B_DO.do_state = inb(DA_682B_DO.addr);

	printk("moxa-device-dio, %s: %s series  \n", __FUNCTION__, DA_682B_DO.name);
	return &DA_682B_DO;
}

