/*
* TemperatureController.c
*
* Created: 02.05.2019 16:01:58
* Author : MIK
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
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
#define BUZZER PD5

 #define NORMAL_MODE 0
 #define SETTING_MODE 1
 #define ALARM_MODE 2
 #define LOAD_MODE 3
 #define NORMAL_TEMP 30

 #define CELCIUS_SYMBOL 0b01100011
 #define SETTING_SYMBOL 0b00011101
 #define ALARM_SYMBOL 0b01110111

//void beep();
//объявление функций статическими дает экономию памяти, если эти функции находятся в других файлах...
void init();
inline void initPinChangeInterrupts();

enum states{
	waiting,
	setting,
	alarm,
	ack_alarm
};

struct {
	unsigned int up : 1;
	unsigned int down : 1;
	unsigned int set : 1;
	unsigned int reset : 1;
}tasks;

unsigned char temperature;
enum states state;

unsigned char settingTemp;

int main(void)
{
	// Возможно без этого кода "в железе" работать не будет 
	// Crystal Oscillator division factor: 1 настройка предделителя
	//#pragma optsize-
	//CLKPR=0x80;
	//CLKPR=0x00;
	//#ifdef _OPTIMIZE_SIZE_
	//#pragma optsize+
	//#endif
	settingTemp = NORMAL_TEMP;
	state = waiting;
	temperature = 0;
	init();
	while (1)
	{
		temperature = getTemp();
	
		switch(state){
			case waiting:
				if(tasks.set){
					state = setting;
					tasks.set = 0;
				}
				ledShowValue(temperature, CELCIUS_SYMBOL);
				if(temperature > settingTemp){
					state = alarm;
				}
				break;
			case setting:
				if(tasks.up){
					settingTemp++;
					tasks.up = 0;
					}
				if(tasks.down){
					settingTemp--;
					tasks.down = 0;
					}
				if(tasks.set){
					state = waiting;
					tasks.set = 0;
				}
				ledShowValue(settingTemp,SETTING_SYMBOL);
				break;
			case alarm:
				PORTD |= 1<<BUZZER;
				ledShowValue(temperature,ALARM_SYMBOL);
				if(tasks.set){
					state = ack_alarm;
					tasks.set = 0;
				}
				break;
			case ack_alarm:
				PORTD &= ~(1<<BUZZER);
				ledShowValue(temperature,ALARM_SYMBOL);
				if(temperature < settingTemp){
					state = waiting;
				}
				if(tasks.set){
					state = setting;
					tasks.set = 0;
				}
				break;
		}	
	}
}

void init(){
	DDRB = 0x0;
	PORTB = 0;

	DDRD |= 1<<CS|1<<BUZZER;
	PORTD |= 1<<CS|0<<BUZZER;
	initPinChangeInterrupts();
	ledInit();
	//beep();
}

void beep(){
	PORTD |= 1<<BUZZER;
	_delay_ms(100);
	PORTD &= ~(1<<BUZZER);
}


void initPinChangeInterrupts(){
	GIMSK |= 1<<PCIE0; 
	PCMSK |= 1<<PCINT1|1<<PCINT2|1<<PCINT3|1<<PCINT4;
	sei();
}



ISR(PCINT_vect){
	if(PINB & UP){
		tasks.up = 1;
	}
	if(PINB & DOWN){
		tasks.down = 1;
	}
	if(PINB & SET){
		tasks.set = 1;
	}
	//if(PINB & RESET){
		//tasks.reset = 1;
	//}
}
