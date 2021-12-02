#ifndef __MX_MODE_8250__
#define __MX_MODE_8250__

#if (defined DA820)
#include "da820_uart8250_mode.h"
#elif (defined MC7200)
#include "mc7220_uart8250_mode.h"
#elif (defined MC1100)
#include "mc1100_uart8250_mode.h"
#elif (defined EXPC1519)
#include "expc1519_uart8250_mode.h"
#elif (defined DA720)
#include "da720_uart8250_mode.h"
#endif

#define MX_MODE_8250_MINOR  112

#define IOCTL_GET_8250_MODE _IOR(MX_MODE_8250_MINOR,1,int)
#define IOCTL_SET_8250_MODE _IOW(MX_MODE_8250_MINOR,2,int)

typedef struct __port_type_t {
	int port_number;	/* port number should be 1, 2 */
	int port_type;		/* port type: 0 - RS-232, 1 - RS-485-2W, 2 - RS-422/RS-485-4W */
} port_type_t;

#endif
