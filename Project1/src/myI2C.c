/**
 * @file myI2C.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-22
 * @reference https://iotdk.intel.com/docs/master/mraa/i2c_8h.html#a935c1206dfd241e182de7fe133aadb18
 * @copyright Copyright (c) 2019
 * 
 */

/* mraa header */
#include "includes.h"
#include "myI2C.h"
#include <errno.h>

int I2C_init(i2c_struct_t *i2c_handler)
{
     /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();
    i2c_handler->i2c_context = mraa_i2c_init_raw(I2C_BUS);
    if (i2c_handler->i2c_context == NULL) {
        perror("Failed to initialize I2C");
        mraa_deinit();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int I2C_close(i2c_struct_t *i2c_handler)
{
    i2c_handler->status = mraa_i2c_stop(i2c_handler->i2c_context);
    if(i2c_handler->status != MRAA_SUCCESS)
    {
        perror("Failed to stop I2C");
        return EXIT_FAILURE;   
    }
    mraa_deinit();
    return i2c_handler->status;
}

int I2C_set_slave(i2c_struct_t *i2c_handler,uint8_t slave_addr)
{
    i2c_handler->status = mraa_i2c_address(i2c_handler->i2c_context,slave_addr);
    if(i2c_handler->status != MRAA_SUCCESS)
    {
        perror("Could not set Slave address");
        return EXIT_FAILURE;
    }
    return i2c_handler->status;
}

int I2C_write_byte(i2c_struct_t *i2c_handler,uint8_t slave_addr,uint8_t data)
{
    if(! I2C_set_slave(i2c_handler,slave_addr))
    {
        i2c_handler->status = mraa_i2c_write_byte(i2c_handler->i2c_context, data);
        if(i2c_handler->status != MRAA_SUCCESS)
        {
            perror("Could not write data to Slave address");
            return EXIT_FAILURE;
        }
        else return i2c_handler->status;
    }
    else return EXIT_FAILURE;    
}

int I2C_read_byte(i2c_struct_t *i2c_handler,uint8_t slave_addr,uint8_t command_reg, uint8_t *data)
{
    int ret;
    if(! I2C_set_slave(i2c_handler,slave_addr))
    {
        ret = mraa_i2c_read_byte_data(i2c_handler->i2c_context,command_reg);
        if(ret == -1)
        {
            return EXIT_FAILURE;
        }
        else {
            *data = (uint8_t)ret;
            return EXIT_SUCCESS;
        }
    }
    else return EXIT_FAILURE; 
	    
}

int I2C_read_bytes(i2c_struct_t *i2c_handler,uint8_t slave_addr,uint8_t command_reg, uint8_t *data, size_t length)
{
    int ret;
    if(! I2C_set_slave(i2c_handler,slave_addr))
    {
        ret = mraa_i2c_read_bytes_data(i2c_handler->i2c_context, command_reg, data,length);
        if(ret != length)
        {
            perror("Could not read data bytes from register");
            return EXIT_FAILURE;
        }
        else return ret;
    }
    else return EXIT_FAILURE;  
}


//msb byte2 followed by lsb byte1 format
int I2C_write_byte_data(i2c_struct_t *i2c_handler,uint8_t slave_addr,uint8_t command_reg,uint8_t data)
{
    if(! I2C_set_slave(i2c_handler,slave_addr))
    {
        i2c_handler->status = mraa_i2c_write_byte_data(i2c_handler->i2c_context, data, command_reg);
        if(i2c_handler->status != MRAA_SUCCESS)
        {
            perror("Could not write data to Slave address");
            return EXIT_FAILURE;
        }
        else return i2c_handler->status;
    }
    else return EXIT_FAILURE;      
}

//msb byte2 followed by lsb byte1 format
int I2C_write_word(i2c_struct_t *i2c_handler,uint8_t slave_addr,uint8_t command_reg,uint16_t data)
{
    if(! I2C_set_slave(i2c_handler,slave_addr))
    {
        i2c_handler->status = mraa_i2c_write_word_data(i2c_handler->i2c_context, data,command_reg);
        if(i2c_handler->status != MRAA_SUCCESS)
        {
            perror("Could not write data to Slave address");
            return EXIT_FAILURE;
        }
        else return i2c_handler->status;
    }
    else return EXIT_FAILURE;      
}