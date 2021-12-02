
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
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<sys/ioctl.h>
#include 	<linux/serial.h>

#define RS232_MODE              0
#define RS485_2WIRE_MODE        1
#define RS422_MODE              2
#define RS485_4WIRE_MODE        3
#define NOT_SET_MODE		4

void get_serial(char *device, int *mode)
{
        struct serial_struct serinfo;
        int     fd;

        if ((fd = open(device, O_RDWR|O_NONBLOCK)) < 0) {
                perror(device);
                return;
        }
        serinfo.reserved_char[0] = 0;

        if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0) {
                perror("Cannot get serial info");
                close(fd);
                return;
        }

        if (serinfo.irq == 9)
                serinfo.irq = 2;        /* People understand 2 better than 9 */
//	printf("%s mode 0x%.4x\n", device, serinfo.port);
	*mode = serinfo.port;
        close(fd);
}

void set_serial(char *device, int mode)
{
        struct serial_struct old_serinfo, new_serinfo;
        int     fd;

        if ((fd = open(device, O_RDWR|O_NONBLOCK)) < 0) {
                perror(device);
                exit(201);
        }
        if (ioctl(fd, TIOCGSERIAL, &old_serinfo) < 0) {
                perror("Cannot get serial info");
                exit(1);
        }
        new_serinfo = old_serinfo;
        new_serinfo.port = mode;
        if (ioctl(fd, TIOCSSERIAL, &new_serinfo) < 0) {
                perror("Cannot set serial info");
                exit(1);
        }
        close(fd);
}

void usage()
{
	printf("Usage: setinterface device-node [interface-no]\n");
	printf("	device-node	- /dev/ttyM0 ~ /dev/ttyM3\n");
	printf("	interface-no 	- following:\n");
	printf("	none - to view now setting\n");
	printf("	0 - set to RS232 interface\n");
	printf("	1 - set to RS485-2WIRES interface\n");
	printf("	2 - set to RS422 interface\n");
	printf("	3 - set to RS485-4WIRES interface\n");
	exit(0);
}

int	main(int argc, char *argv[])
{
	int	fd, interface;

	if ( argc <= 1 ) {
		usage();
	}

	// to open the device node
	fd = open(argv[1], O_RDWR);
	if ( fd < 0 ) {
		printf("Open device node %s fail !\n", argv[1]);
		usage();
	}

	if ( argc <= 2 ) {
		get_serial(argv[1], &interface);
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
		close(fd);
		exit(0);
	}

	sscanf(argv[2], "%d", &interface);
	if ( interface != RS232_MODE &&
	     interface != RS485_2WIRE_MODE &&
	     interface != RS422_MODE &&
	     interface != RS485_4WIRE_MODE ) {
		close(fd);
		printf("Error interface.\n");
		usage();
	}

	set_serial(argv[1], interface);
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
	close(fd);
	exit(0);
}
