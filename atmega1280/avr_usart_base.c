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
/*	TCCR1A = _BV(WGM10);  PWM */

	TCCR1A |= _BV(COM1B1); /* Non-inverting */
	TCCR1A |= _BV(COM1A1); /* Non-inverting */
	TCCR1A |= _BV(COM1C1); /* Non-inverting */

	ICR1 = 0xFFFF; /* TOP = MAX */

	TIMSK1 |= _BV(TOIE1);

	OCR1B = 0x0c7F; /* output pin OC1B, PB6 */
	OCR1A = 0x0c7F; 
	OCR1C = 0x0c7F; 




	DDRB = _BV(PB7) | _BV(PB6);


	sei();

   for (;;) // Loop forever
   {
   }   

}
ISR(TIMER1_OVF_vect)
{
	OCR1C = ((OCR1C>>6)+OCR1C+1) % 0XFFFF;
	OCR1B = ((OCR1B>>6)+OCR1B+2) % 0XFFFF; 
	/*static char dir = 1;

	if (dir){
		OCR1B += OCR1B>>1;
		if (OCR1B >= 0x00FF)
			dir = 0;
	}
	else{
		OCR1B -= OCR1B>>1;
		if (OCR1B <= 0x10)
			dir = 1;
	}*/
}
