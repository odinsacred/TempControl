/*
* keyboard.c
*
* Created: 21.07.2019 22:53:08
*  Author: MIK
*/
#include "keyboard.h"

#define UP 2
#define DOWN 4
#define SET 8

#define STATE_SCAN 0
#define STATE_FIX 1

uint8_t buttons = 0;

task_t _task;
uint8_t _keyboard_state = STATE_SCAN;

void keyboard_refresh(task_t task){
	switch(_keyboard_state){
		case STATE_SCAN:
		if(PINB & (1<<PINB1)){
			buttons  |= UP;
			_keyboard_state= STATE_FIX;
		}
		if(PINB & (1<<PINB2)){
			buttons  |= DOWN;
			_keyboard_state= STATE_FIX;
		}
		if(PINB & (1<<PINB3)){
			buttons  |= SET;
			_keyboard_state= STATE_FIX;
		}
		break;
		case STATE_FIX:
			if(buttons & UP){
				task->up = 1;
				buttons  &=~UP;
				}
			if(buttons & DOWN){
				buttons  &=~DOWN;
				task->down = 1;
				}
			if(buttons & SET){
				buttons  &=~SET;
				task->set = 1;
				}
			_keyboard_state= STATE_SCAN;
		break;
	}
}
