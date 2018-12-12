/*
 * To set the serial port mode.
 * Usage: setuartmode -n port_number -s mode
 * 	  port_number	- should be 1, 2
 *        mode		- should be 0, 1, 2
 *            0 - RS232
 *            1 - RS485-2WIRES
 *            2 - RS422/RS485-4WIRES
 *
 * To get the serial port mode.
 * Usage: setuartmode -n port_number -g
 * 	  port_number	- should be 1, 2
 *
 * History:
 * Date		Author			Comment
 * 08-15-2014	Jared Wu.		Create it.
 * 05-19-2015	Jared Wu.		Update for control the MC-7200 serials port3, port4 as RS-232, RS-422/485-2w or RS-485-4w.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<sys/ioctl.h>


#define MX_MODE_8250_MINOR  112

#define IOCTL_GET_8250_MODE _IOR(MX_MODE_8250_MINOR,1,int)
#define IOCTL_SET_8250_MODE _IOW(MX_MODE_8250_MINOR,2,int)

#ifdef MC7200
#define DEFAULT_PORT_NUMBER	3 /* MC-7270-DC-CP default port number is 3 */
#else
#define DEFAULT_PORT_NUMBER	1 /* Default port number is 1 */
#endif

typedef struct __port_type_t {
	int port_number;	/* port number should be 0, 1 */
	int port_type;		/* port type: 0 - RS-232, 1 - RS-485-2W, 2 - RS-422 */
} port_type_t;

char *uart_interface_mode[]={"RS-232", "RS-485-2W", "RS-422/RS-485-4W"};

void usage(char *name) {
	printf("Get/set the serial port mode utility\n");
	printf("Usage: %s -n [the nth port] [-g|-s] [-h]\n", name);
	printf("    Show the usage information if no argument apply.\n");
	printf("        -h: Show this information.\n");
	printf("        -n [the nth port]: Indicate the n-th serial port.\n");
	printf("            1 for the first serial port (default)\n");
	printf("            2 for the second serial port.\n");
	printf("        -g: Get the n-th serial port interface type.\n");
	printf("        -s: Set the n-th serial port interface.\n");
	printf("            0 for RS-232.\n");
	printf("            1 for RS-485-2W.\n");
	printf("            2 for RS-422/RS-485-4W.\n");
	printf("Example:\n");
	printf("    To set the first serial port as RS-232 mode\n");
	printf("    root@Moxa:~# setuartmode -n 1 -s 0\n");
	printf("    To set the first serial port as RS-485-2W mode\n");
	printf("    root@Moxa:~# setuartmode -n 1 -s 1\n");
	printf("    To set the first serial port as RS-422/485-4W mode\n");
	printf("    root@Moxa:~# setuartmode -n 1 -s 2\n");
	printf("    To get the first serial port mode.\n");
	printf("    root@Moxa:~# setuartmode  -n 1 -g\n");
	printf("    To set the second serial port as RS-232 mode\n");
	printf("    root@Moxa:~# setuartmode -n 2 -s 0\n");
	printf("    To set the second serial port as RS-485-2W mode\n");
	printf("    root@Moxa:~# setuartmode -n 2 -s 1\n");
	printf("    To set the second serial port as RS-422/485-4W mode\n");
	printf("    root@Moxa:~# setuartmode -n 2 -s 2\n");
	printf("    To get the second serial port mode.\n");
	printf("    root@Moxa:~# setuartmode  -n 2 -g\n");
}

extern int optind, opterr, optopt; 
extern char *optarg;

int	main(int argc, char *argv[])
{
	int fd, result;
	int num;
	int bSet = 0;
	int bGet = 0;
	port_type_t uart_port = {
		.port_number = 1,
		.port_type = 0
	};
	char optstring[] = "hn:s:g";
	char c;

	if ( argc <= 1 ) {
		usage(argv[0]);
		return (0);
	}

	while ((c = getopt(argc, argv, optstring)) != -1)
		switch (c) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'n':
			uart_port.port_number = atoi(optarg);
			if ( uart_port.port_number <= ( DEFAULT_PORT_NUMBER - 1 ) || uart_port.port_number > ( DEFAULT_PORT_NUMBER + 1 ) ) {
				printf(" n:%d is not in %d and %d\n", uart_port.port_number, ( DEFAULT_PORT_NUMBER - 1 ), ( DEFAULT_PORT_NUMBER + 1) );
				return 0;
			}
			break;
		case 's':
			bSet = 1; 
			uart_port.port_type=atoi(optarg) ;
			if ( uart_port.port_type < 0 || uart_port.port_type > 2 ) {
				printf(" s:%d port is not in type(%d) 0, 1 and 2\n", uart_port.port_number, uart_port.port_type);
				return 0;
			}
			break;
		case 'g':
			bGet = 1;
			break;
		case '?':
			printf("Invalid option\n");
			usage(argv[0]);
			return 0;
		default:
			usage(argv[0]);
			return 0;
		}

	// to open the device node
	fd = open("/dev/moxa_uart8250_mode", O_RDWR);
	if ( fd < 0 ) {
		printf("Open device, /dev/moxa_uart8250_mode, fail !\n");
		goto main_close;
	}

	uart_port.port_number--;

	if ( ( uart_port.port_number >= DEFAULT_PORT_NUMBER-1 && uart_port.port_number <= DEFAULT_PORT_NUMBER ) && bGet == 0 && bSet == 0 ) {
		printf("The -n option should be used with -g or -s\n");
		printf("EX: To set the first serial port on by, `setuartmode -n 1 -s 1`\n");
		goto main_close;
	}
	else if ( bGet == 1 ) {
		result = ioctl(fd, IOCTL_GET_8250_MODE, &uart_port);
		if ( result < 0 ) {
			printf("ioctl(), fail !\n");
			goto main_close;
		}
		printf("Get the %dth serial port type: %s\n", uart_port.port_number+1, uart_interface_mode[uart_port.port_type]);
	}
	else if ( bSet == 1 ) {

		printf("Set the %dth serial port type: %s\n", uart_port.port_number+1, uart_interface_mode[uart_port.port_type]);
		result = ioctl(fd, IOCTL_SET_8250_MODE, &uart_port);
		if ( result < 0 ) {
			printf("device_set() fail\n");
			goto main_close;
		}
	}

main_close:
	close(fd);

	return 0;
}
