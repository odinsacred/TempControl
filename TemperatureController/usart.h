/*
 * usart.h
 *
 * Created: 10.07.2019 21:03:58
 *  Author: MIK
 */ 


#ifndef USART_H_
#define USART_H_
#include <stdio.h>


void usart_init(uint16_t baud);
void usart_write(uint8_t* data, size_t length);

#endif /* USART_H_ */