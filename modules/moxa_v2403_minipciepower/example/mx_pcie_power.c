/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.

    2010-07-22 Wes Huang Wes.Huang@moxa.com
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mx_pcie_power.h"

void usage()
{
	printf("Usage : mx_pcie_power [socket_num] [0|1]\n");
        printf("\t[socket_num] :\n");
        printf("\t\t1 : socket1 PCIe/USB\n");
        printf("\t\t2 : socket2 USB(Sierra)\n");
        printf("\t[0|1] :\n");
        printf("\t\t0 : Power off\n");
        printf("\t\t1 : Power on\n");
        printf("Example :\n");
        printf("\tSocket1 power off\n");
        printf("\t\tmx_pcie_power 1 0\n");
}

int main(int argc, char *argv[]) {
	int fd;
	int slot_num;
	int val;
	char *endptr;

	if (argc != 3) {
		printf("Incorrect number of parameters\n");
		usage();
		return -1;
    } 

	slot_num = strtol(argv[1], &endptr, 10);
	if (strcmp(endptr, "")) {
		printf("Error: slot = %s invalid part = %s\n", argv[1], endptr);
		exit(1);
	}
	val = strtol(argv[2], &endptr, 10);
	if (strcmp(endptr, "")) {
		printf("Error: val = %s invalid part = %s\n", argv[2], endptr);
		exit(1);
	}

	if (((slot_num != 1) && (slot_num != 2)) 
			|| ((val != 0) && (val != 1))) {
		printf("Incorrect parameter\n");
		usage();
		return -1;
	}

	fd = mxpciepower_open();
	if (fd == 0)  {
		printf("Can't open the device /dev/mxpciepower");
		return -1;
	}

	if (mxpciepower_set_slot(fd, slot_num, val)) 
		printf("Incorrect IOCTL!\n");
	else  if (val) {
		switch(slot_num) {
			case 1:
				printf("Power on the PCIe/USB interface in socket1\n");
				break;
			case 2:
				printf("Power on the USB(Sierra) interface in socket2\n");
				break;
		}
	}
	else {
		switch(slot_num) {
                        case 1:
                                printf("Power off the PCIe/USB interface in socket1\n");
                                break;
                        case 2:
                                printf("Power off the USB(Sierra) interface in socket2\n");
                                break;
                }
	}
	mxpciepower_close(fd);

	return 0;
}

