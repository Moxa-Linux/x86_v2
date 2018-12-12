/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    2011-04-21 WadeLiang
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mx_pcie_power.h" /* prototype */

#define IOCTL_SET_SLOT 1
int
mxpciepower_set_slot( unsigned int fd, int slot_num, int on) 
{
	struct slot_info si = {slot_num, on};
	return ioctl(fd, IOCTL_SET_SLOT, &si);
}


unsigned int
mxpciepower_open()
{
	unsigned int fd;
	fd = open( (const char*) "/dev/mxpciepower", O_RDWR);
	if (fd == -1) fd = 0; /* return 0 for fail */
	return fd;
}
