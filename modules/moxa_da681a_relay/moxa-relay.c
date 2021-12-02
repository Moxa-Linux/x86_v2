/*
 * This driver is for Moxa embedded computer programmable relay driver.
 *
 * History:
 * Date		Aurhor		Comment
 * 07-18-2015	Jared Wu.	First create for DA-681A.
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

#include "da681a.h"
#define DRV_VERSION "1.1"

/* mknod /dev/relay c 10 108 for this module */
#define MOXA_RELAY_MINOR 108
#define NAME		"relay"

#ifdef DEBUG
#define dbg_printf(x...)        printk(x)
#else
#define dbg_printf(x...)
#endif

static int relay_open (struct inode *inode, struct file *file) {
	dbg_printf("relay_open\n");
	return 0;
}


static int relay_release (struct inode *inode, struct file *file) {
	dbg_printf("relay_release\n");
	return 0;
}


/* Write function
 * Note: use echo 1 > /dev/relay 
 */
ssize_t relay_write (struct file *filp, const char __user *buf, size_t count,
		loff_t *pos)
{
	int ret;
	int i;
	u8 relay_status;
	char stack_buf[RELAY_NUM];

	dbg_printf("Enter relay_write\n");

	/* check input value */
	dbg_printf("Check the input size\n");
	if ( count != RELAY_NUM+1 ) { /* 2 = 1 digits + newline */
		printk( KERN_ERR "Moxa relay error! paramter should be %d digits, like \"%s\" \n", RELAY_NUM, RELAY_PATERN);
		return -EINVAL;
	}

	/* set control device */
	superio_config(count);

	/* parse strint to hex: 
	* stack_buf=[ relay1 \n]
	*/
	
	ret = copy_from_user(stack_buf, buf, count-1);
	relay_status=0x00;
	for (i = 0; i < RELAY_NUM; i++) {
		if (stack_buf[i] == '1')  /* change status to 1, because high activate */
			relay_status |= 1<<i;
		else if (stack_buf[i] == '0')  {
			// Nothing to do
		}
		else {
			printk("relay: error, you input is %s", stack_buf);
			goto end;
		}
	}

	if ( ACTIVE_LOW )
		relay_status = ~relay_status;

	dbg_printf("val=%02x\n",relay_status);

	/* change status */
	moxaoutb(relay_status, RELAY_PORT);
	dbg_printf("relay: set status finish!");
end:
	return count;
}

ssize_t relay_read (struct file *filp, char __user *buf, size_t count,
		loff_t *pos)
{
	int i, ret;
	u8 relay_status;
	char stack_buf[RELAY_NUM];
	unsigned char bit_mask = 0x01;

	if ( count <= RELAY_NUM ) { /* 2 digits  */
		printk( KERN_ERR "Moxa relay error! paramter should be %d digits.\n", RELAY_NUM);
		return -EINVAL;
	}

	relay_status=moxainb(RELAY_PORT);
	dbg_printf("%s[%d]relay status:%x\n",__FUNCTION__,__LINE__, relay_status);

	memset(stack_buf, '0', RELAY_NUM *sizeof(char) );
	for (i = 0; i < RELAY_NUM; i++) {
		if ( !(relay_status & (bit_mask<<i)) )
			stack_buf[i] = '1';
	}
	dbg_printf("%s[%d]relay status:%s\n",__FUNCTION__,__LINE__, stack_buf);

	ret = copy_to_user((void*)buf, (void*)stack_buf, sizeof(char)*RELAY_NUM);
	if( ret	< 0 ) {
		printk( KERN_ERR "Moxa relay error! paramter should be %d digits, like \n", RELAY_NUM);
		return -ENOMEM;
	}

	return RELAY_NUM;
}

struct file_operations relay_fops = {
	.owner	=	THIS_MODULE,
	.write	=	relay_write,
	.read	=	relay_read,
	.open	=	relay_open,
	.release=	relay_release,
};

static struct miscdevice relay_miscdev = {
	.minor = MOXA_RELAY_MINOR,
	.name = NAME,
	.fops = &relay_fops,
};

static int __init relay_init_module (void) {

	printk("Initializing MOXA relay module, %s\n", DRV_VERSION);

	if ( misc_register(&relay_miscdev)!=0 ) {
		printk(KERN_ERR "Moxa relay driver: Register misc fail !\n");
		return -ENOMEM;
	}

	moxa_platform_led_init();

	return 0;
}

// close and cleanup module
static void __exit relay_cleanup_module (void) {

	printk("Uninitializing MOXA relay module, %s\n", DRV_VERSION);
	misc_deregister(&relay_miscdev);
}

module_init(relay_init_module);
module_exit(relay_cleanup_module);
MODULE_AUTHOR("jared.wu@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA programmable relay device driver");

