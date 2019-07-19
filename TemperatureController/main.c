/*
* TemperatureController.c
*
* Created: 02.05.2019 16:01:58
* Author : MIK
*/

#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ds18b20.h"
#include "7segLed.h"
#include "soft_timer.h"

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
#define PAUSE_BETWEEN_POLL 5

#define CELCIUS_SYMBOL 0b01100011
#define SETTING_SYMBOL 0b00011101
#define ALARM_SYMBOL 0b01110111

#define POLL_TIMEOUT 100
#define BUZZ_PERIOD 500

void beep();
//объявление функций статическими дает экономию памяти, если эти функции находятся в других файлах...
void main_init();
inline void init_pin_change_interrupts();

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

uint8_t temperature;
enum states state;

uint8_t settingTemp;
size_t _poll_timer = 0;
size_t _buzz_timer = 0;

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
	main_init();
	while (1)
	{
		if(timer_check(_poll_timer)==TIMER_OUT){
			temperature = ds18b20_get_temp();
			timer_restart(_poll_timer,POLL_TIMEOUT);
		}

		switch(state){
			case waiting:
			if(tasks.set){
				state = setting;
				tasks.set = 0;
			}
			led_show_value(temperature, CELCIUS_SYMBOL);
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
			led_show_value(settingTemp,SETTING_SYMBOL);
			break;
			case alarm:
			PORTD |= 1<<BUZZER;
			led_show_value(temperature,ALARM_SYMBOL);
			if(tasks.set){
				state = ack_alarm;
				tasks.set = 0;
			}
			break;
			case ack_alarm:
			PORTD &= ~(1<<BUZZER);
			led_show_value(temperature,ALARM_SYMBOL);
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
	return 0;
}

void main_init(){
	cli();
	DDRB = 0x00;
	PORTB = 0x00;

	DDRD |= 1<<CS|1<<BUZZER;
	PORTD |= 1<<CS|0<<BUZZER;
	ACSR |=1<<ACD; // отключение АЦП
	init_pin_change_interrupts();
	led_init();
	//timer0Init();
	timer_init_soft_timer();
	_poll_timer = timer_create(POLL_TIMEOUT);
	_buzz_timer = timer_create(BUZZ_PERIOD);
	beep();
	sei();
}

void beep(){
	PORTD |= 1<<BUZZER;
	_delay_ms(100);
	PORTD &= ~(1<<BUZZER);
}


void init_pin_change_interrupts(){
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

}
