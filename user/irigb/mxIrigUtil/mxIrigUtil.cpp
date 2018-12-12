/*
  Copyright (C) MOXA Inc. All rights reserved.
  This software is distributed under the terms of the
  MOXA License.  See the file COPYING-MOXA for details.
*/

/**
 * @file mxIrigUtil.cpp : Utility for Moxa IRIGB Card.
 *
 * @version 1.1.0.0 - 2015/07/01
 *
 * @author holsety.chen@moxa.com
 */
#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mxirig/mxirig.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\mxirig.lib")
#else
#pragma comment(lib, "..\\Release\\mxirig.lib")
#endif
#endif

const char *strTimeSrc[] =
{ "Free Run(Internal RTC)", "Fiber In", "Port 1 In" };

const char *strSignalStatus[] =
{ "Normal", "Off Line", "Frame Error", "Parity Error" };

const char *strPortList[] =
{ "Port Fiber", "Port 1", "Port 2", "Port 3", "Port 4" };

const char *strSignalType[] =
{ "TTL", "DIFFERENTIAL" };

const char *strOutputMode[] =
{ "From Fiber In", "From Port 1 In", "IRIG-B", "PPS" };

const char *strHardWareId[] =
{ "N/A", "DA_IRIGB_4DIO_PCI104", "N/A", "N/A", "N/A", "N/A", "N/A", "DA_IRIGB_S" };

const char *strParityCheckMode[] =
{ "Even", "Odd", "None" };

enum _IRIG_CMD_FUN_CODE_LIST_
{
	FUNCODE_mxIrigbGetHardwareID,
	FUNCODE_mxIrigbGetTime,
	FUNCODE_mxIrigbSetTime,
	FUNCODE_mxIrigbGetSyncTimeSrc,
	FUNCODE_mxIrigbSetSyncTimeSrc,
	FUNCODE_mxIrigbGetSignalStatus,
    FUNCODE_mxIrigbGetInputParityCheckMode,
    FUNCODE_mxIrigbSetInputParityCheckMode,
    FUNCODE_mxIrigbGetOutputParityCheckMode,
    FUNCODE_mxIrigbSetOutputParityCheckMode,
	FUNCODE_mxIrigbGetPpsWidth,
	FUNCODE_mxIrigbSetPpsWidth,
	FUNCODE_mxIrigbGetInputSignalType,
	FUNCODE_mxIrigbSetInputSignalType,
	FUNCODE_mxIrigbGetOutputSignalType,
	FUNCODE_mxIrigbSetOutputSignalType,
	FUNCODE_mxIrigbGetDigitalOutputSignal,
	FUNCODE_mxIrigbSetDigitalOutputSignal,
	FUNCODE_mxIrigbGetDigitalInputSignal,
	FUNCODE_mxIrigbGetFpgaBuildDate,

	FUNCODE_MAX
};

struct irigcmd
{
	int cmdFunCode;
	char *funDesc;
	int paraCount;
	char *paraDesc;
};

irigcmd irigcmdlist[] =
{
	{ FUNCODE_mxIrigbGetHardwareID, "Get Hardware ID", 0, NULL},
	{ FUNCODE_mxIrigbGetTime, "Get IRIG-B RTC Time", 0, NULL},
	{ FUNCODE_mxIrigbSetTime, "Set IRIG-B RTC Time", 6, 
		"[2000-2099],[1-12],[1-31],[0-23],[0-59],[0-59] (year[2000-2099],month[1-12],day[1-31],hour[0-23],min[0-59],sec[0-59]); default value is 2014,01,01,00,00,00"},
	{ FUNCODE_mxIrigbGetSyncTimeSrc, "Get IRIG-B RTC Sync. Source", 0, NULL},
	{ FUNCODE_mxIrigbSetSyncTimeSrc, "Set IRIG-B RTC Sync. Source", 1,
		"[0-2] (Source: 0=FreeRun In (Internal RTC), 1=Fiber In, 2=Port 1 In); default value is 2"},
	{ FUNCODE_mxIrigbGetSignalStatus, "Get IRIG-B Signal Status", 1, 
		"[1-2] (Source: 1=Fiber In, 2=Port 1 In); default value is 2"},
	{ FUNCODE_mxIrigbGetInputParityCheckMode, "Get IRIG-B Input Parity Check Mode", 1, 
		"Source[1-2] (1=Fiber In, 2=Port 1 In); default value is 2"},
	{ FUNCODE_mxIrigbSetInputParityCheckMode, "Set IRIG-B Input Parity Check Mode", 2, 
		"Source[1-2] (1=Fiber In, 2=Port 1 In),Mode[0-2] (0=Even, 1=Odd, 2=None); default value is 2,0"},
	{ FUNCODE_mxIrigbGetOutputParityCheckMode, "Get IRIG-B Output Parity Check Mode", 0, NULL},
	{ FUNCODE_mxIrigbSetOutputParityCheckMode, "Set IRIG-B Output Parity Check Mode", 1, 
		"Mode[0-1] (0=Even, 1=Odd); default value is 0"},
	{ FUNCODE_mxIrigbGetPpsWidth, "Get Pulse per second width(ms)", 0, NULL},
	{ FUNCODE_mxIrigbSetPpsWidth, "Set Pulse per second width(ms)", 1, "[0-999] (width: 0-999 ms); default value is 0"},
	{ FUNCODE_mxIrigbGetInputSignalType, "Get input signal type", 1, "[0-1] 0=Fiber, 1=Port 1 (port[0-1]); default value is 1"},
	{ FUNCODE_mxIrigbSetInputSignalType, "Set input signal type", 3, 
		"[0-1],[0-1],[0-1] \n\
		(port[0-1]: 0=Fiber 1=Port 1,\n\
		signal type[0-1]: 0=TTL, 1=DIFF,\n\
        inverse[0-1]: 0=No inverse 1=Inverse)\n\
		default value is 1,1,0"},
	{ FUNCODE_mxIrigbGetOutputSignalType, "Get output signal type", 1, "[1-4] (output port[1-4]); default value is 1"},
	{ FUNCODE_mxIrigbSetOutputSignalType, "Set output signal type", 4, 
		"[1-4],[0-1],[0-3],[0-1] \n\
		(output port[1-4]: Output port 1-4, \n\
		signal type[0-1]: 0=TTL, 1=DIFF,\n\
		mode[0-3]: 0=From Fiber Input Port, 1=From Port 1 Input, 2=From IRIG-B encode(Internal RTC), 3=From PPS encode;\n\
        inverse[0-1]: 0=No inverse, 1=Inverse)\n\
		default value is 1,1,2,0"},
	{ FUNCODE_mxIrigbGetDigitalOutputSignal, "Get Digital Output", 1,
		"[0-3] (digital output port[0-3]); default value is 0"},
	{ FUNCODE_mxIrigbSetDigitalOutputSignal, "Set Digital Output", 2,
		"[0-3],[0-1] (digital output port[0-3],level[0-1]); default value is 0,0"},
	{ FUNCODE_mxIrigbGetDigitalInputSignal, "Get Digital Input", 1,
		"[0-2] (digital input port[0-2]); default value is 0"},
	{ FUNCODE_mxIrigbGetFpgaBuildDate, "Get FPGA firmware build date", 0, NULL},
};

void usage(char *name) {
    printf("Get/set Moxa DA-IRIGB utility\n");
    printf("Usage: %s -f function_id [-p parameters] [-c] [-h]\n", name);
    printf("    Show the utility information if no argument apply.\n");
    printf("    -h: Show this information.\n");
    printf("    -c: Indicate the n-the IRIG-B Card.\n");
    printf("    -f: Pass function id argument to execute specify functionality\n");
    printf("    -p: Parameters for each function, use comma to pass multiple varible\n");
    printf("\nFor example: Set IRIG-B RTC Time 2014/01/01 03:25:00\n");
    printf("\t%s -f %d -p 2014,1,1,3,25,0\n", name, FUNCODE_mxIrigbSetTime);
    printf("\nFunction description list:\n");
    for (int i=0; i<(sizeof(irigcmdlist)/sizeof(irigcmd));i++)
    {
        printf("        %d:%s\n", irigcmdlist[i].cmdFunCode, irigcmdlist[i].funDesc);
        if (irigcmdlist[i].paraCount)
            printf("          -p %s\n", irigcmdlist[i].paraDesc);
    }
    printf("\n\n");
}

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

#ifdef WIN32
extern int
getopt(int nargc, char *const *nargv, const char *ostr);
#else
#include <unistd.h> 	/* For getopt() */
#endif
extern int optind, opterr, optopt;
extern char *optarg;

#ifdef __cplusplus    // If used by C++ code, 
}
#endif

int parsingString( char* source, char **split, char *key, int count )
{
	char *pch = strtok(source, key );
	int i;

	for (i=0;pch!=NULL && i<count;i++)
	{
		split[i] = pch;
		pch = strtok(NULL, key );	
	}

	return i;
}

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	int cardIndex = 0;
	int controlMode = -1;
	char parameters[260] = "";
    char optstring[] = "hc:f:p:";
    char c;
	BOOL ret = FALSE;
	int result = 0;
	char *p[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	RTCTIME rtctime;

    if ( argc == 1 ) {
        usage(argv[0]);
        return 0;
    }

    while ((c = getopt(argc, argv, optstring)) != -1)
        switch (c) {
        case 'h':
            usage(argv[0]);
            return 0;
        case 'c':
	    cardIndex=atoi(optarg);
            break;
	case 'f':
	    controlMode = atoi(optarg);
	    if ( controlMode < 0 || controlMode > (FUNCODE_MAX-1) ) {
	        printf("The controlMode is over range:%d\n", controlMode);
	        return 0;
	    }
	    break;
	case 'p':
	    strcpy(parameters,optarg);
	    break;
        case '?':
            printf("Invalid option\n");
            usage(argv[0]);
            return 0;
        default:
            usage(argv[0]);
            return 0;
        }

	if ( controlMode > (sizeof(irigcmdlist)/sizeof(irigcmd)))
	{
		fprintf(stderr, "Invalid Function code!\n");
		return -1;
	}

	if (irigcmdlist[controlMode].paraCount)
	{
		if ( irigcmdlist[controlMode].paraCount < 
			parsingString( parameters, p, ",", irigcmdlist[controlMode].paraCount ) )
		{
			fprintf(stderr, "Invalid Parameters!\n");
			return -1;
		}
	}

	HANDLE hDev = mxIrigbOpen(cardIndex);
#ifdef WIN32
	if (hDev == INVALID_HANDLE_VALUE || hDev == NULL)
#else
	if ( hDev < 0 ) 
#endif
	{
		fprintf(stderr, "mxIrigbOpen ");
#ifdef WIN32
		printf("error number = %d\n", GetLastError());
#endif
		return -1;
	}


	if (FUNCODE_mxIrigbGetHardwareID==controlMode)
	{
		DWORD dwHwId;
		ret = mxIrigbGetHardwareID(hDev, &dwHwId);
		if ( ret )
		{
			printf("Hardware ID = %d (%s)\n", dwHwId, strHardWareId[dwHwId]);
		}
	}
	else if (FUNCODE_mxIrigbGetTime==controlMode)
	{
		ret = mxIrigbGetTime( hDev, &rtctime);
		if ( ret )
		{
			printf("IRIG-B RTC = %d/%d/%d %d:%d:%d.%09d\nTZ = ", 
				rtctime.year, rtctime.mon, rtctime.mday,
				rtctime.hour, rtctime.min, rtctime.sec, rtctime.nanosec);
			if (rtctime.tzs)
				printf("-");
			else
				printf("+");
			printf("%d",rtctime.tz);
			if (rtctime.tzh)
				printf(".5");
			printf(", TQ = %d",rtctime.tq);
			printf(", LSP = %d",rtctime.lsp);
			printf(", LS = %d",rtctime.ls);
			printf(", DSP = %d",rtctime.dsp);
			printf(", DST = %d",rtctime.dst);
			printf("\n");
		}
	}
	else if (FUNCODE_mxIrigbSetTime==controlMode)
	{
		rtctime.year = ( !p[0] ) ? 2014 : atoi(p[0]);
		rtctime.mon = ( !p[1] ) ? 1 : atoi(p[1]);
		rtctime.mday = ( !p[2] ) ? 1 : atoi(p[2]);
		rtctime.hour = ( !p[3] ) ? 0 : atoi(p[3]);
		rtctime.min = ( !p[4] ) ? 0 : atoi(p[4]);
		rtctime.sec = ( !p[5] ) ? 0 : atoi(p[5]);

		/* 2000 and 2099 is the O.S. limitation. We don't allow to set the IRIG-B time over this range */
		if (rtctime.year>=2000 && rtctime.year<=2099 &&
			rtctime.mon>=1 && rtctime.mon<=12 &&
			rtctime.mday>=1 && rtctime.mday<=31 &&
			rtctime.hour>=0 && rtctime.hour<=23 &&
			rtctime.min>=0 && rtctime.min<=59 &&
			rtctime.sec>=0 && rtctime.sec<=59 )
		{
			ret = mxIrigbSetTime( hDev, &rtctime);
			if (ret)
			{
				printf("Set IRIGB RTC = %d/%d/%d %d:%d:%d\n", 
					rtctime.year, rtctime.mon, rtctime.mday,
					rtctime.hour, rtctime.min, rtctime.sec);
			}
		}
		else
		{
			result = -1;
		}
	}
	else if (FUNCODE_mxIrigbGetSyncTimeSrc==controlMode)
	{
		DWORD dwSync;
		ret = mxIrigbGetSyncTimeSrc( hDev, &dwSync);
		if (ret)
		{
			printf("Sync. Source = %d (%s)\n", dwSync, strTimeSrc[dwSync]);
		}
	}
	else if (FUNCODE_mxIrigbSetSyncTimeSrc==controlMode)
	{
		DWORD dwSync = ( !p[0] ) ? TIMESRC_PORT1 : atoi(p[0]);

		ret = mxIrigbSetSyncTimeSrc(hDev, dwSync);
		if (ret)
		{
			printf("Set Sync. Source = %d\n", dwSync);
		}
	}
	else if (FUNCODE_mxIrigbGetSignalStatus==controlMode)
	{
		DWORD dwSync = ( !p[0] ) ? TIMESRC_PORT1 : atoi(p[0]);
		DWORD dwStatus;
		
		ret = mxIrigbGetSignalStatus(hDev, dwSync, &dwStatus);
		if (ret)
		{
			printf("%s Signal status = %d(%s)\n", strTimeSrc[dwSync], dwStatus, strSignalStatus[dwStatus]);
		}
	}
	else if (FUNCODE_mxIrigbGetPpsWidth==controlMode)
	{
		DWORD dwPpsWidth;

		ret = mxIrigbGetPpsWidth(hDev, &dwPpsWidth);
		if (ret)
		{
			printf("PPS width = %d ms\n", dwPpsWidth);
		}
	}
	else if (FUNCODE_mxIrigbSetPpsWidth==controlMode)
	{
		DWORD dwPpsWidth = ( !p[0] ) ? 0 : atoi(p[0]);

		ret = mxIrigbSetPpsWidth(hDev, dwPpsWidth);
		if (ret)
		{
			printf("Set PPS width = %d ms\n", dwPpsWidth);
		}
	}
	else if (FUNCODE_mxIrigbGetInputSignalType==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? PORT_1 : atoi(p[0]);
		DWORD dwSignalType;
		BOOL bInvert;
		ret = mxIrigbGetInputSignalType( hDev, dwPort, &dwSignalType, &bInvert);
		if (ret)
		{
			printf("%s Input signal = %d(%s), Inverse = %d\n", 
				strPortList[dwPort], dwSignalType, strSignalType[dwSignalType], bInvert);
		}
	}
	else if (FUNCODE_mxIrigbSetInputSignalType==controlMode)
	{
		DWORD dwPort =  ( !p[0] ) ? PORT_1 : atoi(p[0]);
		DWORD dwSignalType =  ( !p[1] ) ? TYPE_DIFFERENTIAL : atoi(p[1]);
		BOOL bInvert =  ( !p[2] ) ? 0 : atoi(p[2]);

		if( ! ((bInvert==0) || (bInvert==1)) ) {
			printf("The inverse only support value 0 or 1.\n");
            mxIrigbClose(hDev);
			return -1;
		}

		/* The fiber only support TTL signal */
		if ( dwPort == PORT_FIBER ) {
			if ( dwSignalType == TYPE_DIFFERENTIAL ) {
				printf("The Fiber port only support TTL signal\n");
                mxIrigbClose(hDev);
				return -1;	
			}
		}

		ret = mxIrigbSetInputSignalType( hDev, dwPort, dwSignalType, bInvert);
		if (ret)
		{
			printf("Set %s Input signal = %d(%s), Inverse = %d\n", 
				strPortList[dwPort], dwSignalType, strSignalType[dwSignalType], bInvert);
		}
	}
	else if (FUNCODE_mxIrigbGetOutputSignalType==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? PORT_1 : atoi(p[0]);
		DWORD dwSignalType;
		DWORD dwMode;
		BOOL bInvert;

		ret = mxIrigbGetOutputSignalType(hDev,dwPort, &dwSignalType, &dwMode, &bInvert);
		if (ret)
		{
			printf("%s Output signal = %d(%s), Mode = %d(%s), Inverse = %d\n", 
				strPortList[dwPort], 
				dwSignalType, strSignalType[dwSignalType], 
				dwMode, strOutputMode[dwMode],
				bInvert);
		}
	}
	else if (FUNCODE_mxIrigbSetOutputSignalType==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? PORT_1 : atoi(p[0]);
		DWORD dwSignalType = ( !p[1] ) ? TYPE_DIFFERENTIAL : atoi(p[1]);
		DWORD dwMode = ( !p[2] ) ? MODE_IRIGB : atoi(p[2]);
		BOOL bInvert = ( !p[3] ) ? 0 : atoi(p[3]);

		ret = mxIrigbSetOutputSignalType(hDev,dwPort, dwSignalType, dwMode, bInvert);
		if (ret)
		{
			printf("Set %s Output signal = %d(%s), Mode = %d(%s), Inverse = %d\n", 
				strPortList[dwPort], 
				dwSignalType, strSignalType[dwSignalType], 
				dwMode, strOutputMode[dwMode],
				bInvert);
		}
	}
	else if (FUNCODE_mxIrigbGetDigitalOutputSignal==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? 0 : atoi(p[0]);
		DWORD dwValue;

		ret = mxIrigbGetDigitalOutputSignal( hDev, dwPort, &dwValue);
		if (ret)
		{
			printf("Get DO %d = %d\n", dwPort, dwValue);
		}
	}
	else if (FUNCODE_mxIrigbSetDigitalOutputSignal==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? 0 : atoi(p[0]);
		DWORD dwValue = ( !p[1] ) ? 0 : atoi(p[1]);

		ret = mxIrigbSetDigitalOutputSignal( hDev, dwPort, dwValue);
		if (ret)
		{
			printf("Set DO %d = %d\n", dwPort, dwValue);
		}
	}
	else if (FUNCODE_mxIrigbGetDigitalInputSignal==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? 0 : atoi(p[0]);
		DWORD dwValue;

		ret = mxIrigbGetDigitalInputSignal( hDev, dwPort, &dwValue);
		if (ret)
		{
			printf("Get DI %d = %d\n", dwPort, dwValue);
		}
	}
	else if (FUNCODE_mxIrigbSetInputParityCheckMode==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? TIMESRC_PORT1 : atoi(p[0]);
		DWORD dwValue = ( !p[1] ) ? PARITY_CHECK_EVEN : atoi(p[1]);

		ret = mxIrigbSetInputParityCheckMode( hDev, dwPort, dwValue);
		if (ret)
		{
			printf("Set %s IRIG-B input parity check mode = %s\n", strTimeSrc[dwPort], strParityCheckMode[dwValue]);
		}
	}
	else if (FUNCODE_mxIrigbGetInputParityCheckMode==controlMode)
	{
		DWORD dwPort = ( !p[0] ) ? TIMESRC_PORT1 : atoi(p[0]);
		DWORD dwValue;

		ret = mxIrigbGetInputParityCheckMode( hDev, dwPort, &dwValue);
		if (ret)
		{
			printf("%s IRIG-B input parity check mode = %s\n", strTimeSrc[dwPort], strParityCheckMode[dwValue]);
		}
	}
	else if (FUNCODE_mxIrigbSetOutputParityCheckMode==controlMode)
	{
		DWORD dwValue = ( !p[0] ) ? PARITY_CHECK_EVEN : atoi(p[0]);

		ret = mxIrigbSetOutputParityCheckMode( hDev, dwValue);
		if (ret)
		{
			printf("Set IRIG-B output parity check mode = %s\n", strParityCheckMode[dwValue]);
		}
	}
	else if (FUNCODE_mxIrigbGetOutputParityCheckMode==controlMode)
	{
		DWORD dwValue;

		ret = mxIrigbGetOutputParityCheckMode( hDev, &dwValue);
		if (ret)
		{
			printf("Get IRIG-B Output parity check mode = %s\n", strParityCheckMode[dwValue]);
		}
	}
	else if (FUNCODE_mxIrigbGetFpgaBuildDate==controlMode)
	{
		DWORD dwValue;

		ret = mxIrigbGetFpgaBuildDate( hDev, &dwValue);
		if (ret)
		{
			printf("FPGA firmware build date = %08x\n", dwValue);
		}
	}
    
	mxIrigbClose(hDev);

	if (!ret)
	{
		fprintf(stderr, "Invalid Command\n");
		result = -1;
	}

	return result;
}

