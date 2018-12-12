/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
 * This is Moxa linux embedded platform series device driver.
 * This device driver will control the USB GPIO to power on/off the mini-PCIE module.
 *
 * The memory map:
 * I/O port	: 0x305      For V2616A hardware platform, mini-PCIE module Power on/of control
 *   bit 7: high to disable.
 *   bit 6: low is DI, high is reset.
 *   bit 5: low to reset.
 *
 * History:
 * Date		Aurhor			Comment
 * 01-12-2014	Jared Wu.		Create for V2616A mini-PCIE module power on/off.
 * 11-03-2016	Jared Wu.		Porting for Debian Jessie (linux-3.16).
 *
 */

#include <linux/version.h>
#define VERSION_CODE(ver,rel,seq)	((ver << 16) | (rel << 8) | seq)

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
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

/*
 * Ioctl
 */
#define IOCTL_CELLULAR_POWER	_IOW(MX_USB_GPIO_MINOR,0,int)
#define IOCTL_CELLULAR_IGT	_IOW(MX_USB_GPIO_MINOR,1,int)
#define IOCTL_PCIE_RESET	_IOW(MX_USB_GPIO_MINOR,2,int)
#define IOCTL_PCIE_WDISABLE	_IOW(MX_USB_GPIO_MINOR,3,int)

#define PCIE1_RESET		0x20 /* bit 5: Mini-PCIE reset. Low to reset. (But the module doesn't go back. */
#define REMOTE_RESET		0x40 /* bit 6: Remote hardware reset, control the DIO function. Low is DI, High is reset. */
#define PCIE1_WDISABLE		0x80 /* bit 7: Mini-PCIE power. High to disable. */

struct slot_info {
	int     num;
	int     value;	// 1 for on, 0 for off
 };


#define MX_USB_GPIO_MINOR	108
#define MX_USB_GPIO_NAME	"moxa_usb_gpio"
#define MX_USB_GPIO_ADDR	0x305
#define MX_USB_GPIO_REGION_SIZE	0x1

static struct resource	*mx_usb_gpio_res=NULL;
static struct mutex mx_usb_gpio_sem;

#ifdef __PROC_DEBUG__

#include <linux/proc_fs.h>
#if (LINUX_VERSION_CODE >= VERSION_CODE(3,10,0))
#include <linux/seq_file.h>
#endif

static struct proc_dir_entry *proc_minipcie;

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
int proc_minipcie_read(struct seq_file *m, void *data)
#else
int proc_minipcie_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
#endif
{
	int len=0;
#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
	seq_printf(m, "%x\n", inb(MX_USB_GPIO_ADDR) & 0x01 ? 1: 0 );
#else

	if( offset > 0 )
		return 0;

	/* The bit 0 stored the ac_power information  */
	len += sprintf(buffer, "%x\n", inb(MX_USB_GPIO_ADDR) );
#endif
	return len;
}

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
ssize_t minipcie_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
#else
int minipcie_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
#endif
{
	s32	usbgpo_en_value=0;
	char	kbuffer[64];
	
	if ( copy_from_user(kbuffer, (char*)buffer , count) )
		return -EFAULT;

	kbuffer[count+1] = '\0';
	sscanf(kbuffer, "%d", &usbgpo_en_value);

	printk(KERN_ERR "The input value is :%x\n", usbgpo_en_value);

	outb(usbgpo_en_value , MX_USB_GPIO_ADDR);

	return count;
}

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)

static int minipcie_open (struct inode *inode, struct file *file) 
{
	return single_open(file, proc_minipcie_read, NULL);
}

static struct file_operations proc_minipcie_fops = {
	.owner	=	THIS_MODULE,
	.open	=	minipcie_open,
	.read	=	seq_read,
	.write	=	minipcie_write,
	.llseek	=	seq_lseek,
};
#endif

#endif // #ifdef __PROC_DEBUG__

static int usb_gpio_open (struct inode *inode, struct file *file) 
{
	return 0;
}

static int usb_gpio_release (struct inode *inode, struct file *file) 
{
	return 0;
}


/* 
 *  I/O control 
 */
static long usb_gpio_ioctl(struct file *file,unsigned int cmd, unsigned long arg) 
{
	struct slot_info   si;
	int org_val;
	int retval = 0;

	if ( copy_from_user(&si, (struct slot_info *)arg, sizeof(struct slot_info)) )
		return -EFAULT;

	p("si.num=0x%x, si.value=0x%x\n", si.num, si.value);

	mutex_lock(&mx_usb_gpio_sem);

	org_val = inb(MX_USB_GPIO_ADDR);

	switch (cmd) {
		case IOCTL_PCIE_WDISABLE:
			switch (si.num) {
				case 1:
					p("slot number:%d with invalid slot value:%d\n", si.num, si.value);
					if ( si.value == 0 )
						outb( org_val | PCIE1_WDISABLE, MX_USB_GPIO_ADDR);  // Disable
					else
						outb( org_val & (~PCIE1_WDISABLE), MX_USB_GPIO_ADDR);  // Enable
					break;
				default:
					printk("slot number:%d with invalid slot value:%d\n", si.num, si.value);
					retval=-EINVAL;
					goto goto_error1;
			}
			break;

		case IOCTL_PCIE_RESET:
			switch (si.num) {
				case 1:
					p("slot number:%d with invalid slot value:%d\n", si.num, si.value);
					if ( si.value == 0 )
						outb( org_val & (~PCIE1_RESET), MX_USB_GPIO_ADDR); // Low to reset
					else
						outb( org_val | PCIE1_RESET, MX_USB_GPIO_ADDR);
					break;
				default:
					printk("slot number:%d with invalid slot value:%d\n", si.num, si.value);
					retval=-EINVAL;
					goto goto_error1;
			}
			break;
		default:
			printk("Parameter is incorrect, cmd(%x)!=IOCTL_PCIE_RESET(%lx) or IOCTL_PCIE_WDISABLE(%lx)\n", cmd, IOCTL_PCIE_RESET, IOCTL_PCIE_WDISABLE);
			retval=-EINVAL;
			goto goto_error1;
	}

goto_error1:
	mutex_unlock(&mx_usb_gpio_sem);

	return retval;
}

static struct file_operations mx_usb_gpio_fops = {
	.owner	=	THIS_MODULE,
	.unlocked_ioctl	=	usb_gpio_ioctl,
	.open	=	usb_gpio_open,
	.release=	usb_gpio_release,
};

static struct miscdevice mx_usb_gpio_dev = {
	.minor = MX_USB_GPIO_MINOR,
	.name = MX_USB_GPIO_NAME,
	.fops = &mx_usb_gpio_fops,
};

static int __init usb_gpio_init_module (void) 
{
	int res;
	printk("initializing MOXA USB GPIO driver, v%s.\n", DRIVER_VERSION);

	/* register misc driver */
	res = misc_register(&mx_usb_gpio_dev);
	if ( res < 0 ) {
		printk("Moxa USB GPIO driver: Register misc fail !\n");
		goto misc_register_error;
	}

	mx_usb_gpio_res = request_region(MX_USB_GPIO_ADDR, MX_USB_GPIO_REGION_SIZE, MX_USB_GPIO_NAME);
	if ( mx_usb_gpio_res == NULL ) {
		printk("Moxa USB GPIO request memory region fail !\n");
		goto request_mem_region_error;
	}
	
#ifdef __PROC_DEBUG__
	#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
	proc_minipcie = proc_create("driver/minipcie",0644, NULL, &proc_minipcie_fops);
	#else
	proc_minipcie = create_proc_read_entry("driver/minipcie",0644, NULL, proc_minipcie_read, NULL);
	proc_minipcie->write_proc = minipcie_write;
	#endif
#endif
	mutex_init(&mx_usb_gpio_sem);

	return 0;

request_mem_region_error:
	misc_deregister(&mx_usb_gpio_dev);
misc_register_error:
	return -EIO;
}

/*
 * close and cleanup module
 */
static void __exit usb_gpio_cleanup_module (void) 
{
	printk("cleaning up the resource belongged to MOXA USB GPIO driver\n");

#ifdef __PROC_DEBUG__
	remove_proc_entry("driver/minipcie", proc_minipcie);
#endif

	if ( mx_usb_gpio_res )
		release_region(MX_USB_GPIO_ADDR, MX_USB_GPIO_REGION_SIZE);

	misc_deregister(&mx_usb_gpio_dev);
}

module_init(usb_gpio_init_module);
module_exit(usb_gpio_cleanup_module);
MODULE_AUTHOR("Jared.Wu@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA USB GPIO driver for power off/on the mini-PCIE module");

