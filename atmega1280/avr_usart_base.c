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


int main (void)
{
	/* USART init */
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register

	UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
	
	/* Timer 0 will count and overflow at every 1 ms
	 * F_CPU = 16 MHz, prescalar = 62, timer0 = 250kHz
	 * 1 ms = 250 ticks */
	TCNT0 = 0; /* Init timer */
	TCCR0A = _BV(WGM01); /* CTC mode */
	OCR0A = 250; /* 250 ticks */
	TIMSK0 = _BV(OCIE0A); /* Enable overflow interrupt */
	TCCR0B = _BV(CS01) | _BV(CS00); /* prescalar = 64 */


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

	itoa((uint16_t)clock_minute, time_str, 10);
	for (i=0; i<=time_str_len-1; ++i){
		if ((time_str[i] == '\0') || (time_str[i] == '\n'))
			break;
		send_byte_usart0(time_str[i]);
	}
	send_byte_usart0(':');
	itoa((uint16_t)clock_second, time_str, 10);
	for (i=0; i<=time_str_len-1; ++i){
		if ((time_str[i] == '\0') || (time_str[i] == '\n'))
			break;
		send_byte_usart0(time_str[i]);
	}
	send_byte_usart0(':');
	itoa((uint16_t)clock_millisecond, time_str, 10);
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
//The output compate interrupt handler
//We set up the timer in such a way that
//this ISR is called exactly at 1ms interval
ISR(TIMER0_COMPA_vect)
{
	clock_millisecond++;
	if(clock_millisecond==1000) {
		clock_second++;
		clock_millisecond=0;
		if(clock_second==60) {
			clock_minute++;
			clock_second=0;
			}
		if(clock_minute==60) {
			clock_minute=0;
		}
	}
}
