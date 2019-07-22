/*
* CFile1.c
*
* Created: 04.05.2019 18:20:16
*  Author: MIK
*/
#include <avr/io.h>
#define F_CPU 4000000UL
#include <util/delay.h>
#include "7segLed.h"


#define CS		 PD2
#define INFO_POS 4

void led_shutdown_mode_on();
void led_shutdown_mode_off();
void led_display_test();
void led_display_test_off();
static void led_decode_mode_on();
void led_decode_mode_off();
static void led_set_scan_limit();
static void led_show_symbol(char code);
void led_init();
static void led_show_digit(uint8_t digit, uint8_t pos);
void led_show_value(uint8_t value, char symbol);
void led_show_load();


void spi_init(){
	DDRB |= (1<<PB6 | 1<<PB7);
	DDRB&=~(1<<PB5);
	PORTB&=~(1<<PB6 | 1<<PB7);
}

void spi_send_byte(uint8_t b){
	USIDR = b;
	USISR |= (1<<USIOIF);//сбрасываем флаг
	while(!(USISR & (1<<USIOIF)))//пока нет флага окончания передачи
	{
		USICR |= (1<<USIWM0) | (1<<USICS1) | (1<<USICLK) | (1<<USITC);//формируем тактирующие импульсы
		_delay_us(5);
	}
}

void spi_send_packet(uint8_t addr, uint8_t data){
	PORTD &= ~(1<<CS);
	spi_send_byte(addr);
	spi_send_byte(data);
	PORTD |= 1<<CS;
}

//void blink(){
//shutdownModeOn();
////_delay_ms(200);
//shutdownModeOff();
//}

void led_shutdown_mode_on(){
	spi_send_packet(0b00001100,0b00000000); //shutdown on
}

void led_shutdown_mode_off(){
	spi_send_packet(0b00001100,0b00000001); //shutdown off
}

void led_display_test(){
	spi_send_packet(0b00001111, 0b00000001);
}

void led_display_test_off(){
	spi_send_packet(0b00001111, 0b00000000);
}

void led_decode_mode_on(){
	spi_send_packet(0b00001001, 0b00000111);
}

void led_decode_mode_off(){
	spi_send_packet(0b00001001, 0b00000000);
}

void led_set_scan_limit(){
	spi_send_packet(0b00001011, 0b00000011);
}

void led_show_symbol(char code){
	spi_send_packet(INFO_POS, code);
}


void led_init(){
	spi_init();
	led_decode_mode_on();
	led_set_scan_limit();
	led_shutdown_mode_off();
}

void led_show_digit(uint8_t digit, uint8_t pos){
	spi_send_packet(pos, digit);
}


void led_show_value(uint8_t value, char symbol){
	uint8_t hundreds = value/100;
	uint8_t decs = (value - (hundreds*100))/10;
	uint8_t units = (value - (hundreds*100) - decs*10);
	led_show_digit(hundreds,1);
	led_show_digit(decs,2);
	led_show_digit(units,3);
	led_show_symbol(symbol);
}

