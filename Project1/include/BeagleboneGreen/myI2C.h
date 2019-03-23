/**
 * @file myI2C.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-22
 * @Reference https://github.com/intel-iot-devkit/mraa/blob/master/examples/c/i2c_hmc5883l.c
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MYI2C_H_
#define MYI2C_H_
#include "mraa/i2c.h"
#define I2C_BUS  2

typedef struct
{
mraa_result_t status;
mraa_i2c_context i2c_context; 

}i2c_struct_t;

/**
 * @brief Initialise i2c context, passing in the i2c bus to use. 
 * 
 * @param i2c_handler 
 * @return int 
 */
int I2C_init(i2c_struct_t i2c_handler);
/**
 * @brief 
 * 
 * @param i2c_handler 
 * @return int 
 */
int I2C_close(i2c_struct_t i2c_handler);
/**
 * @brief set slave address for i2c read and write
 * 
 * @param i2c_handler 
 * @param slave_addr 
 * @return int 
 */
int I2C_set_slave(i2c_struct_t i2c_handler,uint8_t slave_addr);
/**
 * @brief write 1 byte to the given slave address
 * 
 * @param i2c_handler 
 * @param slave_addr 
 * @param data 
 * @return int 
 */
int I2C_write_byte(i2c_struct_t i2c_handler,uint8_t slave_addr,uint8_t data);
/**
 * @brief read 1 byte from the slave from the particular register
 * 
 * @param i2c_handler 
 * @param slave_addr 
 * @param command_reg 
 * @param data 
 * @return int 
 */
int I2C_read_byte(i2c_struct_t i2c_handler,uint8_t slave_addr,uint8_t command_reg, uint8_t *data);
/**
 * @brief read multiple bytes from the command register of the slave into data
 * 
 * @param i2c_handler 
 * @param slave_addr 
 * @param command_reg 
 * @param data 
 * @param length 
 * @return int 
 */
int I2C_read_bytes(i2c_struct_t i2c_handler,uint8_t slave_addr,uint8_t command_reg, uint8_t *data, size_t length);
/**
 * @brief write 2 bytes MSB first then lsb into the command register of the slave addr
 * 
 * @param i2c_handler 
 * @param slave_addr 
 * @param command_reg 
 * @param data 
 * @return int 
 */
int I2C_write_word(i2c_struct_t i2c_handler,uint8_t slave_addr,uint8_t command_reg,uint16_t data);
#endif