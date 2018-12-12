/*  Copyright (C) MOXA Inc. All rights reserved.
 *
 *      This software is distributed under the terms of the
 *          MOXA License.  See the file COPYING-MOXA for details.
 *
 *     File name: moxa_uart8250_mode.c
 *
 *
 *	  Device file:
 *	  	mknod /dev/epm_3438_dio c $major 0
 *
 *	v1.0	08/13/2014 Jared Wu	New release for DA-820-LX 8250 interface setting
 *	v1.1	05/14/2015 Jared Wu	New release for MC-7200-DPP 8250 interface setting
 *	v1.2	09/05/2017 Holsety Chen	New release for DA-720/682B-LX 8250 interface setting
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/pci.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include "moxa_superio.h"
#include "moxa_uart8250_mode.h"

#define DRIVER_VERSION		"1.2"
#define MODEL_NAME		"moxa_uart8250_mode"

/* portNum
 *    For DA-820, 0: port1, 1: port2.
 *    For MC-7200, 2: port3, 3: port4.
 */
s32 EnableAutoDirectionControl(int portNum)
{
	u8 reg_val = 0x0;
	u8 ucAutoDirectionControl = 0x0;

	/* Get current register data from hardware*/
#if (defined DA820) || (defined EXPC1519) || (defined DA720)
	reg_val = superio_read(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+1);
#elif (defined MC7200) /* Logic device 8 for COM3, Logic device 9 for COM4 */
	reg_val = superio_read(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+6);
#endif

	/* Enable auto direction control */
	ucAutoDirectionControl = reg_val | 0x80;
#if (defined DA820) || (defined EXPC1519) || (defined DA720)
	superio_write(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+1, (u8)ucAutoDirectionControl );
#elif (defined MC7200) /* Logic device 8 for COM3, Logic device 9 for COM4 */
	superio_write(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+6, (u8)ucAutoDirectionControl );
#endif

	return 0;
}

/* portNum
 *    For DA-820, 0: port1, 1: port2.
 *    For MC-7200, 2: port3, 3: port4.
 */
s32 DisableAutoDirectionControl(int portNum)
{
	u8 reg_val = 0x0;
	u8 ucAutoDirectionControl = 0x0;

	/* Get current register data from hardware*/
#if (defined DA820) || (defined EXPC1519) || (defined DA720)
	reg_val = superio_read(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+1);
#elif (defined MC7200) /* Logic device 8 for COM3, Logic device 9 for COM4 */
	reg_val = superio_read(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+6);
#endif

	/* Disable auto direction control */
	ucAutoDirectionControl = reg_val & ~0x80;
#if (defined DA820) || (defined EXPC1519) || (defined DA720)
	superio_write(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+1, (u8)ucAutoDirectionControl );
#elif (defined MC7200) /* Logic device 8 for COM3, Logic device 9 for COM4 */
	superio_write(LOGIC_DEVICE_NUMBER, UART_AUTODIRECTION_CONTROL_ADDRESS, portNum+6, (u8)ucAutoDirectionControl );
#endif

	return 0;
}


int moxa_uart8250_mode_open (struct inode *inode, struct file *fp)
{
	return 0;
}

ssize_t moxa_uart8250_mode_read(struct file *fp, char *buffer, size_t length, loff_t *offset) 
{
	return 0;
}

long moxa_uart8250_mode_ioctl (struct file *fp, unsigned int cmd, unsigned long arg)
{
	port_type_t uart_port;
	s32 uartModeBit[][3] = {
/* Jared, DA-820/DA-720/EXPC-1519/MC7200 serial ports 1, 2 support RS-232/422/485 mode switching */
#if (defined DA820) || (defined EXPC1519) || (defined DA720)
		{UART_MODE_BIT_COM1_RS232, UART_MODE_BIT_COM1_RS485, UART_MODE_BIT_COM1_RS422},
		{UART_MODE_BIT_COM2_RS232, UART_MODE_BIT_COM2_RS485, UART_MODE_BIT_COM2_RS422},
#elif (defined MC7200)	/* Jared, MC-7200 serial ports 3, 4 support RS-232/422/485 mode switching */
		{0, 0, 0},
		{0, 0, 0},
		{UART_MODE_BIT_COM3_RS232, UART_MODE_BIT_COM3_RS485, UART_MODE_BIT_COM3_RS422},
		{UART_MODE_BIT_COM4_RS232, UART_MODE_BIT_COM4_RS485, UART_MODE_BIT_COM4_RS422},
#elif (defined MC1100)	/* Jared, MC-1100 serial ports 1, 2 support RS-232/422/485 mode switching */
		{UART_MODE_BIT_COM1_RS232, UART_MODE_BIT_COM1_RS485, UART_MODE_BIT_COM1_RS422},
		{UART_MODE_BIT_COM2_RS232, UART_MODE_BIT_COM2_RS485, UART_MODE_BIT_COM2_RS422},
#endif
	};
	s32 i;
	u8 reg_val;

	switch ( cmd ) {
		case IOCTL_GET_8250_MODE:

        		if ( copy_from_user(&uart_port, (void*)arg , sizeof(port_type_t)) ) {
				return -EFAULT;
        		}

			reg_val = inb(UART_MODE_IO_ADDRESS);

			/* Check port mode with register */
			for(i = 0; i< 3; i++) {
				if( reg_val & (1<<uartModeBit[uart_port.port_number][i]) ) {
					uart_port.port_type = i;
					break;
				}
			}

                	if ( copy_to_user((void *)arg, &uart_port, sizeof(port_type_t)) )
				return -EFAULT;

			break;

		case IOCTL_SET_8250_MODE:

        		if ( copy_from_user(&uart_port, (void*)arg , sizeof(port_type_t)) ) {
				return -EFAULT;
        		}

			/* Keep the original uart interface type setting */
			reg_val = inb(UART_MODE_IO_ADDRESS);

			/*
			 * Clear the bitmap of the UART mode register
			 *
			 * bit 0: UART port 0 RS-232 mode
			 * bit 1: UART port 0 RS-485-2W mode
			 * bit 2: UART port 0 RS-422/485-4W mode
			 * bit 3: reserved
			 * bit 4: UART port 1 RS-232 mode
			 * bit 5: UART port 1 RS-485-2W mode
			 * bit 6: UART port 1 RS-422/485-4W mode
			 * bit 7: reserved
			 */
			switch ( uart_port.port_number ) {
#if (defined DA820) || (defined EXPC1519) || (defined DA720) 	/* Jared, DA-820/EXPC-1519 serial ports 1, 2 support RS-232/422/485 mode switching */
				case 0:
					reg_val &= ~(UART0_MASK);
					break;
				case 1:
					reg_val &= ~(UART1_MASK);
					break;
#elif (defined MC7200)	/* Jared, MC-7200 serial ports 3, 4 support RS-232/422/485 mode switching */
				case 2:
					reg_val &= ~(UART2_MASK);
					break;
				case 3:
					reg_val &= ~(UART3_MASK);
					break;
#endif
			}

			/* Set the interface type */
			reg_val |= ( 1 << uartModeBit[uart_port.port_number][uart_port.port_type] );

			/* Apply the setting */
			outb(reg_val, UART_MODE_IO_ADDRESS);

			/* Set auto direction control */
			if( uart_port.port_type != 1 ) {	// We are in 232/422 mode
				DisableAutoDirectionControl(uart_port.port_number);
			}
			else {	// Only in 485 mode need Auto Directoion control
				EnableAutoDirectionControl(uart_port.port_number);
			}
			break;

		default:
			printk("ioctl:error\n");
			return -EINVAL;
	}
	return 0;
}

int moxa_uart8250_mode_release (struct inode *inode, struct file *fp)
{
	return 0;
}

static struct file_operations moxa_uart8250_mode_fops = {
	.owner = THIS_MODULE,
	.open = moxa_uart8250_mode_open,
	.unlocked_ioctl = moxa_uart8250_mode_ioctl,
	.release = moxa_uart8250_mode_release,
};

static struct miscdevice moxa_uart8250_mode = {
	.minor = MX_MODE_8250_MINOR,
	.name = MODEL_NAME,
	.fops = &moxa_uart8250_mode_fops
};

static int __init moxa_uart8250_mode_init(void)
{
	int retval=0;
	struct resource *base_res;

	printk(KERN_INFO"%s Driver version %s\n", MODEL_NAME, DRIVER_VERSION);

	retval = misc_register(&moxa_uart8250_mode);
	if ( retval < 0 ) {
		printk(KERN_ERR "Can't register misc driver\n");
		retval=-ENODEV;
		goto moxa_uart8250_mode_init_fail_0;
	}

	base_res = request_region(UART_MODE_IO_ADDRESS, 0x01, MODEL_NAME);
	if ( !base_res ) {
		printk(KERN_ERR "Can't request_region, %x\n", UART_MODE_IO_ADDRESS);
		retval=-ENOMEM;
		goto moxa_uart8250_mode_init_fail_1;
	}

	return 0;

moxa_uart8250_mode_init_fail_1:
	misc_deregister(&moxa_uart8250_mode);
moxa_uart8250_mode_init_fail_0:
	return retval;
}

void __exit moxa_uart8250_mode_exit(void)
{
	printk(KERN_INFO"Remove Driver %s from kernel\n", MODEL_NAME);

	release_region(UART_MODE_IO_ADDRESS, 0x01);
	misc_deregister(&moxa_uart8250_mode);
}

module_init(moxa_uart8250_mode_init);
module_exit(moxa_uart8250_mode_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jared.wu@moxa.com");
MODULE_DESCRIPTION("Moxa set RS-232/422/485 interface module");

