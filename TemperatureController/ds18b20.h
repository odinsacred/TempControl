/*
* ds18b20.h
*
* Created: 04.05.2019 17:15:48
*  Author: MIK
*/


#ifndef DS18B20_H_
#define DS18B20_H_
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "crc_8_dallas.h"

void ds18b20_init(void);
uint8_t ds18b20_get_temp();

#endif /* DS18B20_H_ */