/* 	Copyright (C) MOXA Inc. All rights reserved.
   
   	This is free software distributed under the terms of the
   	GNU Public License. See the file COPYING-GPL for details.
*/

/*
	conftool.c
*/

/*****************************************************************************/
/* Program:								      */
/* 		MOXA New PCIe Device Utility				      */
/*									      */
/* History:								      */
/* 2008/04/29								      */
/*									      */
/* Author:								      */
/* Joy Tu						      */
/*									      */
/* Discription:								      */
/* The utility is to set or get the MOXA new PCIe configurations	      */
/*									      */
/******************************************************************************/

#include        <stdlib.h>
#include	<stdio.h>
#include        <string.h>
#include        <fcntl.h>
#include	<getopt.h>
#include	<unistd.h>

#define MX_IOCTL_CODE		0x400
#define MX_SET_INTERFACE	(MX_IOCTL_CODE+79)
#define MX_GET_INTERFACE	(MX_IOCTL_CODE+80)
#define MX_SET_TERM		(MX_IOCTL_CODE+81)
#define MX_GET_TERM		(MX_IOCTL_CODE+82)

#define MX_RS232		0x00
#define MX_RS422		0x01
#define MX_RS485_2W		0x0f
#define MX_RS485_4W		0x0b
#define MX_TERM_NONE		0x00
#define MX_TERM_120		0x01

#define OPT_OPEN		1
#define OPT_HELP		2
#define OPT_SET_INTERFACE	4
#define OPT_GET_INTERFACE	8
#define OPT_SET_TERM		16
#define OPT_GET_TERM		32

static int mx_open_device(char *dev);
static int mx_set_interface(int fd, char *dev, char *parm);
static int mx_set_terminator(int fd, char *dev, char *parm);
static void mx_get_interface_term(int fd, char *dev);
static void mx_show_help();

int main(int argc,char *argv[])
{
	int fd, c, opt;
	char *dev, *parm;
	extern char *optarg;
	extern int optind, opterr, optopt;

	opt = 0;
	while ((c = getopt(argc, argv, "i:t:gh")) != -1) {
		switch (c) {
			case 'i':
				opt |= (OPT_OPEN | OPT_SET_INTERFACE);
				parm = optarg;
				break;
			case 't':
				opt |= (OPT_OPEN | OPT_SET_TERM);
				parm = optarg;
				break;
			case 'g':
				opt |= (OPT_OPEN | OPT_GET_INTERFACE | 
					OPT_GET_TERM);
				break;
			case 'h':
			case '?':
				opt |= OPT_HELP;
				break;
		}
	}
	
	if (!opt)
		opt |= OPT_HELP;

	dev = argv[optind];

	if (opt & OPT_OPEN) {
		if ((fd = mx_open_device(dev)) < 0)
			exit(0);
	}

	if (opt & OPT_SET_INTERFACE)
		mx_set_interface(fd, dev, parm);
	else if (opt & OPT_SET_TERM)
		mx_set_terminator(fd, dev, parm);
	else if ((opt & OPT_GET_INTERFACE) && (opt & OPT_GET_TERM))
		mx_get_interface_term(fd,dev);
	else if(opt & OPT_HELP)
		mx_show_help();

	close(fd);

	return 0;
}


static int mx_open_device(char *dev)
{
	int fd;

	fd = open(dev,O_RDWR);

	if (fd < 0) {
		printf("muestty: Open device %s error (%d)!\n",dev,fd);
		return -1;
	}
	
	return fd;
}


static int mx_set_interface(int fd, char *dev, char *parm)
{
	int mode ,ret;
	
	mode = 0;

	if (strncmp(parm,"RS232",5)==0)
		mode = MX_RS232;
	else if (strncmp(parm,"RS422",5)==0)
		mode = MX_RS422;
	else if (strncmp(parm,"RS4852W",8)==0)
		mode = MX_RS485_2W;
	else if (strncmp(parm,"RS4854W",8)==0)
		mode = MX_RS485_4W;
	else 	
		mode = -1;
	if ((ret = ioctl(fd,MX_SET_INTERFACE,mode)) < 0) {
		printf("muestty : Invaild operation of MOXA Smartio MUE series device.\n");
		return -5;
	}

	printf("muestty: Set interface of %s ok.\n", dev);

	return 0;
}

static int mx_set_terminator(int fd, char *dev, char *parm)
{
	int mode, ret;
	
	mode = 1;

	if (strncmp(parm, "NONTERM", 7) == 0)
		mode = MX_TERM_NONE;
	else if (strncmp(parm, "120TERM", 7) == 0)
		mode = MX_TERM_120;	
	else	
		mode = -1;
	if ((ret = ioctl(fd, MX_SET_TERM, mode)) < 0) {
		printf("muestty : Invaild operation of MOXA Smartio MUE series device.\n");
		return -1;
	}
	printf("muestty : Set interface of %s ok.\n", dev);
	return 0;
}

static void mx_get_interface_term(int fd, char *dev)
{
	int mode ,ret;

	mode = 0;
	ioctl(fd, MX_GET_INTERFACE, &mode);
	switch (mode) {
		case MX_RS232:
			printf("muestty: %s is set to RS-232 mode.\n",dev);
			break;
		case MX_RS422:
			printf("muestty: %s is set to RS-422 mode.\n",dev);
			break;
		case MX_RS485_2W:
			printf("muestty: %s is set to RS-485 2W mode.\n",dev);
			break;
		case MX_RS485_4W:
			printf("muestty: %s is set to RS-485 4W mode.\n",dev);
			break;
	}
	ioctl(fd, MX_GET_TERM, &mode);
	switch (mode) {
		case MX_TERM_NONE:
			printf("muestty: %s None terminal resistor.\n",dev);
			break;
		case MX_TERM_120:
			printf("muestty: %s 120ohm terminal resistor.\n",dev);
			break;
	}
}

static void mx_show_help()
{
	printf("Usage: muestty <operation> device\n");
	printf("\n");	
	printf("device: The MUE series device node.\n");
	printf("\n");	
	printf("operation	-h	  Help\n");
	printf("		-g	  Get interface & terminator type\n"); 
	printf("		-i intf   Set interface type with options");
	printf(" below\n");
	printf("		-t value  Set terminator resistor with");
	printf(" options below\n");
	printf("\n");	
	printf("intf 		RS232     RS-232 mode\n");
	printf("		RS422     RS-422 mode\n");
	printf("		RS4852W   RS-485 2 wire mode\n");
	printf("		RS4854W	  RS-485 4 wire mode\n");
	printf("\n");	
	printf("value		NONTERM	  None terminator resistor\n");
	printf("		120TERM	  120ohm terminator resistor\n");
}
