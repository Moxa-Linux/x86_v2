/*
 * This driver is for Moxa embedded computer programmable LED driver.
 *
 * History:
 * Date		Aurhor		Comment
 * 04-24-2013	Jared Wu.	First create for DA-682A.
 * 03-18-2014	Jared Wu.	First create for DA-820.
 * 11-10-2016	Jared Wu.	First create for DA-682B.
 * 07-19-2018	Harry YJ Jhou.	Merge V2201 mini-PCIE cellular module power on/off.
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

#ifdef DA683
#include "da683.h"
#elif defined DA682A
#include "da682a.h"
#elif defined DA681A
#include "da681a.h"
#elif defined DA682B
#include "da682b.h"
#elif defined DA710
#include "da710.h"
#elif defined V2201
#include "v2201.h"
#elif defined DA820
#include "da820.h"
#endif

/* mknod /dev/pled c 10 105 for this module */
#define MOXA_PLED_MINOR 105
#define NAME		"pled"
#define DRV_VERSION	"v1.3"

/* Debug message */
// #define DEBUG
#ifdef DEBUG
#define dbg_printf(x...)        printk(x)
#else
#define dbg_printf(x...)
#endif

// open function - called when the "file" /dev/modelname_io is opened in userspace
static int pled_open (struct inode *inode, struct file *file) {
	dbg_printf("pled_open\n");
	return 0;
}


// close function - called when the "file" /dev/modelname_pled is closed in userspace  
static int pled_release (struct inode *inode, struct file *file) {
	dbg_printf("pled_release\n");
	return 0;
}


/* Write function
 * Note: use echo 1111 > /dev/pled 
 * The order is [pled1, pled2, pled3, pled4, ...\n]
 */
ssize_t pled_write (struct file *filp, const char __user *buf, size_t count,
		loff_t *pos)
{
	int ret;
	int i;
	u8 pled_status = 0;
	char stack_buf[LED_NUM];

	dbg_printf("Enter pled_write\n");

	/* check input value */
	dbg_printf("Check the input size\n");
	if ( count != LED_NUM+1 ) { /* 9 = 8 digits + newline */
		printk( KERN_ERR "Moxa pled error! paramter should be %d digits, like \"%s\" \n", LED_NUM, LED_PATERN);
		return -EINVAL;
	}

	/* set control device */
	superio_config(count);

	/* parse strint to hex: 
	* stack_buf=[ pled1 pled2 pled3 pled4 ...\n]
	*/
	
	ret = copy_from_user(stack_buf, buf, count-1);
	/* 
	 * Jared, 2016/11/10, keep the DO0 and DO1 status
	 * bitmap: bit 6~7 =DO0, DO1  bit 0~3 = PLED0 ,PLED1, PLED2, PLED3.
	 */
	/*
	pled_status = moxainb(LED_PORT) & 0xF0;
	*/

	for (i = 0; i < LED_NUM; i++) {
		if (stack_buf[i] == '1')  /* change status to 1, because high activate */
			pled_status |= 1<<i;
		else if (stack_buf[i] == '0')  {
			// Nothing to do
		}
		else {
			printk("pled: error, you input is %s", stack_buf);
			goto end;
		}
	}

	if ( ACTIVE_LOW ) {
	/* 
	 * Jared, 2016/12/20, keep the DO0 and DO1 status
	 * bitmap: bit 6~7 =DO0, DO1  bit 0~3 = PLED0 ,PLED1, PLED2, PLED3.
	 */
		//pled_status = (moxainb(LED_PORT) & 0xF0) | ( ~pled_status & 0x0F);
		pled_status = (moxainb(LED_PORT) & (~LED_MASK)) | ( ~pled_status & LED_MASK);
	}

	dbg_printf("val=%02x\n",pled_status);

	/* change status */
	moxaoutb(pled_status, LED_PORT);

	dbg_printf("pled: set status finish!");
end:
	return count;
}

ssize_t pled_read (struct file *filp, char __user *buf, size_t count,
		loff_t *pos)
{
	int i, ret;
	u8 pled_status;
	char stack_buf[LED_NUM];
	unsigned char bit_mask = 0x01;

	if ( count <= LED_NUM ) { /* 8 digits  */
		printk( KERN_ERR "Moxa pled error! paramter should be %d digits.\n", LED_NUM);
		return -EINVAL;
	}

	pled_status=moxainb(LED_PORT);
	dbg_printf("%s[%d]pled status:%x\n",__FUNCTION__,__LINE__, pled_status);

	memset(stack_buf, '0', LED_NUM *sizeof(char) );
	for (i = 0; i < LED_NUM; i++) {
		if ( !(pled_status & (bit_mask<<i)) )
			stack_buf[i] = '1';
	}
	dbg_printf("%s[%d]pled status:%s\n",__FUNCTION__,__LINE__, stack_buf);

	ret = copy_to_user((void*)buf, (void*)stack_buf, sizeof(char)*LED_NUM);
	if( ret	< 0 ) {
		printk( KERN_ERR "Moxa pled error! paramter should be %d digits, like \n", LED_NUM);
		return -ENOMEM;
	}

	return LED_NUM;
}

#ifdef V2201
static long usb_gpio_ioctl(struct file *file,unsigned int cmd, unsigned long arg)
{
	struct slot_info   si;
        u8 org_val;
        int retval = 0;

	printk("ioctl pled\n");
	if ( copy_from_user(&si, (struct slot_info *)arg, sizeof(struct slot_info)) )
                return -EFAULT;

	
	/* set control device */
	superio_config(count);


	org_val = ~moxainb(LED_PORT);
	//printk("org_val=%x\n", org_val);
	
	switch (cmd) {
		case IOCTL_POWER_OFF:
			if ( si.num == 0 )
				org_val = org_val | 1<<2 ;
			else if ( si.num == 1 )
				org_val = org_val | 1<<4 ;

			printk("pcie slot #%d power off\n", si.num);
			break;
		case IOCTL_POWER_ON:
			if ( si.num == 0 )
				org_val = org_val & (~(1<<2));
			else if ( si.num == 1)
				org_val = org_val & (~(1<<4));
			printk("pcie slot #%d power on\n", si.num);
			break;
		default:
			printk("Parameter is incorrect, cmd(%x)!=IOCTL_POWER_OFF(%lu) or IOCTL_POWER_ON(%lu)",
				cmd, IOCTL_POWER_OFF, IOCTL_POWER_ON);
			break;
	}

	if ( ACTIVE_LOW )
		org_val = ~org_val;

	/* change status */
	moxaoutb(org_val, LED_PORT);

	return retval;
}
#endif // defined for V2201

// define which file operations are supported
struct file_operations pled_fops = {
	.owner	=	THIS_MODULE,
	.write	=	pled_write,
	.read	=	pled_read,
#ifdef V2201
	.unlocked_ioctl	=	usb_gpio_ioctl,
#endif // defined for V2201
	.open	=	pled_open,
	.release=	pled_release,
};

// register as misc driver
static struct miscdevice pled_miscdev = {
	.minor = MOXA_PLED_MINOR,
	.name = NAME,
	.fops = &pled_fops,
};

// initialize module (and interrupt)
static int __init pled_init_module (void) {
	printk("Initializing MOXA pled module, %s\n", DRV_VERSION);

	// register misc driver
	if ( misc_register(&pled_miscdev)!=0 ) {
		printk("Moxa pled driver: Register misc fail !\n");
		return -ENOMEM;
	}

	moxa_platform_led_init();

	return 0;
}

// close and cleanup module
static void __exit pled_cleanup_module (void) {
	printk("Unregister MOXA pled module, %s\n", DRV_VERSION);
	misc_deregister(&pled_miscdev);
}

module_init(pled_init_module);
module_exit(pled_cleanup_module);
MODULE_AUTHOR("jared.wu@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA programmable LED device driver");

