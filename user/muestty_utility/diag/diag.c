/* 	Copyright (C) MOXA Inc. All rights reserved.
   
   	This is free software distributed under the terms of the
   	GNU Public License. See the file COPYING-GPL for details.
*/

/*
	diag.c
*/

/*****************************************************************************/
/* DIAG.C                                                                    */
/*									     */
/* Copyright (c) Moxa Inc. 2008. All Rights Reserved.	     */
/*									     */
/* Revision history:							     */
/*     8/5/99   Casper                                                       */
/*     3/9/2001 Casper     add Industio                                      */
/*****************************************************************************/

#include <stdio.h>
#include <fcntl.h>

char DIAG_VER[] = "1.3";
#include "../global.h"

#define	MOXA_GET_MAJOR       (MOXA + 63)
#define	MOXA_GET_CUMAJOR     (MOXA + 64)

int main()
{
	int	retpci = 0, retpcie = 0;
	int	fd,pcie_fd;
	int	i, j, brd_cnt;
	int	ports;
        int     calloutmajor, major, pcie_calloutmajor, pcie_major;
	char    title_str[100];
	struct mxser_hwconf mxsercfg[MXSER_BOARDS];
	struct mxupcie_hwconf mxupciecfg[MXSER_BOARDS];

	printf("\n == MOXA Smartio/Industio Family Multiport Board Status Utility(%s) ==\n", DIAG_VER);

	brd_cnt = 0;
	if ((fd = open("/dev/mxser", O_RDWR)) < 0) {
	    //printf("Open PCI /dev/mxser fail\n");
            printf("Please run msmknod and load driver first or there are no Card in the System.\n");
	    retpci = -1;
	    goto no_pci;
	}

	if (ioctl(fd, MOXA_GET_MAJOR, &major)<0) {
            printf("Can't get tty major number.\n");
            close(fd);
            return 1;
        }
        printf("Tty device major number= %d.\n", major);
	
	
	if (ioctl(fd, MOXA_GET_CUMAJOR, &calloutmajor)<0) {
            printf("Can't get callout device major number.\n");
            close(fd);
            return 1;
        }
        printf("Callout device major number= %d.\n\n", calloutmajor);
	
	if (ioctl(fd, MOXA_GET_CONF, mxsercfg) < 0) {
            printf("Can't get driver configuration.\n");
            close(fd);
            return 1;
        }
	close(fd);
	for (i = 0; i < MXSER_BOARDS; i++) {
	    if (mxsercfg[i].board_type == -1)
	    	continue;
	    
	    ports = mxser_numports[mxsercfg[i].board_type-1];

            if ((mxsercfg[i].pciInfo.busNum == 0) && 
               (mxsercfg[i].pciInfo.devNum == 0)) {
                //ISA board
		printf("Board %d : %s\n", i+1,
 			mxser_brdname[mxsercfg[i].board_type-1]);
      	    
      	        for (j = 0; j < ports && j < MXSER_PORTS_PER_BOARD; j++) {
      		    if (mxsercfg[i].baud[j] == 115200)
       		        printf("\tPort %d: 0x%lx, max. baud rate = %s bps.\n", j+				1, mxsercfg[i].ioaddr[j], "115200");
      		    else
       		        printf("\tPort %d: 0x%lx, max. baud rate = %s bps.\n", j+				1, mxsercfg[i].ioaddr[j], "921600");
                }
            } else {
      	        printf("Board %d : %s (BusNo=%d, DevNo=%d)\n",
                              i+1, mxser_brdname[mxsercfg[i].board_type-1],
                              mxsercfg[i].pciInfo.busNum,
                              mxsercfg[i].pciInfo.devNum >> 3);
                              
      	        for (j = 0; j < ports && j < MXSER_PORTS_PER_BOARD; j++) {
        		printf("\tPort %d: 0x%lx, max. baud rate = %d bps.\n", 
			j+1,mxsercfg[i].ioaddr[j], 
			mxsercfg[i].MaxCanSetBaudRate[j]);
		}
			
            }
	    printf("\n");
	    brd_cnt++;
	}

no_pci:
	if ((pcie_fd = open("/dev/mxupcie",O_RDWR)) < 0) {
		//printf("Open PCIe /dev/mxupcie fail\n");
		printf("Please run msmknod and load driver first or there are no Card in the system.\n");
		retpcie = -1;
		goto no_pcie;
	}
	
	if (ioctl(pcie_fd, MOXA_GET_MAJOR, &pcie_major ) < 0) {
		printf("Can't get PCIe tty major number.\n");
		close(pcie_fd);
		return 1;
	}
	printf("PCIe tty device major number= %d.\n", pcie_major);

	if (ioctl(pcie_fd, MOXA_GET_CUMAJOR, &pcie_calloutmajor) < 0) {
		printf("Cant get PCIe callout device major number\n");
		close(pcie_fd);
		return 1;
	}
	printf("PCIe callout device major number= %d.\n\n", pcie_calloutmajor);
	/*get PCIe hw_configuration*/	
	if (ioctl(pcie_fd, MOXA_GET_CONF, mxupciecfg) < 0) {
            printf("Can't get driver configuration.\n");
            close(pcie_fd);
            return 1;
        }
	close(pcie_fd);
	for (i = 0; i < MXSER_BOARDS; i++) {
	    if (mxupciecfg[i].board_type == -1)
	    	continue;
	    ports = mxupcie_numports[mxupciecfg[i].board_type-1];
            if ((mxupciecfg[i].pciInfo.busNum == 0) && 
               (mxupciecfg[i].pciInfo.devNum == 0)) {
                //ISA board
		printf("Board %d : %s\n", i+1+brd_cnt,
 			mxupcie_brdname[mxupciecfg[i].board_type-1]);
      	         
      	        for (j = 0; j < ports && j < MXSER_PORTS_PER_BOARD; j++) {
      		    if (mxupciecfg[i].baud[j] == 115200)
       		        printf("\tPort %d: %p, max. baud rate = %s bps.\n", j+				1, mxupciecfg[i].ioaddr[j], "115200");
      		    else
       		        printf("\tPort %d: %p, max. baud rate = %s bps.\n", j+				1, mxupciecfg[i].ioaddr[j], "921600");
                }
            } else {
      	        printf("Board %d : %s (BusNo=%d, DevNo=%d)\n", i+1+brd_cnt,
			mxupcie_brdname[mxupciecfg[i].board_type-1],
                              mxupciecfg[i].pciInfo.busNum,
                              mxupciecfg[i].pciInfo.devNum >> 3);
                              
      	        for (j = 0; j < ports && j < MXSER_PORTS_PER_BOARD; j++) {
        		printf("\tPort %d: %p, max. baud rate = %d bps.\n", 
			j+1,mxupciecfg[i].ioaddr[j], 
			mxupciecfg[i].MaxCanSetBaudRate[j]);
		}
			
            }
            printf("\n");
	}
no_pcie:
	if(retpci == -1 && retpcie == -1)
		return 1;

        return 0;
}
       

