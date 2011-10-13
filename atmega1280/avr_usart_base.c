#define F_CPU 16000000
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

//Hardware definitions
#define PORT 		PORTB
#define PIN			PINB
#define DDR			DDRB
#define REDLED 	PB5
#define GREENLED 	PB6
#define BLUELED 	PB7
#define BTN			PB3

//Fading States
#define REDtoYELLOW 	0
#define YELLOWtoGREEN 	1
#define GREENtoCYAN 	2
#define CYANtoBLUE 	3
#define BLUEtoVIOLET 	4
#define VIOLETtoRED 	5

//Less Typing of _
#define BV _BV

//Maximim value for led brightness
#define V_MAX 255
#define M_CONST 150

char mode=0;
char state; //Counter for each mode. Reset to 0 while changing modes

//Current color values
unsigned char red;
unsigned char green;
unsigned char blue;

//Predefine function
void button(void);

char state2; //State in the rainbow fade progress
void rainbowfade(int n){
	//Go one step through a state machine that fades through the rainbow
	//n sets the step increment
	//255%n must equal 0
	if (state2==REDtoYELLOW) green+=n;
	if (state2==YELLOWtoGREEN) red-=n;
	if (state2==GREENtoCYAN) blue+=n;
	if (state2==CYANtoBLUE) green-=n;
	if (state2==BLUEtoVIOLET) red+=n;
	if (state2==VIOLETtoRED) blue-=n;
	if  (red==V_MAX || green==(V_MAX-M_CONST) || blue==(V_MAX-M_CONST) || red==0 || green==0 || blue==0){
		//Finished fading a color; move on to the next
		state2++;
		state2%=6;
	}
}

void mode1(void){
	rainbowfade(1); //Fade 1 step per PWM Cycle
}

void mode2(void){
	state++;
	if (state%128==0){//Every 128 PWM cycles
		//Save old colors
		char r=red;
		char g=green;
		char b=blue;
		//Rotate colors
		red=g;
		green=b;
		blue=r;
	}
}

void mode3(void){
	state++;
	if (state%128==0){ //Every 128 PWM Cycles
		rainbowfade(255); //Skip to the next color
	}
}

void mode0(void);

#define NUM_MODES 4

//Array of mode function pointers
typedef void(*modefn)(void);
modefn modefns[NUM_MODES]={
	mode0,
	mode1,
	mode2,
	mode3,
};
modefn current; //Function pointer for current mode

void setmode(char m){
	//Switch mode to m
	mode=m;

	//Reset state
	red=255;
	green=0;
	blue=0;
	state=0;
	state2=0;

	//Set function pointer
	current=modefns[(int)mode];
}


void mode0(void){
	PORT|=BV(PB4); //Turn on debug LED
	PORT|=(BV(REDLED)| BV(BLUELED)| BV(GREENLED)); //Turn off LEDs

	unsigned char n=10;
	while (n--) _delay_ms(255); 	//Wait for a bit so it doesn't wake up while
											//the button is still down

	PORT&=~BV(PB4); //Turn off debug LED


	setmode(1); //Switch to mode 1
	while (n>250){ //wait until  button is released
		_delay_ms(25);
	}
}

SIGNAL(SIG_PIN_CHANGE0){ //Button change, used to wake from sleep, not while running
	PORT|=BV(PB4);
}

void timer(void){ //Called once per PWM Cycle
	/* current(); //Let the current mode update colors */
   mode1();
}

int main(void){
	unsigned int i=0;

	//Set pins to output
	DDR|= _BV(PB7) | _BV(PB6) | _BV(PB5);

	

	setmode(1); //Start with mode 1
	timer(); //Let mode set initial color

	while (1) {
		//Software PWM
		if (i<red){
			PORT &=~ BV(REDLED);
		}else{
 			PORT|=BV(REDLED);
		}
		if (i<green){
			PORT &=~ BV(GREENLED);
		}else{
 			PORT|=BV(GREENLED);
		}
		if (i<blue){
			PORT &=~ BV(BLUELED);
		}else{
 			PORT|=BV(BLUELED);
		}

		if (i>=512){ //After blinking LEDs 255 times
			timer();
			i = 0;
		}
		i++;
	}
	return 0; //Will never get here
}
