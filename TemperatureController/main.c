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
#include "buzzer.h"
#include "eeprom.h"

#define PCIE0 5

#define UCSK     PB7
#define DI       PB6
#define DO       PB5
#define CS		 PD2

#define EEPROM_ADDRESS 0
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
#define SHUTDOWN_TIMEOUT 10000

void beep();
//объявление функций статическими дает экономию памяти, если эти функции находятся в других файлах...
void main_init();

enum states{
	waiting,
	setting,
	alarm,
	pre_alarm,
	pre_set,
	ack_alarm
};



uint8_t temperature;
enum states state;

uint8_t _setting_temp;
size_t _poll_timer = 0;
size_t _buzz_timer = 0;
size_t _chatter_timer = 0;
size_t _shutdown_timer = 0;
int main(void)
{
    struct tasks task_list;
	_setting_temp = eeprom_read(EEPROM_ADDRESS);//NORMAL_TEMP;
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
				if(timer_check(_shutdown_timer)==TIMER_OUT){
					led_shutdown_mode_on();
					timer_restart(_shutdown_timer, SHUTDOWN_TIMEOUT);
				}
				if(task_list.set){
					led_shutdown_mode_off();								
					timer_restart(_shutdown_timer, SHUTDOWN_TIMEOUT);
					task_list.set = 0;
					state = pre_set;
				}
				led_show_value(temperature, CELCIUS_SYMBOL);
				if(temperature > _setting_temp){
					state = pre_alarm;
				}

			break;
			case pre_set:
				if(timer_check(_shutdown_timer)==TIMER_OUT){
					state = waiting;
				}
				if(task_list.set){					
					task_list.set = 0;
					state = setting;
				}
			break;
			case setting:
				led_show_value(_setting_temp,SETTING_SYMBOL);
				if(task_list.up){
					_setting_temp++;
					task_list.up = 0;
				}
				if(task_list.down){
					_setting_temp--;
					task_list.down = 0;
				}
				if(task_list.set){

					eeprom_write(EEPROM_ADDRESS,_setting_temp);
					timer_restart(_shutdown_timer,SHUTDOWN_TIMEOUT);
					state = waiting;
					task_list.set = 0;
				}
				
				break;
			case pre_alarm: 
				led_shutdown_mode_off();
				state = alarm;
				break;
			case alarm:
				buzzer_on();
				led_show_value(temperature,ALARM_SYMBOL);
				if(task_list.set){
					state = ack_alarm;
					task_list.set = 0;
				}
				break;
			case ack_alarm:
				buzzer_off();
				led_show_value(temperature,ALARM_SYMBOL);
				if(temperature < _setting_temp){
					state = waiting;
					timer_restart(_shutdown_timer,SHUTDOWN_TIMEOUT);
				}
				if(task_list.set){
					state = setting;
					task_list.set = 0;
					led_shutdown_mode_off();
				}
				break;
		}
	}
	return 0;
}

void main_init(){
	cli();
	

	DDRD |= 1<<CS;
	PORTD |= 1<<CS;
	ACSR |=1<<ACD; // отключение компаратора
	led_init();
	timer_init_soft_timer();
	_chatter_timer = timer_create(CHATTER_PERIOD);
	_poll_timer = timer_create(POLL_TIMEOUT);
	_buzz_timer = timer_create(BUZZ_PERIOD);
	_shutdown_timer = timer_create(SHUTDOWN_TIMEOUT);
	buzzer_init();
	eeprom_init();
	//beep();
	sei();
}

void beep(){
	//PORTD |= 1<<BUZZER;
	//_delay_ms(100);
	//PORTD &= ~(1<<BUZZER);
}





