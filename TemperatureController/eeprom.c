/*
 * eeprom.c
 *
 * Created: 03.10.2019 19:50:13
 *  Author: MIK
 */ 
 #include "eeprom.h"
 #include <avr/io.h>

 void eeprom_init(){
	//автоматический режим записи в EEPROM
	EECR &= ~(1<<EEPM0)|(1<<EEPM1);
 }


 uint8_t eeprom_read(uint8_t address){
	while(EECR & (1<<EEPE))
	;
	EEAR = address;
	EECR|=(1<<EERE);
	return EEDR;
 }

 void eeprom_write(uint8_t address, uint8_t data){
	// wait for completion of prev write op
	while(EECR & (1<<EEPE))
	;
	EEAR = address;
	EEDR = data;
	EECR|=(1<<EEMPE);
	//start writing to eeprom
	EECR|=(1<<EEPE);
 }