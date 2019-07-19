/*
 * softTimer.h
 *
 * Created: 27.05.2019 19:12:40
 *  Author: MIK
 */ 


#ifndef SOFTTIMER_H_
#include <stdlib.h>
#define SOFTTIMER_H_

#define TIMER_OUT 1
#define TIMER_OFF 0

typedef uint16_t *timer_t;

void timer_init_soft_timer();
uint8_t timer_create(uint16_t ms);
uint8_t timer_check(size_t timer);
void timer_restart(size_t timer, uint16_t ms);
void timer_stop(size_t timer);
//uchar getState(uchar timer);
//uchar getTime(uchar timer);

#endif /* SOFTTIMER_H_ */