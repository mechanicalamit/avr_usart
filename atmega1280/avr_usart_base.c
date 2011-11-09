#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>


//Global variable for the clock system
volatile unsigned int   clock_millisecond=0;
volatile unsigned char  clock_second=0;
volatile unsigned char  clock_minute=0;

/* Prototypes */
void send_byte_usart0(char ch);
void send_time_usart(void);


int main (void)
{
	/* USART init */
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register

	UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
	

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

