#include "dio_main.h" 

#define DRV_VERSION "v1.2"

/* 
 * the order for this driver is [do di]
 */

/* 
 * Global table 
 * ------------
 * Field: name, do_state, addr, di_num, do_num, gpio_method(superio/gpio), model_select 
 * Note: if you use device group, register your model_select funtion here and implement
 * in your_device_name.c
 */

static dio_dev* mdev;
static char* device=CONFIG_PRODUCT;	/* V2416A, V2616A, DA-820, MC-7130, ... */

OUTB_CALLBACK dio_outb;
INB_CALLBACK dio_inb;
GET_DOUT_CALLBACK get_dout;

int register_dio_inb( INB_CALLBACK inb_fun ) {

	dio_inb = inb_fun;

	return 0;
}

int register_dio_outb( OUTB_CALLBACK outb_fun ) {

	dio_outb = outb_fun;

	return 0;
}

int register_get_dout( GET_DOUT_CALLBACK get_dout_fun) {

	get_dout = get_dout_fun;

	return 0;
}

/* 
 * Open/release
 */

static int io_open (struct inode *inode, struct file *file) 
{
	dp("io_open\n");
	return 0;
}

static int io_release (struct inode *inode, struct file *file) 
{
	dp("io_release\n");
	return 0;
}


/* 
 *  Ioctl - I/O control 
 */
static long io_ioctl(struct file *file,unsigned int cmd, unsigned long arg) 
{
	struct dio_set_struct   set;
	u8 di_state = 0;

	/* check data */
	if (copy_from_user(&set, (struct dio_set_struct *)arg, sizeof(struct dio_set_struct)))
		return -EFAULT;

	switch (cmd) {
		case IOCTL_SET_DOUT : 
			dp("set dio:%x\n",set.io_number);
			if (set.io_number < 0 || set.io_number >= mdev->do_num)
				return -EINVAL;

			if ( set.mode_data != DIO_HIGH && set.mode_data != DIO_LOW )
				return -EINVAL;
			
			/* set do */
			dio_outb(mdev, &set);

			break;

		case IOCTL_GET_DOUT : 
			dp("get dout:%x\n",set.io_number);
			if (set.io_number == ALL_PORT) { 
					set.mode_data = mdev->do_state;
			} else {
				if (set.io_number < 0 || set.io_number >= mdev->do_num)
                                        return -EINVAL;

				/* get the DOUT state */
				get_dout(mdev, &set);
			}

			if (copy_to_user((struct dio_set_struct *)arg, &set, sizeof(struct dio_set_struct)))
				return -EFAULT;
			dp("mode_data: %x\n",(unsigned int)set.mode_data);
			break;

		case IOCTL_GET_DIN :
			dp("get din,%x, mdev->do_num:%x\n",set.io_number, mdev->do_num);

			di_state = dio_inb(mdev, &set);
			dp("di_state,%x\n", di_state);

			if (set.io_number == ALL_PORT) {
				set.mode_data = di_state; 
				dp("all port: %x", set.mode_data);
			} else { 
				if (set.io_number < 0 || set.io_number >= mdev->di_num) 
					return -EINVAL;
				//if (di_state & (1 << set.io_number))
				if (di_state & 1)
					set.mode_data = 1;
				else
					set.mode_data = 0;
			}

			if (copy_to_user((struct dio_set_struct *)arg, &set, sizeof(struct dio_set_struct)))
				return -EFAULT;
			dp("mode_data: %x\n",(unsigned int)set.mode_data);
			break;

		default :
			printk("ioctl:error\n");
			return -EINVAL;
	}

	return 0; /* if switch ok */
}

static struct file_operations dio_fops = {
	.owner	=	THIS_MODULE,
	.unlocked_ioctl	=	io_ioctl,
	.open	=	io_open,
	.release=	io_release,
};

static struct miscdevice dio_miscdev = {
	.minor = MOXA_DIO_MINOR,
	.name = "dio",
	.fops = &dio_fops,
};

static int __init io_init_module (void) 
{
	int result;
	printk("initializing MOXA dio module %s\n", DRV_VERSION);

	/* check  module paramter, device = model_name */

	/* initialize mdev */
	mdev = moxa_platform_dio_init();
	if (mdev == NULL) {
		printk("moxa_device_dio: can't support your device, wrong device parameter?\n");
		return -ENODEV;
	}

	/* register misc driver */
	result = misc_register(&dio_miscdev);
	if (result) {
		printk("Moxa dio driver: Register misc fail !\n");
		return result;
	}

	/* Don't setup the initilize value */
	//dio_init();

	return 0;
}

/*
 * close and cleanup module
 */
static void __exit io_cleanup_module (void) 
{
	printk("cleaning up module\n");
	misc_deregister(&dio_miscdev);
}

module_init(io_init_module);
module_exit(io_cleanup_module);
module_param(device, charp, S_IRUGO);
MODULE_AUTHOR("Wade_liang@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA DIO module");

