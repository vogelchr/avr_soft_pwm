#include "serial.h"
#include <stdio.h>
#include <avr/io.h>

/* static FILE structure for stdout */
static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE uart_stdin  = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

#define UART_BUFSIZE    32

static inline void
uart_put_into_outbuf(unsigned char c) {
	while (!(UCSR0A & _BV(UDRE0)))
		asm ("nop;");
	UDR0 = c;
}

static int
uart_putchar(char c, FILE *stream)
{
	if(c == '\r')
		return 0;
	if(c == '\n')
		uart_put_into_outbuf('\r');
	uart_put_into_outbuf(c);
	return 0;
}

static int
uart_getchar(FILE *stream)
{
	/* wait for Receive Complete bit to be set */
//	while(!(UCSR0A & _BV(RXC0)));
	return UDR0;
}

/* there is a byte to be read */
int
serial_status(){
	return UCSR0A & _BV(RXC0);
}

void
serial_init(){
	UBRR0  = 103; /* 16 MHz clockrate, U2Xn=0, 9600 bps */
	UCSR0B = _BV(RXEN0)|_BV(TXEN0);
	UCSR0C = _BV(UCSZ01)|_BV(UCSZ00); /* 8 bit, no parity, 1 stopbit */

	stdout = &uart_stdout;
	stdin  = &uart_stdin;
}
