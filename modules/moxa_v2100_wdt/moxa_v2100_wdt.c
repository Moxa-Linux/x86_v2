/*
 * This is Moxa v2101, da-682, da-683, da-685, da-682a, v2616a, da-820 watch dog driver.
 *
 * History:
 * Date		Aurhor		Comment
 * 04-23-2013	Jared Wu.	First create for DA-682A.
 * 07-17-2014	Jared Wu.	define __VMWARE__ for VMware ESXi 5.02
 * 07-18-2014	Jared Wu.	Define __PROC_READ_ENTRY__ for debugging
 * 12-27-2016	Jared Wu.	Enable the IT8783F_WOPE to reset the Super IO device
 *                              Create /proc entry for linux-3.10 and later
 */
#define __KERNEL_SYSCALLS__
#include <linux/version.h>
#define	VERSION_CODE(ver,rel,seq)	((ver << 16) | (rel << 8) | seq)

#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#if (LINUX_VERSION_CODE >= VERSION_CODE(3,10,0))
#include <linux/seq_file.h>
#endif
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/reboot.h>
#include <linux/mm.h>
#ifdef __VMWARE__ /* Add for VMware ESXi 5.02 */
#include <asm/uaccess.h>
#include "watchdog.h"
#else
#include <linux/watchdog.h>
#endif

#define DRV_VERSION "1.1"
#include "x86_moxa_moxawdt.h"
#include "moxa_superio.h"

#define	WDT_IN_USE		0
#define	WDT_OK_TO_CLOSE		1

#ifndef WATCHDOG_NOWAYOUT
#define WATCHDOG_NOWAYOUT 0
#endif

volatile unsigned long	wdt_status;
static u32	timeout = DEFAULT_WATCHDOG_TIME;
static s32	nowayout = WATCHDOG_NOWAYOUT;
static DEFINE_SPINLOCK(moxawdt_lock);
/* Fix me: to debug this driver, debug = 1 */
static s32		debug = 0;
#ifdef __PROC_READ_ENTRY__
static struct proc_dir_entry *moxawdt_proc_file;
#endif

static void moxawdt_ack(void)
{
	if (debug)
		printk("<1>moxawdt_ack():%d\n", timeout);
#ifndef __WDT_TEST__
	spin_lock(&moxawdt_lock);
	superio_write(LOGIC_DEVICE_NUMBER, IT8783F_WDT_WTV_0_7, GPIO_CONFIG_REG, (u8) timeout);
	spin_unlock(&moxawdt_lock);
#endif
}

static void moxawdt_enable(void)
{
#ifndef __WDT_TEST__
	unsigned char reg_val;
#endif
	if (debug)
		printk("<1>moxawdt_enable():%d\n", timeout);

#ifndef __WDT_TEST__
	spin_lock(&moxawdt_lock);
	reg_val = superio_read(LOGIC_DEVICE_NUMBER, IT8783F_WDT_CONF, GPIO_CONFIG_REG);
	reg_val &= ~IT8783F_WTVES;
	superio_write(LOGIC_DEVICE_NUMBER, IT8783F_WDT_CONF, GPIO_CONFIG_REG, (u8)(reg_val|IT8783F_WOKE|IT8783F_WTVS|IT8783F_WOPE));
	superio_write(LOGIC_DEVICE_NUMBER, IT8783F_WDT_WTV_0_7, GPIO_CONFIG_REG, (u8)timeout);
	spin_unlock(&moxawdt_lock);
#endif
}

static void moxawdt_disable(void)
{
	if (debug)
		printk("<1>moxawdt_disable\n");
#ifndef __WDT_TEST__
	spin_lock(&moxawdt_lock);
	superio_write(LOGIC_DEVICE_NUMBER, IT8783F_WDT_WTV_0_7, GPIO_CONFIG_REG, 0);
	spin_unlock(&moxawdt_lock);
#endif
}

/* Define __PROC_READ_ENTRY__ for debugging */
#ifdef __PROC_READ_ENTRY__

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
int moxawdt_proc_read(struct seq_file *m, void *data)
#else
int moxawdt_proc_read(char *buffer, char **buffer_location, off_t offset, int count, int *eof, void *data)
#endif
{
    int len = 0; /* The number of bytes actually used */

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
    spin_lock(&moxawdt_lock);

    seq_printf(m,
        "The watchdog heartbit\t: %d sec\n"
        "hardware watchdog counter(LSB)\t: %d sec\n"
        "hardware watchdog counter(MSB)\t: %d sec\n",
        timeout, 
        superio_read(LOGIC_DEVICE_NUMBER, IT8783F_WDT_WTV_0_7, GPIO_CONFIG_REG),
        superio_read(LOGIC_DEVICE_NUMBER, IT8783F_WDT_WTV_8_15, GPIO_CONFIG_REG));

    spin_unlock(&moxawdt_lock);

#else
    if (offset > 0)
    	return 0;

    spin_lock(&moxawdt_lock);
    //Fill the buffer and get its length
    len += sprintf(buffer+len,
        "The watchdog heartbit\t: %d sec\n"
        "hardware watchdog counter(LSB)\t: %d sec\n"
        "hardware watchdog counter(MSB)\t: %d sec\n"
        ,timeout 
        ,superio_read(LOGIC_DEVICE_NUMBER, IT8783F_WDT_WTV_0_7, GPIO_CONFIG_REG)
        ,superio_read(LOGIC_DEVICE_NUMBER, IT8783F_WDT_WTV_8_15, GPIO_CONFIG_REG)
        );
    spin_unlock(&moxawdt_lock);
#endif
    return len;
}

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
static int proc_moxawdt_open (struct inode *inode, struct file *file) 
{
	return single_open(file, moxawdt_proc_read, NULL);
}

static struct file_operations proc_moxawdt_fops = {
	.owner	=	THIS_MODULE,
	.open	=	proc_moxawdt_open,
	.read	=	seq_read,
	.llseek	=	seq_lseek,
};
#endif /* #if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0) */

#endif

static int moxawdt_open(struct inode *inode, struct file *file)
{
	if ( MINOR(inode->i_rdev) != WATCHDOG_MINOR )
		return -ENODEV;
	
	if (test_and_set_bit(WDT_IN_USE, &wdt_status))
		return -EBUSY;

	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);
	moxawdt_enable();

	return 0;
}

static int moxawdt_release(struct inode *inode, struct file *file)
{
	if (test_bit(WDT_OK_TO_CLOSE, &wdt_status))
		moxawdt_disable();
	else
		printk(KERN_CRIT "WATCHDOG: Device closed unexpectedly - "
					"timer will not stop\n");

	clear_bit(WDT_IN_USE, &wdt_status);
	clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

	return 0;
}

static const struct watchdog_info ident = {
	.options	= WDIOF_MAGICCLOSE |
			  WDIOF_SETTIMEOUT | 
			  WDIOF_KEEPALIVEPING ,
	.identity	= "Moxa V2100 based Watchdog",
};

static long moxawdt_ioctl (struct file *file, unsigned int ioc_cmd, unsigned long arg)
{
	int	ret = -ENOTTY;
	int	time;
	int	new_options;

	switch ( ioc_cmd ) {
	case WDIOC_GETSUPPORT:
		return copy_to_user((struct watchdog_info *)arg, &ident,
				   sizeof(ident)) ? -EFAULT : 0;
	case WDIOC_GETSTATUS:
		ret = put_user(1, (int *)arg);
		break;
	case WDIOC_GETBOOTSTATUS:
		ret = put_user(0, (int *)arg);
		break;
	case WDIOC_SETOPTIONS:
		if (get_user(new_options, (int *)arg))
			return -EFAULT;
		if (new_options & WDIOS_DISABLECARD) {
			printk("<1>WDIOS_DISABLECARD\n");
			moxawdt_disable();
			ret = 0;
		}
		if (new_options & WDIOS_ENABLECARD) {
			printk("<1>WDIOS_ENABLECARD\n");
			moxawdt_enable();
			ret = 0;
		}
		break;
	case WDIOC_KEEPALIVE:
		moxawdt_ack();
		ret = 0;
		break;

	case WDIOC_SETTIMEOUT:

		ret = get_user(time, (int *)arg);
		if (ret)
			return -EFAULT;

		if (time <= WATCHDOG_MIN_TIME || time > WATCHDOG_MAX_TIME) {
			ret = -EINVAL;
			break;
		}

		timeout = time;

		moxawdt_ack();
		/* Fall through */

	case WDIOC_GETTIMEOUT:
		ret = put_user(timeout, (int *)arg);
		break;
	default:
		ret = -ENOTTY;
	}

	return ret;
}

/*
 *      moxawdt_write:
 *      @file: file handle to the watchdog
 *      @buf: buffer to write (unused as data does not matter here
 *      @count: count of bytes
 *      @ppos: pointer to the position to write. No seeks allowed
 *
 *      A write to a watchdog device is defined as a keepalive signal. Any
 *      write of data will do, as we we don't define content meaning.
 */

static ssize_t moxawdt_write(struct file *file, const char *buf, \
			    size_t count, loff_t *ppos)
{
	if (count) {
		if (!nowayout) {
			size_t i;

			clear_bit(WDT_OK_TO_CLOSE, &wdt_status);

			for (i = 0; i != count; i++) {
				char c;
				if (get_user(c, buf + i))
					return -EFAULT;
				if (c == 'V')
					set_bit(WDT_OK_TO_CLOSE, &wdt_status);
			}
		}
		moxawdt_ack();
	}

	return count;
}

/**
 *	notify_sys:
 *	@this: our notifier block
 *	@code: the event being reported
 *	@unused: unused
 *
 *	Our notifier is called on system shutdowns. We want to turn the card
 *	off at reboot otherwise the machine will reboot again during memory
 *	test or worse yet during the following fsck. This would suck, in fact
 *	trust me - if it happens it does suck.
 */

static int wdt_notify_sys(struct notifier_block *this, unsigned long code,
	void *unused)
{
	if (code == SYS_DOWN || code == SYS_HALT)
		moxawdt_disable();
	return NOTIFY_DONE;
}

/*
 *	The WDT card needs to learn about soft shutdowns in order to
 *	turn the timebomb registers off.
 */

static struct notifier_block wdt_notifier = {
	.notifier_call = wdt_notify_sys,
};

static struct file_operations moxawdt_fops = {
	.owner=THIS_MODULE,
	.open=moxawdt_open,
	.write=moxawdt_write,
	.unlocked_ioctl=moxawdt_ioctl,
	.release=moxawdt_release,
};

static struct miscdevice wdt_miscdev = {
	.minor = WATCHDOG_MINOR,
	.name = "watchdog",
	.fops = &moxawdt_fops,
};

static int __init moxawdt_init(void) {

	int ret = 0;

	ret = register_reboot_notifier(&wdt_notifier);
	if ( ret ) {
		printk("moxawdt_init(): can't register reboot notifier\n");
		goto moxawdt_init_err1;
	}

	/* set timer_margin */
	if (timeout <= WATCHDOG_MIN_TIME || timeout > WATCHDOG_MAX_TIME) {
		printk("moxawdt_init(): timeout value, %d, should be in %d and %d.\n", timeout, WATCHDOG_MIN_TIME, WATCHDOG_MAX_TIME);
		goto moxawdt_init_err1;
	}

	ret = misc_register(&wdt_miscdev);
	if ( ret ) {
		printk("moxawdt_init(): misc_register() fail !\n");
		goto moxawdt_init_err2;
	}

/* Define __PROC_READ_ENTRY__ for debugging */
#ifdef __PROC_READ_ENTRY__

#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
	moxawdt_proc_file = proc_create("driver/moxawdt",0644, NULL, &proc_moxawdt_fops);
#else
	moxawdt_proc_file = create_proc_read_entry("driver/moxawdt", 0644, NULL, moxawdt_proc_read, NULL);
#endif /* #if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0) */
	if ( !moxawdt_proc_file ) {
        	printk("<1>moxawdt_init(): create_proc_read_entry() fail\n");
		goto moxawdt_init_err3;
	}

#endif
	spin_lock_init(&moxawdt_lock);

	printk("Initializing moxa_v2100_wdt module, %s\n", DRV_VERSION);
	printk (KERN_INFO "initialized. (nowayout=%d)\n", nowayout);
	printk (KERN_INFO "initialized. (debug=%d)\n", debug);
	printk (KERN_INFO "initialized. (timeout=%d)\n", timeout);

	return 0;

/* Define __PROC_READ_ENTRY__ for debugging */
#ifdef __PROC_READ_ENTRY__
moxawdt_init_err3:
	misc_deregister(&wdt_miscdev);
#endif
moxawdt_init_err2:
	unregister_reboot_notifier(&wdt_notifier);
moxawdt_init_err1:
	return ret;
}

static void __exit moxawdt_exit(void) {

	unregister_reboot_notifier(&wdt_notifier);

/* Define __PROC_READ_ENTRY__ for debugging */
#ifdef __PROC_READ_ENTRY__
	remove_proc_entry("driver/moxawdt", NULL);
#endif
	misc_deregister(&wdt_miscdev);
}

module_init(moxawdt_init);
module_exit(moxawdt_exit);

MODULE_AUTHOR("Jared.Wu@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Moxa V2100 based WDT driver");
module_param(timeout, int, 0644);
MODULE_PARM_DESC(timeout, "Watchdog timeout in seconds. 2 <= timeout <=255, default="__MODULE_STRING(WATCHDOG_TIMEOUT) ".");
module_param(nowayout, int, 0644);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default=CONFIG_WATCHDOG_NOWAYOUT)");
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Print the verbose debug message");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
