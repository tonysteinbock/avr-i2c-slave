/*
 * i2cslave.c
 *
 * Created: 11.05.2017 21:12:33
 *  Author: Tony
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "i2cslave.h"

void i2c_init_slave(uint8_t addr)
{
	// setup i2c slave addr in TWI register
	TWAR = (addr << 1);
	// setup TWCR register: , enable addr matching, enable TWI, clear TWINT, interrupt 
	TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
	// enable interrupt
	sei();
}

void i2c_stop_slave(void)
{
	// clear ack and enable bits
	TWCR &= ~((1<<TWEA) | (1<<TWEN));
}

ISR(TWI_vect)
{
	uint8_t rx_data=0;
	
	// addr ack
	if((TWSR & 0xF8) == TW_SR_SLA_ACK)
	{
		// set register buffer index to end
		i2c_reg_addr = 0xff;
		// clear TWI interrupt flag, ack for recv next byte
		TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	}
	
	// handle rx data in rx mode
	else if((TWSR & 0xF8) == TW_SR_DATA_ACK)
	{		
		rx_data = TWDR;
		
		// handle rx data
		if(i2c_reg_addr == 0xff)
		{						
			// check and set i2c register addr, 2nd byte is register addr
			if(rx_data < I2C_REG_SIZE)
			{
				i2c_reg_addr = rx_data;
			} 
			else 
			{
				// error routine
				// set register addr to start
				i2c_reg_addr = 0x00;
			}
			
			// TWCR ACK
			TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		} 
		else 
		{
			// handle rx user data			
			if(i2c_reg_addr < I2C_REG_SIZE) {
				i2cdata[i2c_reg_addr] = rx_data;
				i2c_reg_addr++;
				
				// TWCR ACK
				TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
			} 
			else
			{
				// rx last byte, no ack, clear TWI interrupt flag
				TWCR |= (1<<TWIE) | (1<<TWINT) | (0<<TWEA) | (1<<TWEN); 
			}
		}
	}
	
	// handle slave as transmitter
	else if((TWSR & 0xF8) == TW_ST_DATA_ACK)
	{
		rx_data = TWDR;
		
		// check data register to transmit
		if(i2c_reg_addr == 0xff)
		{
			if(rx_data < I2C_REG_SIZE)
			{
				i2c_reg_addr = rx_data;
			} 
			else
			{
				// error routine
				// set register addr to start
				i2c_reg_addr = 0x00;
			}
		}
		
		TWDR = i2cdata[i2c_reg_addr];
		i2c_reg_addr++;
		
		// handle ack
		if(i2c_reg_addr < I2C_REG_SIZE){
			// clear TWI interrupt flag, ack
			TWCR |= (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		}
		else
		{
			// clear TWI interrupt flag, no ack
			TWCR |= (1<<TWIE) | (1<<TWINT) | (0<<TWEA) | (1<<TWEN);
		}		
	}
	
	// default handle
	else
	{
		TWCR |= (1<<TWIE) | (1<<TWEA) | (1<<TWEN);
	}
	
}