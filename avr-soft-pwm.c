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


int
main(void)
{

	serial_init();
	sei();
	puts_P(PSTR("Hello, world."));

	avr_soft_pwm_irq_init();

	printf_P(PSTR("F_CPU=%ld HZ OCR0A=%ld, %ld ns per step\r\n"),
		(uint32_t)F_CPU,
		(uint32_t)T0_MAX,
		(uint32_t)NS_PER_T0_OVF);

	while(1){
		if(serial_status()) {
			int i;
			char c;

			c = getchar();
			if (c == '?') {
				printf_P(PSTR("PWM Values: "));
				for (i=0; i<sizeof(pwm_vals); i++) {
					printf_P(PSTR(" %d"), pwm_vals[i]);
				}
				printf_P(PSTR("\r\n"));
				printf_P(PSTR("PWM CTR: 0x%04x\r\n"), pwm_ctr);
			}
			if (c == '-')
				pwm_vals[2]--;
			if (c == '+')
				pwm_vals[2]++;
		}
	}
}
