/*
 * eeprom.h
 *
 * Created: 03.10.2019 19:50:26
 *  Author: MIK
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_
#include <stdint.h>

void eeprom_init();
uint8_t eeprom_read(uint8_t address);
void eeprom_write(uint8_t address, uint8_t data);


#endif /* EEPROM_H_ */