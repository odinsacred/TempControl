/*
 * _7segLed.h
 *
 * Created: 04.05.2019 18:27:40
 *  Author: MIK
 */ 


#ifndef SEGLED_H_
#define SEGLED_H_
#include <stdint.h>

 void led_init();
 void led_show_value(uint8_t value, char symbol);
#endif /* 7SEGLED_H_ */