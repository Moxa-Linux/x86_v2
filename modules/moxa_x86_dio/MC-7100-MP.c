#include "dio_main.h"

#define MAX_DOUT_PORT 8
#define MAX_DIN_PORT 8
/*
 * For MC-7130-MP, There are 8 DI, 8 DO.
 * IO port bitmap:
 *    DI0 0x301 bit 0
 *    DI1 0x301 bit 1
 *    DI2 0x301 bit 2
 *    DI3 0x301 bit 3
 *    DI4 0x302 bit 0
 *    DI5 0x302 bit 1
 *    DI6 0x302 bit 2
 *    DI7 0x300 bit 4
 *    DO0 0x301 bit 4
 *    DO1 0x301 bit 5
 *    DO2 0x301 bit 6
 *    DO3 0x300 bit 5
 *    DO4 0x305 bit 6
 *    DO5 0x304 bit 5
 *    DO6 0x302 bit 3
 *    DO7 0x302 bit 7
 */
/*
 * Field: Name, do_state, addr, di_num, do_num, gpio_method: superio/gpio, model_select
 * Note: In this case don't care the do_state and addr value.
 */
static dio_dev MC_7130_MP = {"MC-7130-MP", 0, 0x0, MAX_DIN_PORT, MAX_DOUT_PORT, gpio, 0};

#define IO_ADDRESS_GP1X 0x300
#define IO_ADDRESS_GP2X 0x301
#define IO_ADDRESS_GP3X 0x302
#define IO_ADDRESS_GP4X 0x303
#define IO_ADDRESS_GP5X 0x304
#define IO_ADDRESS_GP6X 0x305

#define IO_BIT_GP14 4 //IO Address GP1X Bit 4
#define IO_BIT_GP15 5 //IO Address GP1X Bit 5
#define IO_BIT_GP16 6 //IO Address GP1X Bit 6
#define IO_BIT_GP17 7 //IO Address GP1X Bit 7

#define IO_BIT_GP20 0 //IO Address GP2X Bit 0
#define IO_BIT_GP21 1 //IO Address GP2X Bit 1
#define IO_BIT_GP22 2 //IO Address GP2X Bit 2
#define IO_BIT_GP23 3 //IO Address GP2X Bit 3
#define IO_BIT_GP24 4 //IO Address GP2X Bit 4
#define IO_BIT_GP25 5 //IO Address GP2X Bit 5
#define IO_BIT_GP26 6 //IO Address GP2X Bit 6
#define IO_BIT_GP27 7 //IO Address GP2X Bit 7

#define IO_BIT_GP30 0 //IO Address GP3X Bit 0
#define IO_BIT_GP31 1 //IO Address GP3X Bit 1
#define IO_BIT_GP32 2 //IO Address GP3X Bit 2
#define IO_BIT_GP33 3 //IO Address GP3X Bit 3
#define IO_BIT_GP37 7 //IO Address GP3X Bit 7

#define IO_BIT_GP40 0 //IO Address GP4X Bit 0
#define IO_BIT_GP45 5 //IO Address GP4X Bit 5

#define IO_BIT_GP50 0 //IO Address GP5X Bit 0
#define IO_BIT_GP51 1 //IO Address GP5X Bit 1
#define IO_BIT_GP52 2 //IO Address GP5X Bit 2
#define IO_BIT_GP53 3 //IO Address GP5X Bit 3
#define IO_BIT_GP54 4 //IO Address GP5X Bit 4
#define IO_BIT_GP55 5 //IO Address GP5X Bit 5
#define IO_BIT_GP56 6 //IO Address GP5X Bit 6
#define IO_BIT_GP57 7 //IO Address GP5X Bit 7

typedef struct _IOADDR_DEF {
    unsigned long IOAddress;
    unsigned long DataBit;
    char *Desc;
    u8 do_state; /* save dout states */
} IOADDR_DEF;

#if 0 /* MC-7230-MP */
IOADDR_DEF  DIN[] = {
  {IO_ADDRESS_GP2X, IO_BIT_GP20, "DIN0", 0}, // IOAddress, DataBit, name, do_stat (Don't care)
  {IO_ADDRESS_GP2X, IO_BIT_GP21, "DIN1", 0}, //
  {IO_ADDRESS_GP2X, IO_BIT_GP22, "DIN2", 0}, //
  {IO_ADDRESS_GP2X, IO_BIT_GP23, "DIN3", 0}, //
  {IO_ADDRESS_GP3X, IO_BIT_GP30, "DIN4", 0}, //
  {IO_ADDRESS_GP3X, IO_BIT_GP31, "DIN5", 0}, //
  {IO_ADDRESS_GP3X, IO_BIT_GP32, "DIN6", 0}, //
  {IO_ADDRESS_GP1X, IO_BIT_GP14, "DIN7", 0}
};
 
IOADDR_DEF  DOUT[] = {
  {IO_ADDRESS_GP2X, IO_BIT_GP24, "DOUT0", 0}, // IOAddress, DataBit, name, do_stat
  {IO_ADDRESS_GP2X, IO_BIT_GP25, "DOUT1", 0}, //
  {IO_ADDRESS_GP2X, IO_BIT_GP26, "DOUT2", 0}, //
  {IO_ADDRESS_GP1X, IO_BIT_GP15, "DOUT3", 0}, //
  {IO_ADDRESS_GP5X, IO_BIT_GP56, "DOUT4", 0}, //
  {IO_ADDRESS_GP4X, IO_BIT_GP45, "DOUT5", 0}, //
  {IO_ADDRESS_GP3X, IO_BIT_GP33, "DOUT6", 0}, //
  {IO_ADDRESS_GP3X, IO_BIT_GP37, "DOUT7", 0}
};
#else /* MC-7130-MP */
IOADDR_DEF  DIN[] = {
  {IO_ADDRESS_GP2X, IO_BIT_GP20, "DIN0", 0}, // IOAddress, DataBit, name, do_stat (Don't care)
  {IO_ADDRESS_GP2X, IO_BIT_GP21, "DIN1", 0}, //
  {IO_ADDRESS_GP2X, IO_BIT_GP22, "DIN2", 0}, //
  {IO_ADDRESS_GP2X, IO_BIT_GP23, "DIN3", 0}, //
  {IO_ADDRESS_GP1X, IO_BIT_GP16, "DIN4", 0}, //
  {IO_ADDRESS_GP1X, IO_BIT_GP17, "DIN5", 0}, //
  {IO_ADDRESS_GP3X, IO_BIT_GP32, "DIN6", 0}, //
  {IO_ADDRESS_GP1X, IO_BIT_GP14, "DIN7", 0}
};
 
IOADDR_DEF  DOUT[] = {
  {IO_ADDRESS_GP2X, IO_BIT_GP24, "DOUT0", 0}, // IOAddress, DataBit, name, do_stat
  {IO_ADDRESS_GP2X, IO_BIT_GP25, "DOUT1", 0}, //
  {IO_ADDRESS_GP2X, IO_BIT_GP26, "DOUT2", 0}, //
  {IO_ADDRESS_GP2X, IO_BIT_GP27, "DOUT3", 0}, //
  {IO_ADDRESS_GP5X, IO_BIT_GP50, "DOUT4", 0}, //
  {IO_ADDRESS_GP5X, IO_BIT_GP51, "DOUT5", 0}, //
  {IO_ADDRESS_GP5X, IO_BIT_GP52, "DOUT6", 0}, //
  {IO_ADDRESS_GP5X, IO_BIT_GP53, "DOUT7", 0}
};
#endif
/* 
 * dio_inb() and dio_outb() -
 * call gpio or superio denpend on mdev->method, mdev->addr 
 * order is [do d1] 
 */

u8 MC_7130_MP_dio_inb(dio_dev* mdev, struct dio_set_struct* set)
{ 
	u8 val = 0;

	/* MC-7130-MP uses gpio ioport to control the digital output */
	val = inb(DIN[set->io_number].IOAddress);

	dp("dio_inb: set->io_number:%d, IOAddress:%x, DIN[set->io_number].DataBit:%d, val:%x\n", set->io_number, DIN[set->io_number].IOAddress, DIN[set->io_number].DataBit, val);

	/* Shift for bitmap order: reference the DIN[] array definition */
	return ( val >> DIN[set->io_number].DataBit );
}

void MC_7130_MP_dio_outb(dio_dev* mdev, struct dio_set_struct* set) 
{
	/*
	 * bitmap: reference the DOUT[] array definition.
	 */
	dp("bitmask:%x\n", (1<<DOUT[set->io_number].DataBit));

	DOUT[set->io_number].do_state = inb( DOUT[set->io_number].IOAddress );
	if (set->mode_data == DIO_HIGH) {
		DOUT[set->io_number].do_state |= (1<<DOUT[set->io_number].DataBit);
	} else if (set->mode_data == DIO_LOW) {
		DOUT[set->io_number].do_state &= ~(1<<DOUT[set->io_number].DataBit);
	}

	dp("set->io_number:%d, DOUT[set->io_number].do_state:%x, IOAddress:%x, DataBit:%d \n", set->io_number, DOUT[set->io_number].do_state, DOUT[set->io_number].IOAddress, DOUT[set->io_number].DataBit);
	
	/* MC-7130-MP uses gpio ioport to control the digital output */
	outb(DOUT[set->io_number].do_state, DOUT[set->io_number].IOAddress);
}

void MC_7130_MP_get_outb(dio_dev* mdev, struct dio_set_struct* set) {

	if (DOUT[set->io_number].do_state & (1 << DOUT[set->io_number].DataBit))
		set->mode_data = 1;
	else
		set->mode_data = 0;
}

dio_dev* moxa_platform_dio_init(void){

	// Register the SuperIO/GPIO inb()/outb() callback function
	register_dio_inb(MC_7130_MP_dio_inb);
	register_dio_outb(MC_7130_MP_dio_outb);
	register_get_dout(MC_7130_MP_get_outb);

	printk("moxa-device-dio, %s: %s series  \n", __FUNCTION__, MC_7130_MP.name);
	return &MC_7130_MP;
}

