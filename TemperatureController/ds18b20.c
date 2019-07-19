/*
 * ds18b20.c
 *
 * Created: 04.05.2019 17:11:19
 *  Author: MIK
 */ 

 #define F_CPU 4000000UL 

 #include "ds18b20.h"

 #define PORT PORTB
 #define DDR DDRB
 #define PIN PINB
 #define BIT 0

#define SKIP_ROM        0xCC
#define TEMP_MEASURE    0x44
#define READ_SCRATCHPAD 0xBE


 #define sbit(x,PORT) (PORT) |= (1<<x)
 #define cbit(x,PORT) (PORT) &= ~(1<<x)
 #define pin(x,PIN) (PIN) & (1<<x)

 #define DQ_OUT DDR|=1<<BIT
 #define DQ_IN  DDR&=~(1<<BIT)
 #define S_DQ   sbit(0,PORTB)
 #define C_DQ   cbit(0,PORTB)
 #define DQ     pin(0,PINB)
 
 static uint8_t ds18b20_read_byte(void);
 static void ds18b20_write_command(uint8_t);
 static uint8_t ds18b20_read_temp(void);
 static uint16_t t = 0;
 //static uint8_t ds18b20_check_sum(uint8_t data[], uint8_t length);
 #define MEMORY_LENGTH 9
 

// Start transaction with 1-wire line.
void ds18b20_init(void)
{ 
	DQ_OUT;
	C_DQ ;
	_delay_us(600);
	S_DQ;
	_delay_us(50);
	DQ_IN;
	while(DQ);
	_delay_us(240);
	DQ_OUT;
	S_DQ;
	_delay_us(300);
}


// Read a byte from the sensor
uint8_t ds18b20_read_byte(void)
{ 
	uint8_t i = 0,data = 0;
	DQ_OUT;
	for (i=8; i>0; --i)
	{ C_DQ ;
		data >>= 1;
		_delay_us(3);
		S_DQ;
		DQ_IN;
		_delay_us(12);
		if(DQ)
		data |= 0x80;
		DQ_OUT;
		S_DQ;
		_delay_us(45);
		_delay_us(5);
	}
	return(data);
}

// Write a command to the sensor
void ds18b20_write_command(uint8_t data)
{ uint8_t  i;
	for(i=8; i>0; --i)
	{ C_DQ;
		_delay_us(15);
		if(data & 0x01)
		S_DQ;
		else
		C_DQ;
		_delay_us(45);
		data >>= 1;
		S_DQ;
		_delay_us(2);
	}
}

uint8_t ds18b20_get_temp(){	
	return ds18b20_read_temp();
}
//≈сли разместить массив локально - количество зан€той пам€ти программ увеличиваетс€
//≈сли разместить переменную локально - количество зан€той пам€ти программ уменьшаетс€
uint8_t memory[MEMORY_LENGTH];

// Read value from the sensor
inline uint8_t ds18b20_read_temp(void)
{ 
    

	ds18b20_init();
	// Convert
	ds18b20_write_command(SKIP_ROM);
	ds18b20_write_command(TEMP_MEASURE);
	ds18b20_init();
	// Read Scratch memory area
	ds18b20_write_command(SKIP_ROM);
	ds18b20_write_command(READ_SCRATCHPAD);
	memory[0] = ds18b20_read_byte(); //LSB
	memory[1] = ds18b20_read_byte(); //MSB
	memory[2] = ds18b20_read_byte();
	memory[3] = ds18b20_read_byte();
	memory[4] = ds18b20_read_byte();
	memory[5] = ds18b20_read_byte();
	memory[6] = ds18b20_read_byte();
	memory[7] = ds18b20_read_byte();
	memory[8] = ds18b20_read_byte();
	if(crc_8_checkSum(memory,MEMORY_LENGTH)==0){
		t = memory[1];
		t <<= 8;
		t = t|memory[0];
		t>>=4;
		return(t);
	}
	return(t);
}
