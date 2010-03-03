#define F_CPU 7372800
#define USART_BAUDRATE 115200
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
   UCSRB |= (1 << RXEN) | (1 << TXEN);   // Turn on the transmission and reception circuitry
   UCSRC |= (1 << UCSZ0) | (1 << UCSZ1); // Use 8-bit character sizes

   UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
   UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register

   UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt (USART_RXC)
		/* Timer 1 */
   TCCR1B |= _BV(CS12) ; // Timer 1, prescalar 256
   TIMSK |= _BV(TOIE1);


   sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed

   DDRB = _BV(0);

   set_sleep_mode(SLEEP_MODE_IDLE);

   for (;;) // Loop forever
   {
		   sleep_mode();
         // Do nothing - echoing is handled by the ISR instead of in the main loop
		 myputchar('w'); myputchar('\r'); myputchar('\n');
		 myputs_P(mystr); myputchar('\r'); myputchar('\n');
   }   
}

void myputchar(char c)
{
		while (bit_is_clear(UCSRA, UDRE)){ };
		UDR = c;
}

void myputs_P(const char *str)
{
		while(pgm_read_byte(str) != '\0'){
				myputchar(pgm_read_byte(str++));
		}
}

ISR(USART_RX_vect)
{
   char ReceivedByte;
   ReceivedByte = UDR; // Fetch the recieved byte value into the variable "ByteReceived"
   if (ReceivedByte & 0x20)
		   ReceivedByte &= 0xDF;
   else
		   ReceivedByte |= 0x20;
   UDR = ReceivedByte; // Echo back the received byte back to the computer
   PORTB ^= _BV(0);
   myputchar('r');
} 

ISR(TIMER1_OVF_vect)
{
		/* Only used to wake the cpu to check for button presses */
		PORTB ^= _BV(0); /* And to toggle LED */
		myputchar('o');
}

