#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/sleep.h>
#include <util/delay.h>


int main (void)
{



   DDRB = _BV(PB7);


   for (;;) // Loop forever
   {
	_delay_ms(500);
	PORTB |= _BV(PB7);
	_delay_ms(500);
	PORTB &= ~_BV(PB7);
   }   
}
