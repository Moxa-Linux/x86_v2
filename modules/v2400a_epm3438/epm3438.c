/*  Copyright (C) MOXA Inc. All rights reserved.
 *
 *      This software is distributed under the terms of the
 *          MOXA License.  See the file COPYING-MOXA for details.
 *
 *     File name: epm-3438.c
 *
 *        DIO/Counter Linux device driver for EPM-3438
 *		Base port = PCI config register 0x20
 *		DO = base port + 1	  (byte access)
 *		DI = base port + 2	  (byte access)
 *		Counter = base port + 4 (word access), 0 ~ 65535
 *
 *	  Device file:
 *	  	mknod /dev/epm_3438_dio c $major 0
 *	  	mknod /dev/epm_3438_counter c $major 1
 *
 *	R00 06/23/10 Jared Wu      New release for EPM-3438-TB
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/ioport.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/spinlock.h>

//#define __TIMER_VERSION__ // timer polling
#define TIMER_INTERVAL HZ/100

#define DRIVER_VERSION		"1.0"

#define MODEL_NAME		"EPM_3438"
#define EPM_3438_DIO_1		"epm_3438_dio1"
#define EPM_3438_COUNTER_1	"epm_3438_counter1"
#define EPM_3438_DIO_2		"epm_3438_dio2"
#define EPM_3438_COUNTER_2	"epm_3438_counter2"
#define MOXA_EPM3438_DEVICE_NUM	2

#ifndef PCI_VENDOR_ID_MOXA
#define	PCI_VENDOR_ID_MOXA	0x1393	// Moxa vendor ID
#endif

#ifndef PCI_DEVICE_ID_EPM3438
#define PCI_DEVICE_ID_EPM3438	0xA001	// EPM-3438-TB device ID
#endif

//
// DIO file operaiton function call
//

#define MAX_DIO                 8
#define DIO_INPUT               1
#define DIO_OUTPUT              0
#define DIO_HIGH                1
#define DIO_LOW                 0
#define IOCTL_DIO_GET_MODE      1
#define IOCTL_DIO_SET_MODE      2
#define IOCTL_DIO_GET_DATA      3
#define IOCTL_DIO_SET_DATA      4
#define IOCTL_SET_DOUT          15
#define IOCTL_GET_DOUT          16
#define IOCTL_GET_DIN           17
#define IOCTL_RESET_COUNTER	18

typedef struct __epm3438 {
	unsigned long base;
	unsigned long size;
	unsigned int irq;
	unsigned int counter_keep;		// preceding counter state
	unsigned char di_state_keep;		// current di state
	unsigned char do_state_keep;		// current do state
	struct semaphore counter_sem;		// lock for counter
#ifdef __TIMER_VERSION__	// Use kernel timer
	unsigned char pre_di_state;		// preceding di state
	unsigned char di_toggled_flag;		// DI state changed flag
	wait_queue_head_t poll_wq;
	int polling_on;		// kernel timer polling is started
	struct timer_list polling;
	spinlock_t di_lock;			// lock for di state
#endif
} epm3438_struct;

struct dio_set_struct {
        int     io_number;
        int     mode_data; // 1:input, 0:output, 1:high, 0:low
};

// Define the function prototype for EPM-3438
int epm3438_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
void epm3438_remove(struct pci_dev *pdev);

int epm3438_open (struct inode *inode, struct file *fp);
long epm3438_ioctl (struct file *fp, unsigned int cmd, unsigned long arg);
ssize_t epm3438_read(struct file *fp, char *buffer, size_t length, loff_t *offset);
int epm3438_release (struct inode *inode, struct file *fp);

#ifdef __TIMER_VERSION__ // timer
unsigned int epm3438_poll (struct file *fp, struct poll_table_struct *ptable);
void polling_function(unsigned long data);
#endif

// Defined the golbal variables
// This variable is for Epm3438
static epm3438_struct Epm3438[2];

static int Major=0;
static struct cdev epm_3438_cdev;
static struct class *charmodule_class; 
#ifdef __DEBUG__
static int epm3438_debug=0;
#endif
static int epm3438_DO2LOW=0;

static struct file_operations moxa_epm3438_fops = {
	.owner = THIS_MODULE,
	.open = epm3438_open,
	.unlocked_ioctl = epm3438_ioctl,
	.read = epm3438_read,
#ifdef __TIMER_VERSION__	// Use kernel timer
	.poll = epm3438_poll,
#endif
	.release = epm3438_release,
};

static  struct pci_device_id    epm3438_pcibrds[] = {
	 {PCI_VENDOR_ID_MOXA, PCI_DEVICE_ID_EPM3438, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (unsigned long*)&Epm3438},
	 {0}
};

static struct pci_driver epm3438_driver = {
	.name = "epm3438",
	.id_table = epm3438_pcibrds,
	.probe = epm3438_probe,
	.remove = epm3438_remove
};

void inline moxaoutb(unsigned char byte, unsigned port) {
    outb(byte, port);
}

unsigned char inline moxainb(unsigned port) {
    return inb(port);
}

void inline moxaoutw(unsigned short word, unsigned port) {
    outb(word, port);
}

unsigned short inline moxainw(unsigned port) {
    return inw(port);
}

int epm3438_open (struct inode *inode, struct file *fp)
{
	unsigned int    dev = iminor(fp->f_path.dentry->d_inode);

	if ( dev==0 || dev==1 ) { // Board #1
		fp->private_data=&Epm3438[0];
	}
	else { // Board #2
		fp->private_data= &Epm3438[1];
	}

#ifdef __TIMER_VERSION__ // timer
	init_waitqueue_head(&pEpm3438->poll_wq);
	if ( Epm3438.polling_on == 0 ) {
		init_timer(&Epm3438.polling);
		Epm3438.polling.function=polling_function;
		Epm3438.polling.data=(unsigned long)fp->private_data;
		Epm3438.polling.expires=jiffies+ TIMER_INTERVAL;
		add_timer(&Epm3438.polling);
	}
#endif

	return 0;
}

ssize_t epm3438_read(struct file *fp, char *buffer, size_t length, loff_t *offset) {
	ssize_t len = 0;
	unsigned int    dev = iminor(fp->f_path.dentry->d_inode);
	epm3438_struct *pEpm3438 = (epm3438_struct *)fp->private_data;

	#ifdef __TIMER_VERSION__
	if ( dev == 0 || dev == 2 ) { // Device 0/2: DIO
		#ifndef __TIMER_VERSION__
		pEpm3438->di_state_keep = moxainb(pEpm3438->base+2);
		#else		// Use kernel timer
		if ( pEpm3438->di_toggled_flag == 0 ) {
			wait_event_interruptible(pEpm3438->poll_wq, pEpm3438->di_toggled_flag );
			if (signal_pending(current)) {
				return -ERESTARTSYS;
			}
		}
		#endif
        	put_user(&pEpm3438->di_state_keep, buffer++);
		len++;
	}
	else
	#endif
	if ( dev == 1 || dev == 3 ){ // Device 1/3: counter

//		if (down_interruptible(&pEpm3438->counter_sem)) {
			/* something went wrong with wait */
//			return -ERESTARTSYS;
//		}
		down(&pEpm3438->counter_sem);

		#ifndef __TIMER_VERSION__	// Not use kernel timer
		pEpm3438->counter_keep = moxainw(pEpm3438->base+4);
		//printk("%s[%d]pEpm3438->base+4:%x,counter_keep:%x\n", __FUNCTION__, __LINE__, pEpm3438->base+4, pEpm3438->counter_keep);
		#endif

		up(&pEpm3438->counter_sem);

		if( copy_to_user(buffer, &pEpm3438->counter_keep, sizeof(unsigned int))!=0 )
			return -EFAULT;
		len+=sizeof(unsigned int);
	}
	else {
		printk("No device: %d\n", dev);
		return -ENODEV;
	}

	return len;
}

long epm3438_ioctl (struct file *fp, unsigned int cmd, unsigned long arg)
{
	struct dio_set_struct   set;
	epm3438_struct *pEpm3438 = (epm3438_struct *)fp->private_data;

	switch ( cmd ) {
		case IOCTL_RESET_COUNTER :
			moxaoutw(0, pEpm3438->base+4);
			return 0;
			break;
		case IOCTL_SET_DOUT :

		if ( copy_from_user(&set, (struct dio_set_struct *)arg, sizeof(struct dio_set_struct)) )
			return -EFAULT;
        	else if ( set.io_number < 0 || set.io_number >= MAX_DIO )
			return -EINVAL;

#ifdef __DEBUG__
		printk(KERN_DEBUG"%s[%d]set.io_number:%d..\n", __FUNCTION__, __LINE__, set.io_number);
#endif

		if ( set.mode_data == DIO_HIGH )
			pEpm3438->do_state_keep |= (1<<set.io_number);
		else if ( set.mode_data == DIO_LOW )
			pEpm3438->do_state_keep &= ~(1<<set.io_number);
		else
			return -EINVAL;

#ifdef __DEBUG__
		printk(KERN_DEBUG"%s[%d]do_state_keep:%x..\n", __FUNCTION__, __LINE__, pEpm3438->do_state_keep);
#endif

		moxaoutb(pEpm3438->do_state_keep, pEpm3438->base+1 ); // DO: base+1

		break;

		case IOCTL_GET_DOUT :
		case IOCTL_GET_DIN :

		if ( copy_from_user(&set, (struct dio_set_struct *)arg, sizeof(struct dio_set_struct)) )
			return -EFAULT;

#ifdef __DEBUG__
    		printk(KERN_DEBUG"%s,%d Enter get_dio...\n", __FUNCTION__, __LINE__);
#endif

		if ( set.io_number == -1 ) {	// to get all port
			if ( cmd == IOCTL_GET_DOUT ){
				set.mode_data = pEpm3438->do_state_keep & 0xff;

#ifdef __DEBUG__
				printk(KERN_DEBUG"%s[%d] :GET_DOUT:%x\n", __FUNCTION__, __LINE__,(unsigned long)set.mode_data);
#endif
			} else {
				set.mode_data = moxainb(pEpm3438->base+2) & 0xff;

#ifdef __DEBUG__
				printk(KERN_DEBUG"%s,%d :GET_DIO:%x\n", __FUNCTION__, __LINE__,(unsigned long)set.mode_data);
#endif
			}
			goto ioctl_get_label;
		}
		if ( set.io_number < 0 || set.io_number >= MAX_DIO )
			return -EINVAL;
		if ( cmd == IOCTL_GET_DOUT ) {
			if ( pEpm3438->do_state_keep & (1<<set.io_number) )
				set.mode_data = 1;
			else
				set.mode_data = 0;

#ifdef __DEBUG__
			printk(KERN_DEBUG"%s[%d]:%x, do_state_keep:%x\n", __FUNCTION__, __LINE__,(unsigned long)set.mode_data, pEpm3438->do_state_keep);
#endif
		} else {
#ifndef __TIMER_VERSION__	// if not use kernel timer
			pEpm3438->di_state_keep = moxainb(pEpm3438->base+2);
#endif
			if ( pEpm3438->di_state_keep & (1<<set.io_number) )
				set.mode_data = 1;
			else
				set.mode_data = 0;

#ifdef __DEBUG__
			printk(KERN_DEBUG"%s[%d]:%x, di_state_keep:%x\n", __FUNCTION__, __LINE__,(unsigned long)set.mode_data, pEpm3438->di_state_keep);	
#endif
		}

		break;
		default:
			printk("ioctl:error\n");
			return -EINVAL;
	}
ioctl_get_label:
	//wade: note set is a pointer here
	if ( copy_to_user((struct dio_set_struct *)arg, &set, sizeof(struct dio_set_struct)) )
		return -EFAULT;

	return 0;
}
/*
 * polling_function(): polling DI state and counter value
 *   
 */
#ifdef __TIMER_VERSION__ // timer
// polling timer function
void polling_function(unsigned long data)
{
	epm3438_struct *pEpm3438 = (epm3438_struct *)data;

	spin_lock(&pEpm3438->di_lock);

	pEpm3438->di_state_keep = moxainb(pEpm3438->base+2);
//printk("%s[%d]pEpm3438->di_state_keep:%x\n",__FUNCTION__, __LINE__, pEpm3438->di_state_keep);
	// Compare the preceding and current DI value
	if ( pEpm3438->di_state_keep ^ pEpm3438->pre_di_state ) {
//printk("%s[%d]\n",__FUNCTION__, __LINE__);
		pEpm3438->pre_di_state = pEpm3438->di_state_keep;
		pEpm3438->di_toggled_flag = 1;
		wake_up_interruptible( &pEpm3438->poll_wq );
	}

	spin_unlock(&pEpm3438->di_lock);
	
	// Compare the preceding and current counter value
	if ( pEpm3438->counter_keep ^ moxainw(pEpm3438->base+4) ) {
//printk("%s[%d]\n",__FUNCTION__, __LINE__);
		wake_up_interruptible( &pEpm3438->poll_wq );
	}

	// Schedule next polling
	pEpm3438->polling.expires=jiffies+ TIMER_INTERVAL;
	add_timer(&pEpm3438->polling);
}

unsigned int epm3438_poll (struct file *fp, struct poll_table_struct *ptable)
{
	epm3438_struct *pEpm3438 = (epm3438_struct *)fp->private_data;
	unsigned int    devno = iminor(fp->f_path.dentry->d_inode);

//printk("%s[%d]\n",__FUNCTION__, __LINE__);
	poll_wait(fp, &pEpm3438->poll_wq, ptable);

	if ( devno == 0 ) { // Device 0: DIO changed

		spin_lock_irq(&pEpm3438->di_lock);

		// Compare the preceding and current DI value
		if ( pEpm3438->di_toggled_flag ) {
			spin_unlock_irq(&pEpm3438->di_lock);
//printk("%s[%d]pEpm3438->di_state_keep:%x, pEpm3438->pre_di_state:%x, mask:%x\n",__FUNCTION__, __LINE__, pEpm3438->di_state_keep, pEpm3438->pre_di_state);
			return (POLLIN | POLLRDNORM);
		}

		spin_unlock_irq(&pEpm3438->di_lock);
	}
	else if ( devno == 1 ) { // Device 1: counter changed
			// Compare the preceding and current counter value
			if ( pEpm3438->counter_keep ^ moxainw(pEpm3438->base+4) ) {
//printk("%s[%d]pEpm3438->counter_keep:%x, %x\n",__FUNCTION__, __LINE__,pEpm3438->counter_keep, moxainb(pEpm3438->base+4));
				return (POLLIN | POLLRDNORM);
			}
	}
	else { // No such device
		return -ENODEV;
	}

//printk("%s[%d]\n",__FUNCTION__, __LINE__);
	return 0;
}

#endif

int epm3438_release (struct inode *inode, struct file *fp)
{
#ifdef __TIMER_VERSION__ // timer
	epm3438_struct *pEpm3438 = (epm3438_struct *)fp->private_data;

	if ( timer_pending(&pEpm3438->polling) ) {
		del_timer(&pEpm3438->polling);
	}
	pEpm3438->polling_on = 0;
#endif

	return 0;
}

int epm3438_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int retval;
	dev_t devno=0;
	epm3438_struct *pEpm3438;
	static int brd_idx=-1, init_once=0;

	brd_idx++;

	if ( brd_idx==0 ) // board #0
		pEpm3438 = &Epm3438[0];
	else	// board #1
		pEpm3438 = &Epm3438[1];

	printk(KERN_DEBUG"Found device(board index:%d) %x:%x\n", brd_idx, pdev->vendor, pdev->device);
	pci_set_drvdata(pdev, pEpm3438);

	retval = pci_enable_device(pdev);
	if ( retval ) {
		printk(KERN_ERR"Moxa %s module PCI enable fail !\n", MODEL_NAME);
		goto cleanup_probe0;
	}

	pEpm3438->base = pci_resource_start(pdev, 4);
	pEpm3438->size = pci_resource_len(pdev, 4);
	//printk("io:%x; len:%x!\n", pEpm3438->base, pEpm3438->size);

	if ( !request_region(pEpm3438->base, pEpm3438->size, MODEL_NAME) ) {
		printk("Moxa %s request_region fail !\n", MODEL_NAME);
		retval=-ENOMEM;
		goto cleanup_probe1;
	}

	// No IRQ provided of this hardware
	/*
	pEpm3438->irq = pdev->irq;
	printk("irq:%x !\n", pEpm3438->irq);
	*/

	/* Begin --- Initialize hardware status */
	// Reset the Counter
	moxaoutw(0, pEpm3438->base+4);
	//pEpm3438->counter_keep = moxainw(pEpm3438->base+4);

	// Set the DO Initial state
	if ( epm3438_DO2LOW == 1 ) { // Set DO initial value to LOW
		pEpm3438->do_state_keep = 0x0;
		moxaoutb(pEpm3438->do_state_keep, pEpm3438->base+1 ); // DO: base+1
	}
	else { // Jared, 07-21-2010, Keep the original DO value
		pEpm3438->do_state_keep=moxainb(pEpm3438->base+1);
	}
	
	// Read the DI Initial state
	pEpm3438->di_state_keep = moxainb(pEpm3438->base+2);

	#ifdef __DEBUG__
	printk(KERN_DEBUG"%s[%d]:do_state_keep:%x\n", __FUNCTION__, __LINE__,pEpm3438->do_state_keep);	
	printk(KERN_DEBUG"%s[%d]:di_state_keep:%x\n", __FUNCTION__, __LINE__,pEpm3438->di_state_keep);	
	#endif
#ifdef __TIMER_VERSION__
	pEpm3438->pre_di_state = pEpm3438->di_state_keep;
#endif
	/* End --- Initialize hardware status */

	if ( init_once==0 ) { // Allocate for two boards
		retval = alloc_chrdev_region(&devno, 0, MOXA_EPM3438_DEVICE_NUM * 2, MODEL_NAME);
		if ( retval < 0 ) {
			printk ("alloc_chrdev_region() fail\n");
			goto cleanup_probe2;
        	}
	        Major=MAJOR(devno);

		cdev_init(&epm_3438_cdev, &moxa_epm3438_fops);
		epm_3438_cdev.owner=THIS_MODULE;
		retval = cdev_add(&epm_3438_cdev, MKDEV(Major,0), MOXA_EPM3438_DEVICE_NUM * 2);
		if( retval < 0 ) {
			printk ("Error adding cdev\n");
			goto cleanup_probe3;
		}

		charmodule_class=class_create(THIS_MODULE, MODEL_NAME); 

		if(IS_ERR(charmodule_class)) {
			printk("Err: failed in creating class.\n");
			retval=-ENOMEM;
			goto cleanup_probe4;
		}

		init_once=1; // set the flag to run once
	}

	if ( brd_idx==0 ) { // Create device file for board #0
		device_create(charmodule_class, NULL, MKDEV(Major, 0), NULL, EPM_3438_DIO_1); 
		device_create(charmodule_class, NULL, MKDEV(Major, 1), NULL, EPM_3438_COUNTER_1); 
	}
	else {	// Create device file for board #1
		device_create(charmodule_class, NULL, MKDEV(Major, 2), NULL, EPM_3438_DIO_2); 
		device_create(charmodule_class, NULL, MKDEV(Major, 3), NULL, EPM_3438_COUNTER_2); 
	}

#ifdef __TIMER_VERSION__	// Use kernel timer
	pEpm3438->polling_on = 0;
	// Clear the flag
	pEpm3438->di_toggled_flag=0;
	// Initialize the spinlock
	spin_lock_init(&pEpm3438->di_lock);
#endif

	return 0;

cleanup_probe4:
	cdev_del(&epm_3438_cdev);
cleanup_probe3:
	unregister_chrdev_region(MKDEV(Major, 0), MOXA_EPM3438_DEVICE_NUM * (brd_idx+1) );
cleanup_probe2:
	release_region(pEpm3438->base, pEpm3438->size);
cleanup_probe1:
	pci_disable_device(pdev);
cleanup_probe0:

	return retval;
}

void epm3438_remove(struct pci_dev *pdev)
{
	static int remove_once = 0;
	epm3438_struct *pEpm3438;

	printk(KERN_DEBUG"Remove device %x:%x\n", pdev->vendor, pdev->device);
	pEpm3438 = (epm3438_struct*)pci_get_drvdata(pdev);

	// delete device node under /dev
	if ( remove_once==0 ) { // board #0
		device_destroy(charmodule_class, MKDEV(Major, 0)); 
		device_destroy(charmodule_class, MKDEV(Major, 1)); 
		device_destroy(charmodule_class, MKDEV(Major, 2)); 
		device_destroy(charmodule_class, MKDEV(Major, 3)); 

		// delete class created by us
		class_destroy(charmodule_class);

		cdev_del(&epm_3438_cdev);

		unregister_chrdev_region(MKDEV(Major, 0), MOXA_EPM3438_DEVICE_NUM*2);
		remove_once=1;
	}

	release_region(pEpm3438->base, pEpm3438->size);
	pci_disable_device(pdev);
}

static int __init epm3438_init(void)
{
	int retval;

	printk(KERN_DEBUG"%s Driver version %s\n", MODEL_NAME, DRIVER_VERSION);

	sema_init(&Epm3438[0].counter_sem, 1);
	sema_init(&Epm3438[1].counter_sem, 1);

	retval = pci_register_driver(&epm3438_driver);
	if (retval) {
		printk(KERN_ERR "Can't register pci driver\n");
		return (-ENODEV);
	}

	return 0;
}

void __exit epm3438_exit(void)
{
	printk(KERN_DEBUG"%s removed\n", MODEL_NAME);
	pci_unregister_driver(&epm3438_driver);
}

module_init(epm3438_init);
module_exit(epm3438_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jared_wu@moxa.com");
MODULE_DESCRIPTION("EPM-3438: DIO/Counter module");
MODULE_SUPPORTED_DEVICE(EPM_3438_DIO);
MODULE_SUPPORTED_DEVICE(EPM_3438_COUNTER);


module_param(epm3438_DO2LOW, int, 0644);
MODULE_PARM_DESC(epm3438_DO2LOW, "Reset DO to LOW. 0. Set DO to High (default). 1. Set DO to LOW.");

#ifdef __DEBUG__
module_param(epm3438_debug, int, 0644);
MODULE_PARM_DESC(epm3438_debug, "Enable/disable debug information. Default epm3438_debug=0");
#endif
