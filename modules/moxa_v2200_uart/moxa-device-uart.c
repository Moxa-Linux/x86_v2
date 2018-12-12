/*
 * This driver is for Moxa embedded computer V2201 RS-232/422/485 mode control.
 *
 * History:
 * 07-19-2018	Harry YJ Jhou.	V2201 UART RS-232/422/485 mode control.
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
#include "moxa_superio.h" // "x86_superio.h"

#define MOXA_UART_MINOR 112
#define BASEPORT 0x300
#define DRV_VERSION	"v1.1"

#ifdef DEBUG
#define dbg_printf(x...)        printk(x)
#else
#define dbg_printf(x...)
#endif

//
// DIO file operaiton function call
//
#define MOXA                    0x400
#define MOXA_SET_OP_MODE      (MOXA + 66)
#define MOXA_GET_OP_MODE      (MOXA + 67)

#define RS232_MODE              0
#define RS485_2WIRE_MODE        1
#define RS422_MODE              2
#define RS485_4WIRE_MODE        3

struct dio_set_struct {
        int     io_number;
        int     mode_data;      // 1 for input, 0 for output, 1 for high, 0 for low
};

static int io_open (struct inode *inode, struct file *file) {
	dbg_printf("io_open\n");
	return 0;
}

static int io_release (struct inode *inode, struct file *file) {
	dbg_printf("io_release\n");
	return 0;
}

unsigned char keep_opmode=0x00;

// ioctl - I/O control
static long io_ioctl( struct file *file,unsigned int cmd, unsigned long arg) {
	unsigned char port, opmode, val;
	int retval = 0;

	dbg_printf("get op mode\n");
	
	switch ( cmd ) {
		case MOXA_SET_OP_MODE:
			retval = copy_from_user(&opmode,(unsigned char *)arg,sizeof(unsigned char));
			if ( retval < 0 ) {
				printk(KERN_ERR "copy_from_user() fail\n");
				return retval;
			}

			port = opmode >> 4 ;
			opmode = opmode & 0xf;

			if ( opmode != RS232_MODE \
				&& opmode != RS485_2WIRE_MODE \
				&& opmode != RS422_MODE \
				&& opmode != RS485_4WIRE_MODE && port > 1)
				return -EFAULT;

			val = inb(BASEPORT+4) & (~(((unsigned char)0xe)<<(4*port)));
			dbg_printf("port:%x, opmode:%x, val:%x\n", port, opmode, val);

			switch ( opmode ) {
				case RS232_MODE:
					val |= (((unsigned char)0x8)<<(4*port));
					break;			
				case RS485_2WIRE_MODE:
					val |= (((unsigned char)0x2)<<(4*port));
					break;
				case RS422_MODE:
				case RS485_4WIRE_MODE:
					val |= (((unsigned char)0x4)<<(4*port));
					break;
			}

			/* Apply the setting */
			dbg_printf("Write port:%x, val:%x\n", BASEPORT+4, val);
			outb(val, BASEPORT+4);

			keep_opmode &= ~(((unsigned char)0xf)<<(port*4));
			keep_opmode |= opmode<<(port*4);

#if 1	/* Set auto direction control */
			val = superio_read(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, port+1);
			if( opmode != RS485_2WIRE_MODE ) { /* Disable auto-direction in 232/422 mode */
				superio_write(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, port+1, (u8)val & (~0x80) );
			}
			else {	/* Only in 485 mode need Auto Directoion control */
				superio_write(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, port+1, (u8)val | 0x80 );
			}
#else
			superio_enter_config();
			superio_set_logic_device((u8)(port+1));
			if(opmode == RS232_MODE){
				val=superio_get_reg(0xf0)& 0x7f;
			}else{
				val=superio_get_reg(0xf0)| 0x80;
			}
			superio_set_reg(val , 0xf0);
#endif

			break;

		case MOXA_GET_OP_MODE:
			dbg_printf("get op mode\n");
			retval = copy_from_user(&port,(unsigned char *)arg,sizeof(unsigned char));
			if ( retval < 0 ) {
				printk(KERN_ERR "copy_from_user() fail\n");
				return retval;
			}

			if( port > 1 ) {
				printk(KERN_ERR "port value: %d is invalid\n", port);
				return -EINVAL;
			}

			opmode=(keep_opmode>>(port*4))& 0xf;

			retval = copy_to_user((unsigned char*)arg, &opmode, sizeof(unsigned char));
			if ( retval < 0 ) {
				printk(KERN_ERR "copy_to_user() fail\n");
				return retval;
			}

			break;
		default:
			printk(KERN_ERR "Unknown ioctl command: %x\n", cmd);
			return -EINVAL;
	}

	return 0;
}


// define which file operations are supported
struct file_operations io_fops = {
	.owner	=	THIS_MODULE,
	.unlocked_ioctl	=   io_ioctl,
	.open		=   io_open,
	.release	=   io_release,
};

// register as misc driver
static struct miscdevice uart_miscdev = {
    .minor = MOXA_UART_MINOR,
    .name = "uart",
    .fops = &io_fops,
};


// initialize module (and interrupt)
static int __init io_init_module (void) {
	
	int error;

	printk(KERN_INFO "Initializing MOXA V2201 UART mode control module, %s\n", DRV_VERSION);
	error = misc_register(&uart_miscdev);	
	if ( error ) {
        	printk("Moxa uart driver: Register misc fail %d!\n", error);
        	return error;
	}

	return 0;
}

// close and cleanup module
static void __exit io_cleanup_module (void) {
	printk(KERN_INFO "Unregister MOXA V2201 UART mode control module, %s\n", DRV_VERSION);
	misc_deregister(&uart_miscdev);
}

module_init(io_init_module);
module_exit(io_cleanup_module);
MODULE_AUTHOR("HarryYJ.Jhou@moxa.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MOXA UART module");

