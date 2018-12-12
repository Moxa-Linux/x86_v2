#ifndef __MX_EXPC1519_MODE_8250__
#define __MX_EXPC1519_MODE_8250__

/*
 * The bitmap of the UART mode register
 *
 * bit 0: UART port 0 RS-485-2W mode
 * bit 1: UART port 0 RS-232 mode
 * bit 2: UART port 1 RS-422/485-4W mode
 * bit 3: reserved
 * bit 4: UART port 0 RS-422/485-4W 
 * bit 5: UART port 1 modeRS-232 mode
 * bit 6: UART port 1 RS-485-2W mode
 * bit 7: reserved
 *
 */
#define UART_MODE_IO_ADDRESS 0x304 //General IO Address
#define UART_MODE_BIT_COM1_RS232 1
#define UART_MODE_BIT_COM1_RS485 0
#define UART_MODE_BIT_COM1_RS422 3
#define UART_MODE_BIT_COM2_RS232 4
#define UART_MODE_BIT_COM2_RS485 5
#define UART_MODE_BIT_COM2_RS422 2

#define UART0_MASK ( (1<<UART_MODE_BIT_COM1_RS232) | (1<<UART_MODE_BIT_COM1_RS485) | (1<<UART_MODE_BIT_COM1_RS422) ) // 0x0B
#define UART1_MASK ( (1<<UART_MODE_BIT_COM2_RS232) | (1<<UART_MODE_BIT_COM2_RS485) | (1<<UART_MODE_BIT_COM2_RS422) ) // 0x34

#endif
