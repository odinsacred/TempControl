/*
* usart.c
*
* Created: 10.07.2019 21:03:47
*  Author: MIK
*/
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "soft_timer.h"

#define XTAL 4000000
#define MAX_PORTS_AVAILABLE 1

#define HI(x) ((x)>>8)
#define LO(x) ((x)&0xFF)

void usart_0_init(uint16_t baud);
void usart_send_byte(uint8_t byte);


void usart_init(uint16_t baud){
	usart_0_init(baud); 
}

void usart_write(uint8_t* data, size_t length){
	for (uint8_t i = 0; i<length; i++)
	{
		usart_send_byte(*(data+i));
	}
}

void usart_send_byte(uint8_t byte){
	/* Wait for empty transmit buffer */
	while(!(UCSRA & (1<<UDRE)))
	;
	UDR = byte;
}

void usart_0_init(uint16_t baud){
	long bauddivider = XTAL/(16*baud)-1;
	UBRRL = LO(bauddivider);
	UBRRH = HI(bauddivider);
	UCSRA = 0;
	UCSRB = 1<<RXEN|1<<TXEN|1<<RXCIE|1<<TXCIE;
	/* Set frame format: 8data, 1stop bit */
	UCSRC = 1<<UCSZ0|1<<UCSZ1;
}

