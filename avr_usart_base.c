#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>

const char mystr[] PROGMEM="help me! make this work!";

void myputchar(char c);
void myputs_P(const char *str);

int main (void)
{
		/* USART init */


		/* Timer 1 */
   TCCR1B |= _BV(CS12) ; // Timer 1, prescalar 256


   sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed

   DDRB = _BV(PB0);

   set_sleep_mode(SLEEP_MODE_IDLE);

   for (;;) // Loop forever
   {
		 sleep_mode(); /* Let the ISRs wake us up */
		 myputchar('w'); myputchar('\r'); myputchar('\n');
		 if (bit_is_set(PINB, PB1)){
				 myputs_P(PSTR("PB1 is set"));
				 PORTB |= _BV(PB0);
		 }
		 else{
				 myputs_P(PSTR("PB1 is not set"));
				 PORTB &= ~_BV(PB0);
		 }
				 

   }   
}

void myputchar(char c)
{
}

void myputs_P(const char *str)
{
		while(pgm_read_byte(str) != '\0'){
				myputchar(pgm_read_byte(str++));
		}
		 myputchar('\r');
		 myputchar('\n');
}
