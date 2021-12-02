#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MX_USB_POWER_NAME "/dev/moxa_usb_power"
#define MX_USB_POWER_MINOR 109
#define IOCTL_SET_FRONT_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,0,int)
#define IOCTL_GET_FRONT_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,1,int)
#define IOCTL_SET_REAR_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,2,int)
#define IOCTL_GET_REAR_END_USB_POWER	_IOW(MX_USB_POWER_MINOR,3,int)

void usage(char *name) {
	printf("Enable/Disable the USB ports utility\n");
	printf("Usage: %s -r|-f [-e|-d] [-h]\n", name);
	printf("    Show the Front-end, Rear-end USB ports enable/disable status if no argument apply.\n");
	printf("    -h: Show this information\n");
	printf("    -r: Rear-end USB ports. You shall use the '-b' or '-f' option in the mx_usb_power.\n");
	printf("    -f: Front-end USB ports. You shall use the '-b' or '-f' option in the mx_usb_power.\n");
	printf("    -d: Disable the USB ports.\n");
	printf("    -e: Enable the USB ports.\n");
}

/*
 * bFront: 1 - Control the fornt-end USB ports; 0 - Not to control the front-end USB ports.
 * bRear: 1 - Control the rear-end USB ports; 0 - Not to control the rear-end USB ports.
 * bDisable: 1 - Disable the USB ports; 0 - Enable the USB ports.
 */
int usb_power_enable_disable(int bFront, int bRear, int bDisable) {
	int fd;
	int on_off;
	int ret;

	fd = open(MX_USB_POWER_NAME, O_RDONLY);
	if ( fd < 0 ) {
		printf("No such file, %s, or device!\n", MX_USB_POWER_NAME);
		return -1;
	}

	if ( bFront==1 ) {
		if ( bDisable==1 ) {
			printf("Disable the front-end USB ports\n");
			on_off = 0; /* 0: to disable; 1: to enable */
		}
		else {
			printf("Enable the front-end USB ports\n");
			on_off = 1; /* 0: to disable; 1: to enable */
		}

		ret = ioctl(fd, IOCTL_SET_FRONT_END_USB_POWER, &on_off);
		if ( ret < 0 ) {
			perror("IOCTL_SET_FRONT_END_USB_POWER fail\n");
			close(fd);
			return ret;
		}
	}

	if ( bRear==1 ) {
		if ( bDisable==1 ) {
			printf("Disable the rear-end USB ports\n");
			on_off = 0; /* 0: to disable; 1: to enable */
		}
		else {
			printf("Enable the rear-end USB ports\n");
			on_off = 1; /* 0: to disable; 1: to enable */
		}
		
		ret = ioctl(fd, IOCTL_SET_REAR_END_USB_POWER, &on_off);
		if ( ret < 0 ) {
			perror("IOCTL_SET_REAR_END_USB_POWER fail\n");
			close(fd);
			return ret;
		}
	}

	close(fd);

	return 0;
}

int usb_power_status() {
	int fd;
	int on_off;

	fd = open(MX_USB_POWER_NAME, O_RDONLY);
	if ( fd < 0 ) {
		printf("No such file, %s, or device!\n", MX_USB_POWER_NAME);
		return -1;
	}

	if ( ioctl(fd, IOCTL_GET_FRONT_END_USB_POWER, &on_off) < 0 ) {
		perror("IOCTL_GET_FRONT_END_USB_POWER fail\n");
		close(fd);
		return 0;
	}

	printf("The front-end USB ports power is %s\n", (on_off) ? "on" : "off" );

	if ( ioctl(fd, IOCTL_GET_REAR_END_USB_POWER, &on_off) < 0 ) {
		perror("IOCTL_GET_REAR_END_USB_POWER fail\n");
		close(fd);
		return -1;
	}

	printf("The rear-end USB ports power is %s\n", (on_off) ? "on" : "off" );

	close(fd);

	return 0;
}

extern int optind, opterr, optopt;
extern char *optarg, **environ;

int main(int argc, char *argv[]) {
	int	ret, i, c ;
	int	bRear=0;	/* bRear: 1: Control the rear USB ports */
	int	bFront=0;	/* bFront: 1: Control the front USB ports */
	int	bDisable=0;	/* bDisable: 
				      1: Disable the USB ports; 
				      0: Enable the USB ports; (Default)
				*/
	char	optstring[] = "hrfde";

	if ( argc == 1 ) {
		usb_power_status();
		return 0;
	}
	
	while ((c = getopt(argc, argv, optstring)) != -1)
		switch (c) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'r':
			bRear=1;
			break;
		case 'f':
			bFront=1; 
			break;
		case 'd':
			bDisable=1;
			break;
		case 'e':
			bDisable=0;
			break;
		case '?':
			printf("Invalid option\n");
			usage(argv[0]);
			return 0;
		default:
			usage(argv[0]);
			return 0;
		}


	if( (bRear == 0 && bFront == 0) ) {
		printf("You shall specific the rear, -b, or the front, -f, USB ports to enable or disable.\n");
		usage(argv[0]);
		return 0;
	}
	else {
		usb_power_enable_disable(bFront, bRear, bDisable);
	}

	return 0;
}
