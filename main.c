/*
 * avr-i2c-slave.c
 * example app
 * turning led via i2c on and off
 * using register addr 0x00
 * Created: 11.05.2017 21:01:03
 * Author : Tony
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include "i2cslave.h"

#define I2C_SLAVE_ADDR 0x05

int main(void)
{
	// setup pins
	DDRB |= (1 << PB1);
	PORTB &= ~(1 << PB1);
	
    // init i2c slave
	i2c_init_slave(I2C_SLAVE_ADDR);
	
    while (1) 
    {
		
		if(i2cdata[0] == 0x01) {
			PORTB |= (1 << PB1);
		} else {
			PORTB &= ~(1 << PB1);
		}
    }
}

