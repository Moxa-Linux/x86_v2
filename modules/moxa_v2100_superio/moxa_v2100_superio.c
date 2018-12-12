/*
 * superio.c -- a module for superio control 
 *
 * History:
 * Date		Aurhor		Comment
 * 11-10-2014	Jared Wu.	Define __PROC_READ_ENTRY__ for debugging
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/types.h>  /* size_t */
#include <linux/spinlock.h>
#include <linux/ioport.h>

#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/delay.h>

#include "moxa_superio.h"

#define DRV_VERSION "1.1"

#ifndef __VMWARE__  /* It's not necessary for VMware ESXi 5.02 */
static DEFINE_SPINLOCK(superio_lock);
static unsigned long flags;
#endif

static inline unsigned char superio_get_reg(u8 val)
{
	outb_p(val, SUPERIO_PORT);
	val = inb_p(SUPERIO_PORT+1);

	return val;
}

static void superio_select_dev(u8 val,u8 index) {
	outb_p(index, SUPERIO_PORT);
	outb_p(val, (SUPERIO_PORT+1));
}

static void superio_init(void) {

	outb_p (0x87, SUPERIO_PORT);
	outb_p (0x01, SUPERIO_PORT);
	outb_p (0x55, SUPERIO_PORT);
	outb_p (0x55, SUPERIO_PORT);
}

static void superio_release(void) {
	/* exit extended function mode */
	outb_p(0x02, SUPERIO_PORT);
	outb_p(0x02, SUPERIO_PORT);
}

/* 
 * wade: use spin_lock_irq to prevent the interrupt 
 * from hw interrupt /system call/ softirq 
 */
u8 superio_read(u8 logic_dev, u8 index, u8 port_num) 
{
	unsigned char val;
#ifndef __VMWARE__  /* It's not necessary for VMware ESXi 5.02 */
	spin_lock_irqsave(&superio_lock, flags);
#endif
	superio_init();
	superio_select_dev(port_num, logic_dev);

	/* get reg */
	outb_p(index, SUPERIO_PORT);
	udelay(10);
	val = inb_p(SUPERIO_PORT+1);
	udelay(10);

	superio_release();
#ifndef __VMWARE__  /* It's not necessary for VMware ESXi 5.02 */
	spin_unlock_irqrestore(&superio_lock, flags);
#endif
	return val;
}

void superio_write(u8 logic_dev, u8 index, u8 port_num, u8 val) 
{
#ifndef __VMWARE__  /* It's not necessary for VMware ESXi 5.02 */
	spin_lock_irqsave(&superio_lock, flags);
#endif
	superio_init();
	superio_select_dev(port_num, logic_dev);

	/* set reg */
	outb_p(index, SUPERIO_PORT);
	outb_p(val, SUPERIO_PORT + 1);

	superio_release();
#ifndef __VMWARE__  /* It's not necessary for VMware ESXi 5.02 */
	spin_unlock_irqrestore(&superio_lock, flags);
#endif
}


#ifndef __VMWARE__  /* It's not necessary for VMware ESXi 5.02 */
/*
 *	superio_verify_vendor:
 *	return true if vendor ID match
 */
static int superio_verify_vendor(void)
{
	u8 chip_id_h;
	u8 chip_id_l;

	superio_init();
	superio_select_dev(LOGIC_DEVICE_NUMBER, GPIO_CONFIG_REG);
	chip_id_h = superio_get_reg(CHIP_ID_BYTE1);
	chip_id_l = superio_get_reg(CHIP_ID_BYTE2);
	if ((chip_id_h == HW_VENDOR_ID_H) && (chip_id_l == HW_VENDOR_ID_L))
		return 0;

	return 1;
}

EXPORT_SYMBOL(superio_read);
EXPORT_SYMBOL(superio_write);
int __init superio_module_init(void)
{
	int ret = 0;

	printk("Load Moxa superio driver, %s\n", DRV_VERSION);

	if (superio_verify_vendor()) {
		pr_err("hardware device id not match!!\n");
		ret = -ENODEV;
		goto reqreg_err;
	}

	if (!request_region(SUPERIO_PORT, 2, "moxa_superio")) {
		printk("Can't request region !");
		ret = -EBUSY;
		goto reqreg_err;
	}

	spin_lock_init(&superio_lock);

reqreg_err:
	return ret;
}

void __exit superio_module_cleanup(void)
{
	printk("Unload Moxa superio driver\n");
	release_region(SUPERIO_PORT, 2);
}

module_init(superio_module_init);
module_exit(superio_module_cleanup);
MODULE_LICENSE("GPL");
#endif
