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
#include <asm/uaccess.h>
#include <asm/io.h>
#include "moxa_dkcontrol.h" 
/* 
 * Open/release
 */


static int dkcontrol_open (struct inode *inode, struct file *file) 
{
	p("open\n");
	return 0;
}

static int dkcontrol_release (struct inode *inode, struct file *file) 
{
	p("release\n");
	return 0;
}


/* 
 *  I/O control 
 */
static long dkcontrol_ioctl(struct file *file,unsigned int cmd, unsigned long arg) 
{
	struct slot_info   si;
	unsigned char regv;

	if (copy_from_user(&si, (struct slot_info *)arg, sizeof(struct slot_info)))
		return -EFAULT;

	p("si.num=%d\n",si.num);
	p("si.value=%d\n",si.value);
	p("cmd=%d\n",cmd);

	if ((cmd != IOCTL_SET_SLOT) || (si.num > 4) || (si.num < 1) || (si.value > 1) || (si.value < 0)) {
		printk("Incorrect parameter\n");
		return -EINVAL;
	}

	/* 
	 * For the 0x304 register, the value are
	 * bit 7: slot 4, 1 active
	 * bit 6: slot 3, 0 active
	 * bit 5: slot 2, 1 active
	 * bit 4: slot 1, 0 active
	 */
	regv = inb(DKCONTROL_GPIO_ADDR);
	p("current reg value is %x\n", regv);
	switch (si.num) {
		case 4:
			if (si.value)
                                regv |= 0x80;
                        else
                                regv &= 0x7f;
                        break;
		case 3:
			if (!si.value) 
				regv |= 0x40;
			else 
				regv &= 0xbf;
			break;
		case 2:
			if (si.value)
                                regv |= 0x20;
                        else
                                regv &= 0xdf;
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
	.llseek	=	NULL,
	.read		=	NULL,
	.write	=	NULL,
//	.readdir	=	NULL,
	.poll		=	NULL,
	.unlocked_ioctl	=   dkcontrol_ioctl,
	.mmap		=	NULL,
	.open		=   dkcontrol_open,
	.flush	=	NULL,
	.release	=   dkcontrol_release,
	.fsync	=	NULL,
	.fasync	=	NULL,
	.lock		=	NULL,
};

static struct miscdevice mx_miscdev = {
	.minor = MX_MINOR_NUMBER,
	.name = MX_NAME,
	.fops = &mx_fops,
};

static int __init dkcontrol_init_module (void) 
{
	int res;
	printk("initializing MOXA dkcontrol driver\n");

	/* register misc driver */
	res = misc_register(&mx_miscdev);
	if (res) {
		printk("Moxa dkcontrol driver: Register misc fail !\n");
		return res;
	}
	return 0;
}

/*
 * close and cleanup module
 */
static void __exit dkcontrol_cleanup_module (void) 
{
	printk("cleaning up module\n");
	misc_deregister(&mx_miscdev);
}

module_init(dkcontrol_init_module);
module_exit(dkcontrol_cleanup_module);
MODULE_AUTHOR("Wes.Huang@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA dkcontrol module");

