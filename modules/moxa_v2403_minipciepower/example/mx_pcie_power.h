/*
  Copyright (C) MOXA Inc. All rights reserved.
  This software is distributed under the terms of the
  MOXA License.  See the file COPYING-MOXA for details.
*/
#ifndef _MXDK02_H_
#define _MXDK02_H_

#ifdef __cplusplus
extern "C" {
#endif

#define IOCTL_SET_SLOT			1

struct slot_info {
	int     num;
	int     value;// 1 for on, 0 for off
};

/**
 * Open the Moxa EPM-DK02 device 
 * @return  0 on failure, otherwise return fd
 */
unsigned int mxpciepower_open(); 

/**
 * Close the fd of Moxa EPM-DK02 device
 * @param fd the file descriptor of device
 * @return  None
 */
#ifdef WIN32
int mxpciepower_close(unsigned int fd);
#else /* Linux */
#define mxpciepower_close(fd)			close((int)fd)
#endif

/**
 * Power on /off a slot's USB interface in EPM-DK02 module
 * @param fd the file descriptor of device
 * @param slot_num the slot number to be control
 * @param on 1: power on the slot, 0: power off the slot
 * @return  0 on success, otherwise returns a negative value
 */
int mxpciepower_set_slot(unsigned int fd, int slot_num, int on);


#endif /* _MXDK02_H_ */
