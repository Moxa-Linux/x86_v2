
#ifdef DEBUG 
	#define p(fmt, args...) printk("%s: "fmt, __FUNCTION__, ##args);
#else
	#define p(fmt, args...) 
#endif 

#define HW_VENDOR_ID_H			((u8)0x87)

#ifdef DA682B
#define HW_VENDOR_ID_L			((u8)0x86)
#else
#define HW_VENDOR_ID_L			((u8)0x83)
#endif

#define	SUPERIO_PORT			((u8)0x2e)
#define CHIP_ID_BYTE1			((u8)0x20)
#define CHIP_ID_BYTE2			((u8)0x21)
#define LOGIC_DEVICE_NUMBER		((u8)0x07)
#define GPIO_CONFIG_REG			((u8)0x07)

#define WATCHDOG_TIMER1_CTRL		((u8)0x71)
#define UART_AUTODIRECTION_CONTROL_ADDRESS 0xF0  // In ITE Super IO

extern u8 superio_read(u8 logic_dev, u8 index, u8 port_num);
extern void superio_write(u8 logic_dev, u8 index, u8 port_num, u8 val);
