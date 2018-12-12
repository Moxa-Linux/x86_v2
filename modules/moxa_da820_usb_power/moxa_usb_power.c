/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA Licens .  See the file COPYING-MOXA for details.
*/
/*
 * Thiw i1 Moxa linux embedded platform series device driver.
 * This device driver will control the USB POWER to power on/off the USB ports.
 *
 * The memory map:
 * I/O port	: 0x303      For DA-820 platform, USB ports power on/of control
 *   bit 0: Rear USB ports (4 ports)
 *   bit 6: Fron USB ports (2 ports)
 *
 * History:
 * Date		Aurhor			Comment
 * 03-18-2014	Jared Wu.		Create for USB ports power on/off.
 *
 */

#include <linux/version.h>
#define	VERSION_CODE(ver,rel,seq)	((ver << 16) | (rel << 8) | seq)

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/uaccess.h>

#ifdef DEBUG 
	#define p(fmt, args...) printk("%s: "fmt, __FUNCTION__, ##args);
#else
	#define p(fmt, args...) 
#endif 

#define DRIVER_VERSION	"1.0"

#define MX_USB_POWER_MINOR	109
#define MX_USB_POWER_NAME	"moxa_usb_power"
#define MX_USB_POWER_ADDR	0x303
#define MX_USB_POWER_REGION_SIZE	0x1

/*
 * Ioctl
 */
#define IOCTL_SET_FRONT_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,0,int)
#define IOCTL_GET_FRONT_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,1,int)
#define IOCTL_SET_REAR_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,2,int)
#define IOCTL_GET_REAR_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,3,int)

#define FRONT_END	0x40 /* bit 6: Fron-end USB power on(1)/off(0).*/
#define REAR_END	0x01 /* bit 0: Rear-end USB power on(1)/off(0).*/

static struct resource	*mx_usb_power_res=NULL;
static struct mutex mx_usb_power_sem;

#if (LINUX_VERSION_CODE >= VERSION_CODE(3,10,0))
#include <linux/seq_file.h>
#endif

static struct proc_dir_entry *proc_front_end_usb_power;
static struct proc_dir_entry *proc_rear_end_usb_power;

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
int proc_read_front_end_usb_power(struct seq_file *m, void *data)
#else
int proc_read_front_end_usb_power(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
#endif
{
	int len=0;
#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
	seq_printf(m, "%x\n", inb(MX_USB_POWER_ADDR) & FRONT_END ? 1: 0 );
#else

	if( offset > 0 )
		return 0;

	/* The bit 0 stored the front-end information  */
	len += sprintf(buffer, "%x\n", inb(MX_USB_POWER_ADDR) & FRONT_END ? 1: 0  );
#endif
	return len;
}

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
ssize_t proc_write_front_end_usb_power(struct file *file, const char __user *buffer, size_t count, loff_t *data) 
#else
int proc_write_front_end_usb_power(struct file *file, const char __user *buffer, unsigned long count, void *data)
#endif
{
	s32	usb_power_value=0;
	char	kbuffer[64];
	unsigned long retval;
	
	retval = copy_from_user(kbuffer, (char*)buffer , count);	
	if ( retval < 0 )
		return -EFAULT;

	kbuffer[count+1] = '\0';
	sscanf(kbuffer, "%d", &usb_power_value);

	if ( usb_power_value )
		outb( (inb(MX_USB_POWER_ADDR) | FRONT_END) , MX_USB_POWER_ADDR);
	else
		outb( (inb(MX_USB_POWER_ADDR) & (~FRONT_END)) , MX_USB_POWER_ADDR);

	return count;
}

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
int proc_read_rear_end_usb_power(struct seq_file *m, void *data)
#else
int proc_read_rear_end_usb_power(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
#endif
{
	int len=0;
#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
	seq_printf(m, "%x\n", inb(MX_USB_POWER_ADDR) & REAR_END ? 1 : 0 );
#else
	if( offset > 0 )
		return 0;

	/* The bit 6 stored the rear-end information  */
	len += sprintf(buffer, "%x\n", inb(MX_USB_POWER_ADDR) & REAR_END ? 1 : 0);
#endif
	return len;
}

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
ssize_t proc_write_rear_end_usb_power(struct file *file, const char __user *buffer, size_t count, loff_t *data) 
#else
int proc_write_rear_end_usb_power(struct file *file, const char __user *buffer, unsigned long count, void *data) 
#endif
{
	s32	usb_power_value=0;
	char	kbuffer[64];
	unsigned long retval;

	retval = copy_from_user(kbuffer, (char*)buffer , count);	
	if ( retval < 0 )
		return -EFAULT;

	kbuffer[count+1] = '\0';
	sscanf(kbuffer, "%d", &usb_power_value);

	if ( usb_power_value )
		outb( (inb(MX_USB_POWER_ADDR) | REAR_END) , MX_USB_POWER_ADDR);
	else
		outb( (inb(MX_USB_POWER_ADDR) & (~REAR_END)) , MX_USB_POWER_ADDR);

	return count;
}

static int usb_power_open (struct inode *inode, struct file *file) 
{
	return 0;
}

static int usb_power_release (struct inode *inode, struct file *file) 
{
	return 0;
}


/* 
 *  I/O control 
 */
static long usb_power_ioctl(struct file *file,unsigned int cmd, unsigned long arg) 
{
	int org_val;
	int on_off;
	int retval = 0;

	retval = copy_from_user(&on_off, (const void *)arg, sizeof(int));
	if ( retval < 0 )
		return -EFAULT;

	mutex_lock(&mx_usb_power_sem);

	org_val = inb(MX_USB_POWER_ADDR);

	switch (cmd) {
		case IOCTL_SET_FRONT_END_USB_POWER:
			p("Set front-end USB power: %d\n", on_off);
			if ( on_off == 0 )
				outb( org_val & (~FRONT_END), MX_USB_POWER_ADDR);  // Disable
			else
				outb( org_val | FRONT_END, MX_USB_POWER_ADDR);  // Enable
			break;

		case IOCTL_GET_FRONT_END_USB_POWER:
			on_off = inb(MX_USB_POWER_ADDR) & FRONT_END;
			p("Get front-end USB power: %d\n", on_off);
			if( copy_to_user((void*)arg, (void*)&on_off, sizeof(int)) != 0 )
				retval = -EFAULT;

			break;
		case IOCTL_SET_REAR_END_USB_POWER:
			p("Set rear-end USB power: %d\n", on_off);
			if ( on_off == 0 )
				outb( org_val & (~REAR_END), MX_USB_POWER_ADDR);  // Disable
			else
				outb( org_val | REAR_END, MX_USB_POWER_ADDR);  // Enable
			break;
		case IOCTL_GET_REAR_END_USB_POWER:
			on_off = inb(MX_USB_POWER_ADDR) & REAR_END;
			p("Get rear-end USB power: %d\n", on_off);
			if( copy_to_user((void*)arg, (void*)&on_off, sizeof(int)) != 0 )
				retval = -EFAULT;

			break;
		default:
			printk("Inknown IOCTL number:%d\n", cmd);
			retval = -EINVAL;
			goto goto_error1;
			break;
	}

goto_error1:
	mutex_unlock(&mx_usb_power_sem);

	return retval;
}

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)

static int proc_front_end_usb_power_open (struct inode *inode, struct file *file) 
{
	return single_open(file, proc_read_front_end_usb_power, NULL);
}

static struct file_operations proc_front_end_usb_power_fops = {
	.owner	=	THIS_MODULE,
	.open	=	proc_front_end_usb_power_open,
	.read	=	seq_read,
	.write	=	proc_write_front_end_usb_power,
	.llseek	=	seq_lseek,
};

static int proc_rear_end_usb_power_open (struct inode *inode, struct file *file) 
{
	return single_open(file, proc_read_rear_end_usb_power, NULL);
}

static struct file_operations proc_rear_end_usb_power_fops = {
	.owner	=	THIS_MODULE,
	.open	=	proc_rear_end_usb_power_open,
	.read	=	seq_read,
	.write	=	proc_write_rear_end_usb_power,
	.llseek	=	seq_lseek,
};
#endif

static struct file_operations mx_usb_power_fops = {
	.owner	=	THIS_MODULE,
	.unlocked_ioctl	=	usb_power_ioctl,
	.open	=	usb_power_open,
	.release=	usb_power_release,
};

static struct miscdevice mx_usb_power_dev = {
	.minor = MX_USB_POWER_MINOR,
	.name = MX_USB_POWER_NAME,
	.fops = &mx_usb_power_fops,
};

static int __init usb_power_init_module (void) 
{
	int res;
	printk("initializing MOXA USB POWER driver, v%s.\n", DRIVER_VERSION);

	/* register misc driver */
	res = misc_register(&mx_usb_power_dev);
	if ( res < 0 ) {
		printk("Moxa USB POWER driver: Register misc fail !\n");
		goto misc_register_error;
	}

	mx_usb_power_res = request_region(MX_USB_POWER_ADDR, MX_USB_POWER_REGION_SIZE, MX_USB_POWER_NAME);
	if ( mx_usb_power_res == NULL ) {
		printk("Moxa USB POWER request memory region fail !\n");
		goto request_mem_region_error;
	}
#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
	proc_front_end_usb_power = proc_create("driver/moxa_front_end_usb_power",0644, NULL, &proc_front_end_usb_power_fops);
#else
	proc_front_end_usb_power = create_proc_read_entry("driver/moxa_front_end_usb_power",0644, NULL, proc_read_front_end_usb_power, NULL);
	proc_front_end_usb_power->write_proc = proc_write_front_end_usb_power;
#endif

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
	proc_rear_end_usb_power = proc_create("driver/moxa_rear_end_usb_power",0644, NULL, &proc_rear_end_usb_power_fops);
#else
	proc_rear_end_usb_power = create_proc_read_entry("driver/moxa_rear_end_usb_power",0644, NULL, proc_read_rear_end_usb_power, NULL);
	proc_rear_end_usb_power->write_proc = proc_write_rear_end_usb_power;
#endif

	mutex_init(&mx_usb_power_sem);

	return 0;

request_mem_region_error:
	misc_deregister(&mx_usb_power_dev);
misc_register_error:
	return -EIO;
}

/*
 * close and cleanup module
 */
static void __exit usb_power_cleanup_module (void) 
{
	printk("cleaning up the resource belongged to MOXA USB POWER driver\n");

	//remove_proc_entry("driver/moxa_rear_end_usb_power", proc_rear_end_usb_power);
	remove_proc_entry("driver/moxa_rear_end_usb_power", NULL);
	//remove_proc_entry("driver/moxa_front_end_usb_power", proc_front_end_usb_power);
	remove_proc_entry("driver/moxa_front_end_usb_power", NULL);

	if ( mx_usb_power_res )
		release_region(MX_USB_POWER_ADDR, MX_USB_POWER_REGION_SIZE);

	misc_deregister(&mx_usb_power_dev);
}

module_init(usb_power_init_module);
module_exit(usb_power_cleanup_module);
MODULE_AUTHOR("Jared.Wu@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA USB POWER driver for power off/on the USB ports");

