
/*
 * To set serial port interface.
 * Usage: setinterface device-node [interface-no]
 * 	  device-node	- must be /dev/ttyM0 or /dev/ttyM1
 * 	  Interface-no - following
 * 	  none - to view now setting
 * 	  0 - RS232
 * 	  1 - RS485-2WIRES
 * 	  2 - RS422
 * 	  3 - RS485-4WIRES
 *
 * History:
 * Date		Author			Comment
 * 03-28-2005	Victor Yu.		Create it.
 * 06-15-2009	Johnson Liu.		modify for UART of x86 base.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include 	<string.h>
#include	<fcntl.h>
#include	<sys/ioctl.h>
#include <errno.h>

#define MOXA                    0x400
//#define MOXA_SET_OP_MODE        (MOXA + 100)
//#define MOXA_GET_OP_MODE        (MOXA + 101)
#define MOXA_SET_OP_MODE        (MOXA + 66)
#define MOXA_GET_OP_MODE        (MOXA + 67)
#define RS232_MODE              0
#define RS485_2WIRE_MODE        1
#define RS422_MODE              2
#define RS485_4WIRE_MODE        3
#define NOT_SET_MODE		4

int	main(int argc, char *argv[])
{
	int	fd;
	unsigned char port, interface;

	if ( argc <= 1 || strncmp("/dev/ttyS",argv[1],9) || strlen(argv[1])<10 || !isdigit(*(argv[1]+9))) {
usage:
		printf("Usage: %s device-node [interface-no]\n", argv[0]);
		printf("	device-node	- /dev/ttyS0 ~ /dev/ttyS1\n");
		printf("	interface-no 	- following:\n");
		printf("	0 - set to RS232 interface\n");
		printf("	1 - set to RS485-2WIRES interface\n");
		printf("	2 - set to RS422 interface\n");
		printf("	3 - set to RS485-4WIRES interface\n");
		exit(0);
	}

	// to open the device node
	fd = open("/dev/uart", O_RDWR);
	if ( fd < 0 ) {
		printf("Open device /dev/uart fail !\n");
		exit(1);
	}



	if ( argc <= 2 ) {
		interface=(unsigned char)atoi(argv[1]+9);
		if(ioctl(fd, MOXA_GET_OP_MODE, &interface)){
			//printf("get op mode fail\n");
			goto usage;
		}
		switch ( interface ) {
		case RS232_MODE :
			printf("Now setting is RS232 interface.\n");
			break;
		case RS485_2WIRE_MODE :
			printf("Now setting is RS485-2WIRES interface.\n");
			break;
		case RS422_MODE :
			printf("Now setting is RS422 interface.\n");
			break;
		case RS485_4WIRE_MODE :
			printf("Now setting is RS485-4WIRES interface.\n");
			break;
		case NOT_SET_MODE :
			printf("Now does not set to any type interface.\n");
			break;
		default :
			printf("Unknow interface is set.\n");
		}
		exit(0);
	}


	port = (unsigned char)atoi(argv[1]+9);
	interface = (port<<4)|(unsigned char)atoi(argv[2]);
	

	if(ioctl(fd, MOXA_SET_OP_MODE, &interface)){
		goto usage;
	}
	exit(0);
}
