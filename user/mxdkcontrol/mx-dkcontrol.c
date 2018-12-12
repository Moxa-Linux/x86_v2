/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.

    2010-12-10 Wade Liang Wade.Liang@moxa.com
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mxdkcontrol.h"

int main(int argc, char *argv[]) {
	int fd;
	int slot_num;
	int val;
	char *endptr;

	if (argc != 3) {
		printf("Incorrect number of parameters\n");
		printf("Usage: mx-dkcontrol [slot_num] [1/0]\n");
                printf("\t[slot_num] :\n");
                printf("\t\t1. slot1 socket1 PCIe/USB\n");
                printf("\t\t2. slot1 socket2 USB\n");
                printf("\t\t3. slot2 socket1 PCIe/USB\n");
                printf("\t\t4. slot2 socket2 USB\n");
                printf("\t[1/0] :\n");
                printf("\t\t0. Power off\n");
                printf("\t\t1. Power on\n");
		return -1;
    } 

	slot_num = strtol(argv[1], &endptr, 10);
	if (strcmp(endptr, "")) {
		printf("Error: slot=%s invalid part=%s\n", argv[1], endptr);
		exit(1);
	}
	val = strtol(argv[2], &endptr, 10);
	if (strcmp(endptr, "")) {
		printf("Error: val=%s invalid part=%s\n", argv[2], endptr);
		exit(1);
	}

	if (((slot_num !=1) && (slot_num !=2) && (slot_num !=3) && (slot_num != 4)) 
			|| ((val != 0) && (val != 1))) {
		printf("Incorrect parameter\n");
		printf("Usage: mx-dkcontrol [slot_num] [1/0]\n");
		printf("\t[slot_num] :\n");
		printf("\t\t1. slot1 socket1 PCIe/USB\n");
		printf("\t\t2. slot1 socket2 USB\n");
		printf("\t\t3. slot2 socket1 PCIe/USB\n");
		printf("\t\t4. slot2 socket2 USB\n");
		printf("\t[1/0] :\n");
                printf("\t\t0. Power off\n");
                printf("\t\t1. Power on\n");
		return -1;
	}

	fd = mxdkcontrol_open();
	if (fd == 0)  {
		printf("Can't open the device /dev/mxdkcontrol");
		return -1;
	}

	if (mxdkcontrol_set_slot(fd, slot_num, val)) 
		printf("Incorrect IOCTL!\n");
	else  if (val) {
		switch(slot_num) {
			case 1:
				printf("Power on the PCIe/USB interface in slot1 socket1\n");
				break;
			case 2:
				printf("Power on the USB interface in slot1 socket2\n");
				break;
			case 3:
                                printf("Power on the PCIe/USB interface in slot2 socket1\n");
                                break;
			case 4:
                                printf("Power on the USB interface in slot2 socket2\n");
                                break;
		}
	}
	else {
		switch(slot_num) {
                        case 1:
                                printf("Power off the PCIe/USB interface in slot1 socket1\n");
                                break;
                        case 2:
                                printf("Power off the USB interface in slot1 socket2\n");
                                break;
                        case 3:
                                printf("Power off the PCIe/USB interface in slot2 socket1\n");
                                break;
                        case 4:
                                printf("Power off the USB interface in slot2 socket2\n");
                                break;
                }
	}
	mxdkcontrol_close(fd);

	return 0;
}

