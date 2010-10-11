#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdio.h>
#include <stdlib.h>


int main (void)
{
	
	for (;;) // Loop forever
	{
		DDRB |= _BV(PB0) | _BV(PB1); /* Set as outout */
		PORTB &= ~(_BV(PB0) | _BV(PB1)); /* OFF */
		TCCR3B = 0;
		TCNT3 = 0;
		TCCR3B = _BV(CS30); /* Prescalar 1 */
		DDRB &= ~_BV(PB0); /* Set as input, no pullup */
		loop_until_bit_is_set(PINB, PB0);
		TCCR3B = _BV(CS30); /* Stop clock */
		if (TCNT3 > 250)
			PORTB |= _BV(PB1);
		else
			PORTB &= ~_BV(PB1);
	}   

}

