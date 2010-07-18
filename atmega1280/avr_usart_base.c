#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t clkcount = 0; /* 16 bit counter upgrade for timer 1 */


int main (void)
{
	/* USART init */
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register

	UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)

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

void send_byte_usart0(char ch)
{
	while (bit_is_clear(UCSR0A, UDRE0))
		/* nothing */;
	UDR0 = ch;
}

void send_time_usart(void)
{
#define time_str_len 32
	uint8_t i;
	char time_str[time_str_len];
	uint16_t secs, m_secs;

	/* the next two lines are totally wrong */
	secs = clkcount << 8;
	m_secs = (clkcount & 0xFF) >> 2;

	itoa((uint16_t)clkcount, time_str, 10);
	for (i=0; i<=time_str_len-1; ++i){
		if ((time_str[i] == '\0') || (time_str[i] == '\n'))
			break;
		send_byte_usart0(time_str[i]);
	}
	send_byte_usart0('\n');
}

ISR(TIMER1_OVF_vect)
{
	OCR1C = ((OCR1C>>6)+OCR1C+1) % 0XFFFF;
	OCR1B = ((OCR1B>>6)+OCR1B+2) % 0XFFFF; 

	clkcount++;
	/*if ((clkcount & 0xFF) == 0xFF) */
		send_time_usart();
}

ISR(USART0_RX_vect)
{
   char ReceivedByte;
   ReceivedByte = UDR0; // Fetch the recieved byte value into the variable "ByteReceived"
   if (ReceivedByte & 0x20)
		   ReceivedByte &= 0xDF;
   else
		   ReceivedByte |= 0x20;
   send_byte_usart0(ReceivedByte); // Echo back the received byte back to the computer
} 

