/**
 * @file tempSensor.c
 * @author Shreya Chakraborty
 * @brief TMP102 temperature register functions
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "tempSensor.h"
#include "myI2C.h"
#include "logger.h"
#include "main.h"


int TMP102_getTemperature(float *temp_val)
{
    uint8_t tempbuff[2] = {0};
    uint8_t MSB, LSB;
    int ret = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS , TMP102_TEMP_REG, tempbuff, sizeof(tempbuff));
    if(ret == -1 || ret == 1)
    {
        return ret;
    }
    uint16_t result = 0;
    MSB = tempbuff[0];
    LSB = tempbuff[1];
    result = ((((uint16_t)MSB << 8) | LSB) >> 4) & 0xFFF;
    result &= 0xFFF;
    if(result & 0x800) //check for negative temperature
    {
        result = ((~result) + 1) & 0xFFF; 
        *temp_val = (-1) * result * 0.0625;
    }
    else{
        *temp_val = result * 0.0625;
    }
    return EXIT_SUCCESS;
}

/* what is write pointer register?*/
// read tlow register
int TMP102_getTlow(float *tlow_val)
{
    uint16_t result = 0;
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS ,TMP102_TLOW_REG, (uint8_t*)&result,sizeof(ret1));
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    }
    if(result & 0x800) //check for negative temperature
    {
        result = ((~result) + 1); 
        *tlow_val = (-1) * (float)result;
    }
    else{
        *tlow_val = (float)result;
    }
    return EXIT_SUCCESS;  
}

int TMP102_getThigh(float *thigh_val)
{
    uint16_t result = 0;
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS ,TMP102_THIGH_REG, (uint8_t*)&result,sizeof(uint16_t));
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    }
    if(result & 0x800) //check for negative temperature
    {
        result = ((~result) + 1); 
        *thigh_val = (-1) * (float)result;
    }
    else{
        *thigh_val = (float)result;
    }
    return EXIT_SUCCESS;     
}

int TMP102_setThigh(float thigh_val) // default value is 80 deg celcius 
{
    uint16_t result = 0;
    thigh_val = thigh_val/0.0625;
    //get raw temp value from the parameter and check for negative values
    if(thigh_val > 0)
    {
        result = ((uint16_t)thigh_val >> 4);// tlow is 12 bits in normal mode.
    }
    else
    {
        thigh_val = -1 * thigh_val;
        result = (uint16_t)thigh_val;
        result = ((~result) + 1); 
        result >>= 4;
    }

    int ret1 = I2C_write_word(&i2c_handler,TMP102_SLAVE_ADDRESS ,TMP102_THIGH_REG,result);
    if(ret1)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int TMP102_setTlow(float tlow_val) //default value is 75 deg celcius
{
    uint16_t result = 0;
    tlow_val = tlow_val/0.0625;
    //get raw temp value from the parameter and check for negative values
    if(tlow_val > 0)
    {
        result = ((uint16_t)tlow_val >> 4);// tlow is 12 bits in normal mode.
        result &= 0x7FFF;
    }
    else
    {
        tlow_val = -1 * tlow_val;
        result = (uint16_t)tlow_val;
        result = ((~result) + 1); 
        result >>= 4;
    }

    int ret1 = I2C_write_word(&i2c_handler,TMP102_SLAVE_ADDRESS ,TMP102_TLOW_REG,result);
    if(ret1)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;

}

int TMP102_readReg(uint8_t reg, uint8_t *data)
{
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,reg,data, sizeof(data));
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TMP102_writePTRreg(uint8_t reg)
{
    int ret1 = I2C_write_byte(&i2c_handler,TMP102_SLAVE_ADDRESS,reg);
    if(ret1 != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TMP102_setShutdownMode(shutdown_mode_t option) //SD bit in config register default is DEFAULT
{
    uint16_t SD_mode = 0;
    //read the set value from the configuration register
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,(uint8_t*)&SD_mode,sizeof(SD_mode));
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    }
    if(option == POWERSAVING)
    {
        SD_mode = SD_mode | (uint16_t)TMP102_CONF_SD;
    }
    else //default, continuous mode
    {
        SD_mode = SD_mode & ~((uint16_t)TMP102_CONF_SD);
    }
    ret1 = I2C_write_word(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,SD_mode);
    if(ret1 != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


//sensor resolution R1/R0, option - 0 is default 12 bits
int TMP102_setResolution(sensor_resolution_t option) //default R0R1 set
{
    uint16_t resolution = (uint16_t)0;
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,(uint8_t*)&resolution,sizeof(resolution));
    
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    }
    if(option == R0R1_SET) // default resolution -RO and R1 set
    {
        resolution = resolution | (uint16_t)TMP102_CONF_R0 | (uint16_t)TMP102_CONF_R1;    
    }
    else if( option == R1_SET) // R1 set R0 clear
    {
        resolution &= ~(uint16_t)TMP102_CONF_R0;
        resolution |= (uint16_t)TMP102_CONF_R1;
    }
    else if(option == R0_SET) //RO set R1 clear
    {
        resolution &= ~(uint16_t)TMP102_CONF_R1;
        resolution |= (uint16_t)TMP102_CONF_R0;
    }
    else 
    {
        resolution &= ~(uint16_t)TMP102_CONF_R0;
        resolution &= ~(uint16_t)TMP102_CONF_R1;
        //do nothing
    }
    ret1 = I2C_write_word(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,resolution);
    if(ret1 != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}

int TMP102_getFaultbits(uint16_t *data) // default mode is F1 AND F0 clear
{
    uint16_t config = 0;
    int ret = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,(uint8_t*)&config, sizeof(uint16_t));
    if(ret == -1)
    {
        return EXIT_FAILURE;
    }
    *data = config & FAULTBITSMASK;
    return EXIT_SUCCESS;
}

int TMP102_setFaultbits(fault_setting_t option) //default is 1 fault
{
    uint16_t setting = 0;
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,(uint8_t*)&setting,sizeof(setting));
    
    if(option == SIX_FAULTS) // FO and F1 set
    {
        setting |= (uint16_t)TMP102_CONF_F0;
        setting |= (uint16_t)TMP102_CONF_F1; 
    }
    else if( option == FOUR_FAULTS) // F1 set F0 clear
    {
        setting &= (~(uint16_t)TMP102_CONF_F0);
        setting |= ((uint16_t)TMP102_CONF_F1);
    }
    else if(option == TWO_FAULTS) //FO set F1 clear
    {
        setting &= ~(uint16_t)TMP102_CONF_F1;
        setting |= (uint16_t)TMP102_CONF_F0;
    }
    else //(option == ONE_FAULT)
    {
        // nothing
        setting &= ~(uint16_t)TMP102_CONF_F0;
        setting &= ~(uint16_t)TMP102_CONF_F1;
    }
    //printf("settings :0x%x\n",setting);  
    ret1 = I2C_write_word(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,setting);
    if(ret1 != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TMP102_setEM(EM_mode_t option)// default is normal mode
{
    uint16_t EM_mode = 0;
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,(uint8_t*)&EM_mode,sizeof(EM_mode));
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    }
    if(option == EXTENDED)
    {  
        EM_mode = EM_mode | (uint16_t)TMP102_CONF_EM; 
    }
    else //default
    {
        EM_mode = EM_mode & ~((uint16_t)TMP102_CONF_EM);
    }
    ret1 = I2C_write_word(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,EM_mode);
    if(ret1 != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}

int TMP102_setCR(conversion_rate_t option)
{
    uint16_t CR_mode = 0;
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,(uint8_t*)&CR_mode,sizeof(CR_mode));
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    } 
    
    if(option == EIGHT_HZ) 
    {
        CR_mode = CR_mode | (uint16_t)TMP102_CONF_CR0 | (uint16_t)TMP102_CONF_CR1;    
    }
    else if( option == FOUR_HZ) 
    {
        CR_mode &= ~(uint16_t)TMP102_CONF_CR0;
        CR_mode |= (uint16_t)TMP102_CONF_CR1;
    }
    else if(option == ONE_HZ) 
    {
        CR_mode &= ~(uint16_t)TMP102_CONF_CR1;
        CR_mode |= (uint16_t)TMP102_CONF_CR0;
    }
    else // ONE_FOURTH_HZ
    {
        // nothing
        CR_mode &= ~(uint16_t)TMP102_CONF_CR0;
        CR_mode &= ~(uint16_t)TMP102_CONF_CR1;
    } 
    ret1 = I2C_write_word(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,CR_mode);
    if(ret1 != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS; 
}

int TMP102_readAL( uint8_t *bit)
{
    uint16_t AL = 0;
    *bit = 0;
    int ret1 = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS,TMP102_CONFIG_REG,(uint8_t*)&AL,sizeof(uint16_t));
    if(ret1 < 0)
    {
        return EXIT_FAILURE;
    } 
    *bit = ((AL & (uint16_t)TMP102_CONF_AL) >> 13);
    return EXIT_SUCCESS;
}

int TMP102_setAllDefault(){
    int ret = TMP102_setCR(FOUR_HZ);
    ret |= TMP102_setEM(NORMAL);
    ret |= TMP102_setFaultbits(ONE_FAULT);
    ret |= TMP102_setResolution(R0R1_SET);
    ret |= TMP102_setShutdownMode(DEFAULT);
    if (ret != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
