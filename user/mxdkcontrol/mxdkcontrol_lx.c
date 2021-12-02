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
#include <fcntl.h>
#include "mxdkcontrol.h" /* prototype */

#define IOCTL_SET_SLOT 1
int
mxdkcontrol_set_slot( unsigned int fd, int slot_num, int on) 
{
	struct slot_info si = {slot_num, on};
	return ioctl(fd, IOCTL_SET_SLOT, &si);
}


unsigned int
mxdkcontrol_open()
{
	unsigned int fd;
	fd = open( (const char*) "/dev/mxdkcontrol", O_RDWR);
	if (fd == -1) fd = 0; /* return 0 for fail */
	return fd;
}
