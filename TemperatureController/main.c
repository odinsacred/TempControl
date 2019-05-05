/*
* TemperatureController.c
*
* Created: 02.05.2019 16:01:58
* Author : MIK
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include "DS18B20/ds18b20.h"
#include "7SegLed/7segLed.h"

#define PCIE0 5

#define UP 2
#define DOWN 4
#define SET 8
#define RESET 16

#define UCSK     PB7
#define DI       PB6
#define DO       PB5
#define CS		 PD2

void init();
void initPinChangeInterrupts();
void sendPacket(unsigned char addr, unsigned char data);

enum states{
	waiting,
	setting
};

struct {
	unsigned int up : 1;
	unsigned int down : 1;
	unsigned int set : 1;
	unsigned int reset : 1;
}tasks;

unsigned int temperature;
unsigned int temp;
enum states state;

volatile char s;

int main(void)
{
	char val;
	// Возможно без этого кода "в железе" работать не будет 
	// Crystal Oscillator division factor: 1 настройка предделителя
	//#pragma optsize-
	//CLKPR=0x80;
	//CLKPR=0x00;
	//#ifdef _OPTIMIZE_SIZE_
	//#pragma optsize+
	//#endif
	state = waiting;
	temperature = 0;
	temp = 0;
	init();
	while (1)
	{
	val = readtemp();
	showValue(val);
		switch(state){
			case waiting:
				if(tasks.set){
					temp = temperature;
					state = setting;
				}
				break;
			case setting:
				if(tasks.up)
					temp++;
				if(tasks.down)
					temp--;
				if(tasks.reset)
					temp = 0;
				if(tasks.set){
					temperature = temp;
					state = waiting;
				}
		}

		
	}
}

void init(){
	DDRB = 0x1E;
	PORTB = 0;

	DDRD |= 1<<CS;
	PORTD |= 1<<CS;
	initPinChangeInterrupts();
	ledInit();
	//dsInit(PORTB,DDRB, PINB,0);
	dsInit(PD0,DDD0, PINB0,0);
}


void initPinChangeInterrupts(){
	GIMSK |= 1<<PCIE0; 
	PCMSK |= 1<<PCINT1|1<<PCINT2|1<<PCINT3|1<<PCINT4;
	sei();
}






ISR(PCINT_vect){
	if(PINB & UP){
		tasks.up = true;
		//spiSend(0xff);
		//PORTD = 0x4; 
	}
	if(PINB & DOWN){
		tasks.down = true;
		//PORTD = 0x8; 
	}
	if(PINB & SET){
		tasks.set = true;
		//PORTD = 0x10; 
	}
	if(PINB & RESET){
		tasks.reset = true;
		//PORTD = 0x20; 
	}
}
