/* mknod /dev/device_name c 10 MX_MINOR_NUMBER */
#define MX_MINOR_NUMBER 105
#define MX_NAME "mxdkcontrol"

/*
 * Ioctl
 */
#define IOCTL_SET_SLOT	1
#define DKCONTROL_GPIO_ADDR	0x304
/*
 * Debug
 */
#ifdef DEBUG 
#define p(fmt, args...) printk("mx_dkcontrol %s: "fmt, __FUNCTION__,##args)
#else
#define p(fmt, args...) 
#endif

typedef enum emethod {gpio, superio, digital_io} gpio_method;

typedef struct entry {
    char *name;
    u32 addr; /* address of dio status */
    gpio_method method; /* superio/ gpio */
    int log_dev; /* use for super io */
} dkcontrol_dev;

struct slot_info {
	int     num;
	int     value;// 1 for on, 0 for off
 };
