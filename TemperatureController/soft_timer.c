/*
* softTimer.c
*
* Created: 27.05.2019 18:11:08
*  Author: MIK
*/
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "soft_timer.h"

#define XTAL 4000000
#define MAX_TIMERS 10

#define divider 64
#define ticks XTAL/divider

#define INIT 256 - (ticks/1000) // 1000 - чтобы получить 1 мс, 256 - число тиков таймера до переполнения
#define TIMER_ON 1
#define TIMER_OFF 0

#define INITIATED 1
#define NOT_INITIATED 0

static int current_timer = -1;
static uint8_t isInit = NOT_INITIATED;

struct timer{
	uint16_t time;
	uint8_t isSet; 
};

//если добавть static - перестает корректно отрисовываться дисплей
//хз почему, но глюк пропал
volatile static struct timer timers[MAX_TIMERS];

void timer_init_soft_timer()
{
	// настроить таймер на 1 мс тики
	// установить делитель на 64
	//
	uint8_t saveSreg = SREG;
	cli();
	if(isInit == NOT_INITIATED){
		for (size_t i = 0; i < MAX_TIMERS; i++){
			timers[i].isSet = TIMER_OFF;
			timers[i].time = 0;
		}

		DDRA=0xFF;
		GTCCR |= 1<<PSR10; // сброс предделителя
		TCNT0 = INIT;
		TCCR0B |=1<<CS00|1<<CS01|0<<CS02;
		TIMSK |= 1<<TOIE0; // разрешение прерывания
		
		isInit = INITIATED;
	}	
	SREG = saveSreg;
}

uint8_t timer_create(uint16_t ms)
{
	if (current_timer == MAX_TIMERS)
		return 0;
	uint8_t saveSreg = SREG;
	cli();	
	current_timer++;//еще одна странность: если эту строчку разместить в конце, то ничего не работает))
	timers[current_timer].time = ms;			
	timers[current_timer].isSet = TIMER_ON;
	SREG= saveSreg;
	return current_timer;
}

uint8_t timer_check(size_t timer)
{
	if(timers[timer].isSet == TIMER_ON && timers[timer].time == 0)
		return TIMER_OUT;
	else
		return 0;
}

void timer_restart(size_t timer, uint16_t ms)
{
	uint8_t saveSreg = SREG;
	cli();
	timers[timer].isSet = TIMER_ON;
	timers[timer].time = ms;
	SREG = saveSreg;
}

void timer_stop(size_t timer)
{
	uint8_t saveSreg = SREG;
	cli();
	timers[timer].isSet = TIMER_OFF;
	timers[timer].time = 0;
	SREG = saveSreg;
}

ISR(TIMER0_OVF_vect){
	TCNT0 = INIT;
	for (uint16_t i = 0; i <= current_timer; i++)
	{
		if(timers[i].isSet == TIMER_ON){
			if (timers[i].time == 0)
				continue;
			timers[i].time--;			
		}
	}
}