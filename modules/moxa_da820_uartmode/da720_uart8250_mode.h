#ifndef __MX_DA720_MODE_8250__
#define __MX_DA720_MODE_8250__

/*
 * The bitmap of the UART mode register
 *
 * bit 0: UART port 0 RS-232 mode
 * bit 1: UART port 0 RS-485-2W mode
 * bit 2: UART port 0 RS-422/485-4W mode
 * bit 3: reserved
 * bit 4: UART port 1 RS-232 mode
 * bit 5: UART port 1 RS-485-2W mode
 * bit 6: UART port 1 RS-422/485-4W mode
 * bit 7: reserved
 *
 */

#define UART_MODE_IO_ADDRESS 0x200 //General IO Address
#define UART_MODE_BIT_COM1_RS232 3
#define UART_MODE_BIT_COM1_RS485 1
#define UART_MODE_BIT_COM1_RS422 2
#define UART_MODE_BIT_COM2_RS232 6
#define UART_MODE_BIT_COM2_RS485 4
#define UART_MODE_BIT_COM2_RS422 5

#define UART0_MASK ((1<<UART_MODE_BIT_COM1_RS232) | (1<<UART_MODE_BIT_COM1_RS485) | (1<<UART_MODE_BIT_COM1_RS422))
#define UART1_MASK ((1<<UART_MODE_BIT_COM2_RS232) | (1<<UART_MODE_BIT_COM2_RS485) | (1<<UART_MODE_BIT_COM2_RS422))

#endif
