/*
 * i2cslave.h
 *
 * Created: 11.05.2017 21:13:00
 *  Author: Tony
 */ 

#ifndef I2CSLAVE_H_
#define I2CSLAVE_H_

#define I2C_REG_SIZE 5
volatile uint8_t i2cdata[I2C_REG_SIZE];
volatile uint8_t i2c_reg_addr;

void i2c_init_slave(uint8_t addr);
void i2c_stop_slave(void);
ISR(TWI_vect);

#if (I2C_REG_SIZE > 254)
#error buffer of max. 254 bytes valid
#endif
#if (I2C_REG_SIZE < 2)
#error buffer must be min. 2 bytes
#endif

#endif /* I2CSLAVE_H_ */