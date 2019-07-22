/*
 * buzzer.c
 *
 * Created: 22.07.2019 21:23:18
 *  Author: MIK
 */ 

  #include <avr/io.h>
  #include <stdint.h>

  #define BUZZER PB4

  void buzzer_init(){
  	DDRB |=1<<BUZZER;
  	PORTB = 0x00;
	GTCCR |= 1<<PSR10; // סבנמס ןנוההוכטעוכÿ
	TCNT1 = 0;
	OCR1A= 99;
	
  }

  void buzzer_on(){
	TCCR1A |= 1<<COM1B0;
	TCCR1B |=1<<CS10|1<<CS11|0<<CS12|1<<WGM12;

  }

  void buzzer_off(){
  TCCR1A &= ~(1<<COM1B0);
	TCCR1B &=~(1<<CS10|1<<CS11|1<<WGM12);
  }