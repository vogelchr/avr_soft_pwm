#ifndef AVR_SOFT_PWM_IRQ_H
#define AVR_SOFT_PWM_IRQ_H

#include <stdint.h>

/* timer0 counting from 0 .. T0_MAX */
#define T0_MAX         96ULL

/* resulting nanoseconds / step */
#define NS_PER_T0_OVF  1000000000ULL / ((0ULL+(F_CPU)) / ((T0_MAX)+1))

/* period for PWM signal */
#define NS_PWM_PERIOD  20000000ULL  /* 2e7ns = 20ms */

/* minimum high period */
#define NS_MIN_HIGH     1000000ULL  /* 1e6ns = 1 ms */

extern uint8_t pwm_vals[6];
extern uint16_t pwm_ctr;

extern void avr_soft_pwm_irq_init();


#endif

