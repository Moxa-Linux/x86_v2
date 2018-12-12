#define MOXA_HOTSWAP_MINOR 110
#define DEVICE_NAME "hotswap"
#define mprintk(fmt, args...) printk(KERN_ERR "moxa_hotswap:"fmt,  ##args) 

/* 
 * Supersit bits 
 */
#define V2416A
#ifdef V2416A

#define LED_BTN_ADDR 0x301
#define BIT_DISK1_LED 0x10
#define BIT_DISK2_LED 0x20
#define BIT_DISK1_BTN 0x01
#define BIT_DISK2_BTN 0x02

#else

#define LED_BTN_ADDR 0x301
#define BIT_DISK1_LED 0x01
#define BIT_DISK2_LED 0x02
#define BIT_DISK1_BTN 0x04
#define BIT_DISK2_BTN 0x08

#endif


/*
 *	Hardware specific part
 */
#define MY_DEVICE_ID1 0x1c03	/* IDE interface: Intel Corporation 6 Series/C200 Series Chipset Family 6 port SATA IDE Controller */

/*
#define SATA_IO_ADDR_1	0x701
#define SATA_IO_ADDR_2	0x704
#define SATA_IO_LED_BIT 4
#define SATA_IO_BUTTON_BIT 7
*/

#define moxainb inb_p
#define moxaoutb outb_p

typedef struct _disk_info {
	unsigned int status;
	int busy;
	int idle_cnt;
} disk_info;

/*
 * Debug
 */

#ifdef DEBUG 
#define p(fmt, args...) printk("%s: "fmt, __FUNCTION__, ##args) // use ## to remove commaa, for not args condition
#define pp(fmt, args...)  /* pp: not print debug message */
#else
#define p(fmt, args...) 
#define pp(fmt, args...)
#endif
