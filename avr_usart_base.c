#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>


int main (void)
{
	/* PCF PWM setup */
	TCCR1B = _BV(CS10); /* No prescaling */
	TCCR1B |= _BV(WGM13);
	TCCR1A = _BV(WGM10); /* PWM */
	OCR1A = 0xFFFF; /* TOP = MAX */

	TIMSK1 |= _BV(TOIE1);

	OCR1B = 0x00FF; /* output pin OC1B, PB6 */
	TCCR1A |= _BV(COM1B1); /* Non-inverting */



	DDRB = _BV(PB7) | _BV(PB6);

	PORTB |= _BV(PB7);

	sei();

   for (;;) // Loop forever
   {
   }   

}
ISR(TIMER1_OVF_vect)
{
	OCR1B = (OCR1B + 0x10) % 0x0FFF;
}
