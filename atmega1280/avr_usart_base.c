#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char C_dir, B_dir, A_dir;


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




	DDRB = _BV(PB7) | _BV(PB6) | _BV(PB5);


	sei();

   for (;;) // Loop forever
   {
   }   

}
ISR(TIMER1_OVF_vect)
{
	if (OCR1C > 0xF000)
		C_dir = 1;
	if (OCR1C < 0x0010)
		C_dir = 0;
	if (OCR1B > 0xF000)
		B_dir = 1;
	if (OCR1B < 0x0010)
		B_dir = 0;
	if (OCR1A > 0xF000)
		A_dir = 1;
	if (OCR1A < 0x0010)
		A_dir = 0;

	if (C_dir)
		OCR1C = (-(OCR1C>>6)+OCR1C-1) % 0XFFFF;
	else
		OCR1C = ((OCR1C>>6)+OCR1C+1) % 0XFFFF;
	if (B_dir)
		OCR1B = (-(OCR1B>>6)+OCR1B-2) % 0XFFFF; 
	else
		OCR1B = ((OCR1B>>6)+OCR1B+2) % 0XFFFF; 
	if (A_dir)
		OCR1A = (-(OCR1A>>6)+OCR1B-3) % 0XFFFF; 
	else
		OCR1A = ((OCR1A>>6)+OCR1B+3) % 0XFFFF; 

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
