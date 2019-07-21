/*
* TemperatureController.c
*
* Created: 02.05.2019 16:01:58
* Author : MIK
*/

#define F_CPU 4000000UL

#include "ds18b20.h"
#include "7segLed.h"
#include "soft_timer.h"
#include "keyboard.h"
#define PCIE0 5

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
#define CHATTER_PERIOD 100

void beep();
//объявление функций статическими дает экономию памяти, если эти функции находятся в других файлах...
void main_init();

enum states{
	waiting,
	setting,
	alarm,
	ack_alarm
};



uint8_t temperature;
enum states state;

uint8_t settingTemp;
size_t _poll_timer = 0;
size_t _buzz_timer = 0;
size_t _chatter_timer = 0;

int main(void)
{
    struct tasks task_list;
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

		if(timer_check(_chatter_timer)==TIMER_OUT){
			keyboard_refresh(&task_list);
			timer_restart(_chatter_timer,CHATTER_PERIOD);
		}

		switch(state){
			case waiting:
			if(task_list.set){			
				state = setting;
				task_list.set = 0;
			}
			led_show_value(temperature, CELCIUS_SYMBOL);
			if(temperature > settingTemp){
				state = alarm;
			}
			break;
			case setting:
			if(task_list.up){
				settingTemp++;
				task_list.up = 0;
			}
			if(task_list.down){
				settingTemp--;
				task_list.down = 0;
			}
			if(task_list.set){
				state = waiting;
				task_list.set = 0;
			}
			led_show_value(settingTemp,SETTING_SYMBOL);
			break;
			case alarm:
			//PORTD |= 1<<BUZZER;
			led_show_value(temperature,ALARM_SYMBOL);
			if(task_list.set){
				state = ack_alarm;
				task_list.set = 0;
			}
			break;
			case ack_alarm:
			//PORTD &= ~(1<<BUZZER);
			led_show_value(temperature,ALARM_SYMBOL);
			if(temperature < settingTemp){
				state = waiting;
			}
			if(task_list.set){
				state = setting;
				task_list.set = 0;
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
	//init_pin_change_interrupts();
	led_init();
	//timer0Init();
	timer_init_soft_timer();
	_chatter_timer = timer_create(CHATTER_PERIOD);
	_poll_timer = timer_create(POLL_TIMEOUT);
	_buzz_timer = timer_create(BUZZ_PERIOD);
	//beep();
	sei();
}

void beep(){
	//PORTD |= 1<<BUZZER;
	//_delay_ms(100);
	//PORTD &= ~(1<<BUZZER);
}





