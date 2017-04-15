#include "avr-soft-pwm-irq.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define CTR_MAX       ((NS_PWM_PERIOD) / (NS_PER_T0_OVF))
#define CTR_SET_HIGH  (((NS_PWM_PERIOD)-(NS_MIN_HIGH)) / (NS_PER_T0_OVF))

uint8_t pwm_vals[6] = { 0, 255, 4, 4, 4, 4 };
static uint16_t pwm_ctr;

ISR(TIMER0_OVF_vect)
{
	uint16_t ctr;
	uint8_t  ctr_lo;

	ctr = pwm_ctr;

	if (ctr == CTR_SET_HIGH) {
		PORTC |= _BV(0);
		PORTC |= _BV(1);
		PORTC |= _BV(2);
		PORTC |= _BV(3);
		PORTC |= _BV(4);
		PORTC |= _BV(5);
		goto out;
	}

	if (ctr > 0xff)
		goto out;

	ctr_lo = ctr & 0xff; /* first assign low half, avoid 16bit compares later */

	if ( ctr_lo == pwm_vals[0] )
		PORTC &= ~_BV(0);
	if ( ctr_lo == pwm_vals[1] )
		PORTC &= ~_BV(1);
	if ( ctr_lo == pwm_vals[2] )
		PORTC &= ~_BV(2);
	if ( ctr_lo == pwm_vals[3] )
		PORTC &= ~_BV(3);
	if ( ctr_lo == pwm_vals[4] )
		PORTC &= ~_BV(4);
	if ( ctr_lo == pwm_vals[5] )
		PORTC &= ~_BV(5);
out:
	if (ctr == CTR_MAX)
		ctr = 0;
	else
		ctr++;
	pwm_ctr = ctr;
}


void avr_soft_pwm_irq_init() {
	TCCR0A = _BV(WGM01)|_BV(WGM00);
	OCR0A = T0_MAX;
	TCCR0B = _BV(CS00) | _BV(WGM02);
	TIMSK0 = _BV(TOIE0);

	/* PORTC[5 downto 0] are the soft-PWM outputs */
	DDRC = 0x3f;
	PORTC = 0x00;
}
