/*
 * keyboard.h
 *
 * Created: 21.07.2019 23:03:41
 *  Author: MIK
 */ 


#ifndef KEYBOARD_H_
#define KEYBOARD_H_
 #include <avr/io.h>
 #include <stdint.h>

 typedef struct tasks* task_t;

 struct tasks{
	unsigned int up : 1;
	unsigned int down : 1;
	unsigned int set : 1;
}tasks;

void keyboard_refresh(task_t task);
#endif /* KEYBOARD_H_ */