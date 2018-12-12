
/*
 * This is a include file for upgrade firmware define file.
 * History:
 * Date		Author			Comment
 * 11-11-2004	Victor Yu.		Create it.
 */
#ifndef _UPFIRM_H
#define _UPFIRM_H

#include	"moxaver.h"
typedef struct firmware_header_struct {
	int             headerlength;   // this header length
	int             totalfileno;    // the firmware context include total file no.
	unsigned long   checksum;       // not include this header
	unsigned long   totallength;    // not include this header
	unsigned long	apidlist;	// match apid list, add by Victor Yu. 12-08-2004
} firmware_header_t;
#define OLD_FRM_HEADER_LENGTH	16
#define MATCH_APID_NO(x)	(((x)-16)/4)
	
typedef struct file_header_struct {
	int             headerlength;   // this header length
	char            filename[64];   // this file name, must null ending
	char            versionname[64];        // this file version name on redboot
						// configuration, must null ending
	int             mtdno;          // for MTD using, to know which MTD
	unsigned long   checksum;       // this file checksum, not include this header
	unsigned long   filelength;     // this file length, not include this header
	union ver_union version;
} file_header_t;

#endif	// _UPFIRM_H
