/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    2015-04-08 WesHuang
		new release
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include "moxa_pcie_power.h" 
/* 
 * Open/release
 */


static int power_control_open (struct inode *inode, struct file *file) 
{
	p("open\n");
	return 0;
}

static int power_control_release (struct inode *inode, struct file *file) 
{
	p("release\n");
	return 0;
}


/* 
 *  I/O control 
 */
static long power_control_ioctl(struct file *file,unsigned int cmd, unsigned long arg) 
{
	struct slot_info   si;
	unsigned char regv;

	if (copy_from_user(&si, (struct slot_info *)arg, sizeof(struct slot_info)))
		return -EFAULT;

	p("si.num = %d\n", si.num);
	p("si.value = %d\n", si.value);
	p("cmd = %d\n", cmd);

	if ((cmd != IOCTL_SET_SLOT) || (si.num > 4) || (si.num < 1) || (si.value > 1) || (si.value < 0)) {
		printk("Incorrect parameter\n");
		return -EINVAL;
	}

	/* 
	 * For the 0x304 register, the value are
	 * bit 6: socket 2, 0 active
	 * bit 4: socket 1, 0 active
	 */
	regv = inb(DKCONTROL_GPIO_ADDR);
	p("current reg value is %x\n", regv);
	switch (si.num) {
		case 2:
			if (!si.value) 
				regv |= 0x40;
			else 
				regv &= 0xbf;
			break;
		case 1:
			if (!si.value)
				regv |= 0x10;
			else
				regv &= 0xef;
			break;
	}
	p("set regv = %x\n", regv);
	outb(regv, DKCONTROL_GPIO_ADDR);

	return 0;
}

static struct file_operations mx_fops = {
	.owner	=	THIS_MODULE,
	.unlocked_ioctl	=   power_control_ioctl,
	.open		=   power_control_open,
	.release	=   power_control_release,
};

static struct miscdevice mx_miscdev = {
	.minor = MX_MINOR_NUMBER,
	.name = MX_NAME,
	.fops = &mx_fops,
};

static int __init power_control_init_module (void) 
{
	int res;
	printk("initializing MOXA pcie power control driver, %s\n", DRV_VERSION);

	/* register misc driver */
	res = misc_register(&mx_miscdev);
	if (res) {
		printk("Moxa pcie power control driver: Register misc fail !\n");
		return res;
	}
	return 0;
}

/*
 * close and cleanup module
 */
static void __exit power_control_cleanup_module (void) 
{
	printk("cleaning up module\n");
	misc_deregister(&mx_miscdev);
}

module_init(power_control_init_module);
module_exit(power_control_cleanup_module);
MODULE_AUTHOR("Wes.Huang@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA pcie_power_control module");

