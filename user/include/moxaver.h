
/*
 * This is define Moxa product version rule.
 * History:
 * Date		Author			Comment
 * 11-11-2004	Victor Yu.		Create it.
 */
#ifndef _MOXAVER_H
#define _MOXAVER_H

#define MAJOR_VER(x) ( ( x & 0xFF000000) >>24 )
#define MINOR_VER(x) ( ( x & 0x00FF0000) >>16 )
#define OEM_VER(x) ( ( x & 0x0000FF00) >>8 )
#define TESTING_VER(x) ( x & 0x000000FF )

union ver_union {
	unsigned long   ul;     // version unsigned long format
	struct s_part {
		unsigned char   major;  // major version number
		unsigned char   minor;  // minor version number
		unsigned char   oem;    // oem version number
		unsigned char   testing;        // oem upgrade version number
	} part;
};

#endif	// _MOXAVER_H
