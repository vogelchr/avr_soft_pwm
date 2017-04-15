/* ----------------------------------------------------------------------------
(c) 2017 Christian Vogel <vogelchr@vogel.cx>
---------------------------------------------------------------------------- */

#include "avr-soft-pwm-irq.h"
#include "serial.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

enum protocol_state {
	PROTO_IDLE,
	PROTO_CH1,
	PROTO_VAL1,
	PROTO_VAL2
};

uint8_t char_to_nibble(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - ('a'-10);		
	else if (c >= 'A' && c <= 'F')
		return c - ('A'-10);
	return 0xff;
}

int
main(void)
{
	enum protocol_state state = PROTO_IDLE;
	uint8_t chnum = 0;
	uint8_t i;
	uint8_t err=0;
	uint8_t val=0;
	char c;

	serial_init();

	puts_P(PSTR("Hello, world. '?' for help.\r\n"));

	avr_soft_pwm_irq_init();
	sei();

	while(1){
		if (err)
			printf_P(PSTR("ERR\r\n"));
		err = i = 0;
		if(!serial_status())
			continue;

		c = getchar();

		switch (state) {
		case PROTO_IDLE:
			switch (c) {
			case '?':
				printf_P(PSTR("CPU: f = %ld Hz pwm max = %ld, %ld ns per step\r\n"),
					(uint32_t)F_CPU,
					(uint32_t)T0_MAX,
					(uint32_t)NS_PER_T0_OVF);
				printf_P(PSTR("curr chnum %d\r\n"), chnum);
				for (i=0; i<6; i++)
					printf_P(PSTR("%c pwm[%d]=%d\r\n"),
						i==chnum?'*':' ',
						i, pwm_vals[i]);
				printf_P(PSTR(
					"</>: select ch#\r\n"
					"+/-: adjust value\r\n"
					"0..9,a..f,A..F: set value to x/16th of range\r\n"
					"kX : select channel, X=0..5\r\n"
					"vXX : set value XX=00...ff\r\n"
					"q   : quick get channel value\r\n"));
				break;
			case '<':
			case '>':
			case '+':
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				if (c == '<') {
					if (chnum > 0)
						chnum--;
					else {
						printf_P(PSTR("ch<?\r\n"));
						break;
					}
				}
				if (c == '>') {
					if (chnum < 5)
						chnum++;
					else {
						printf_P(PSTR("ch>?\r\n"));
						break;
					}
				}
				i = pwm_vals[chnum];
				if (c == '-') {
					if (i > 0)
						i--;
					else {
						printf_P(PSTR("val<?\r\n"));
						break;
					}
				}
				if (c == '+') {
					if (i < 255)
						i++;
					else {
						printf_P(PSTR("val>?\r\n"));
						break;
					}
				}
				val = char_to_nibble(c);
				if (val != 0xff) {
					i = (val << 4) | val;
				}
				cli();
				pwm_vals[chnum] = i;
				sei();
				printf_P(PSTR("v[%d]=%d OK\r\n"), chnum, i);
				break;
			case 'v':
				val = 0;
				state = PROTO_VAL1;
				break;
			case 'k':
				val = 0;
				state = PROTO_CH1;
				break;
			case 'q':
				printf_P(PSTR("%d %d\r\n"), chnum, pwm_vals[i]);
				break;
			default:
				printf_P(PSTR("cmd?\r\n"));
				break;
			};
			break;
		case PROTO_CH1:
		case PROTO_VAL1:
		case PROTO_VAL2:
			i = char_to_nibble(c);
			if (i == 0xff) {
				printf_P(PSTR("hex?\r\n"));
				state = PROTO_IDLE;
				break;
			}
			val += i;

			if (state == PROTO_CH1) {
				if (i < 6) {
					chnum = val;
					printf_P(PSTR("c%d OK\r\n"), chnum);
				} else {
					printf_P(PSTR("ch?\r\n"));
				}
				state = PROTO_IDLE;
				break;
			}

			if (state == PROTO_VAL1) {
				val <<= 4;
				state = PROTO_VAL2;
				break;
			}

			if (state == PROTO_VAL2) {
				pwm_vals[chnum] = val;
				printf_P(PSTR("v[%d]=%d OK\r\n"), chnum, val);
				state = PROTO_IDLE;
				break;
			}
		}
	}
}
