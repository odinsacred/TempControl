/*
* TemperatureController.c
*
* Created: 02.05.2019 16:01:58
* Author : MIK
*/

#define F_CPU 4000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#define PCIE0 5

#define UP 2
#define DOWN 4
#define SET 8
#define RESET 16

#define UCSK     PB7
#define DI       PB6
#define DO       PB5
#define CS		 PD2

void spiInit();
char spiSend(char reg);
void sendByte(char b);
void init();
void initPinChangeInterrupts();
void displayTest();
void sendPacket(unsigned char addr, unsigned char data);

enum states{
	waiting,
	setting
};

struct {
	unsigned int up : 1;
	unsigned int down : 1;
	unsigned int set : 1;
	unsigned int reset : 1;
}tasks;

unsigned int temperature;
unsigned int temp;
enum states state;

volatile char s;
int main(void)
{
	state = waiting;
	temperature = 0;
	temp = 0;
	init();
	displayTest();
	while (1)
	{
		//displayTest();
		//sendByte(0b11101010);
		//_delay_ms(1000);
		//switch(state){
			//case waiting:
				//if(tasks.set){
					//temp = temperature;
					//state = setting;
				//}
				//break;
			//case setting:
				//if(tasks.up)
					//temp++;
				//if(tasks.down)
					//temp--;
				//if(tasks.reset)
					//temp = 0;
				//if(tasks.set){
					//temperature = temp;
					//state = waiting;
				//}
		//}

		
	}
}

void init(){
	DDRB = 0x1E;
	PORTB = 0;

	DDRD |= 1<<CS;
	PORTD |= 1<<CS;
	initPinChangeInterrupts();
	spiInit();
}


void initPinChangeInterrupts(){
	GIMSK |= 1<<PCIE0; 
	PCMSK |= 1<<PCINT1|1<<PCINT2|1<<PCINT3|1<<PCINT4;
	sei();
}

void spiInit(){
	DDRB |= (1<<PB4 | 1<<PB6 | 1<<PB7);
	DDRB&=~(1<<PB5);
	PORTB&=~(1<<PB4 | 1<<PB6 | 1<<PB7);
}

void sendByte(char b){
	USIDR = b;
	USISR |= (1<<USIOIF);//сбрасываем флаг
	while(!(USISR & (1<<USIOIF)))//пока нет флага окончания передачи
	{
		USICR |= (1<<USIWM0) | (1<<USICS1) | (1<<USICLK) | (1<<USITC);//формируем тактирующие импульсы
		_delay_us(5);
	}
}

void sendPacket(unsigned char addr, unsigned char data){	
	PORTD &= ~(1<<CS);
	sendByte(addr);
	sendByte(data);
	PORTD |= 1<<CS;
}

void displayTest(){
	char addr = 0b00001100;
	char mode = 0b00000001; 
	//sendPacket(0x09,0xFF); //decode mode on
	//sendPacket(0x0A,0xFF);
	//sendPacket(0x09,0xFF);
	//sendPacket(0x09,0xFF);
	sendPacket(addr,mode); //shutdown off
	//_delay_ms(100);
	addr = 0b00001111;
	mode = 0b00000001;
	sendPacket(addr,mode); // display test 
	//_delay_ms(100);
	addr = 0b00001001;
	mode = 0b11111111; //decode mode on
	sendPacket(addr,mode);
	//_delay_ms(100);
	addr = 0b00001011;
	mode = 0b00000011; //scan limit 0 1 2 3
	sendPacket(addr,mode);
	//_delay_ms(100);
	//addr = 0b00000010;
	//mode = 0b00000001;
	//sendPacket(addr,mode);
}

ISR(PCINT_vect){
	if(PINB & UP){
		tasks.up = true;
		displayTest();
		//spiSend(0xff);
		//PORTD = 0x4; 
	}
	if(PINB & DOWN){
		tasks.down = true;
		//PORTD = 0x8; 
	}
	if(PINB & SET){
		tasks.set = true;
		//PORTD = 0x10; 
	}
	if(PINB & RESET){
		tasks.reset = true;
		//PORTD = 0x20; 
	}
}
