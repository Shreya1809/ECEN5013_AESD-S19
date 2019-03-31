/**
 * @file lightSensor.c
 * @author Shreya Chakraborty
 * @brief APDS9301 sensor register functions
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "lightSensor.h"
#include "myI2C.h"
#include "logger.h"

int APDS9301_getCh0(uint16_t *data)
{
    int ret1,ret2;
    uint8_t datalow, datahigh;
    ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS, APDS9301_CH0_DATALOW_REG, &datalow);
    ret1 += I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS, APDS9301_CH0_DATAHIGH_REG, &datahigh);
    if(ret1)
    {
        LOG_ERROR(LIGHT_TASK,"APDS getch0 failed");
        return EXIT_FAILURE;
    }
    *data = (((uint16_t)datahigh) << 8) | datalow ;
    LOG_DEBUG(LIGHT_TASK,"Value of ch0 is %d",*data);
    return EXIT_SUCCESS;
}

int APDS9301_getCh1(uint16_t *data)
{
    int ret1;
    uint8_t datalow, datahigh;
    ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS, APDS9301_CH1_DATALOW_REG, &datalow);
    ret1 += I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS, APDS9301_CH1_DATAHIGH_REG, &datahigh);
    if(ret1)
    {
        LOG_ERROR(LIGHT_TASK,"APDS getch1 failed");
        return EXIT_FAILURE;
    }
    *data = (((uint16_t)datahigh) << 8) | datalow ;
    LOG_DEBUG(LIGHT_TASK,"Value of ch1 is %d",*data);
    return EXIT_SUCCESS;

}

int APDS9301_getlight(float *lux)
{
    uint16_t ch1_data , ch0_data;
    float val = 0.0;
    int ret1 = 0;
    ret1 = APDS9301_getCh0(&ch0_data);
    ret1 += APDS9301_getCh1(&ch1_data);
    if(ret1)
    {
        LOG_ERROR(LIGHT_TASK,"getLight failed");
        return EXIT_FAILURE;
    }
    //LOG_DEBUG(LIGHT_TASK,"CH0 val is %d",ch0_data);
    //LOG_DEBUG(LIGHT_TASK,"CH1 val is %d",ch1_data);
    if(!(ch0_data))
    {
        LOG_ERROR(LIGHT_TASK,"channel 0 val is 0");
        return EXIT_FAILURE;
    }

    val = (float)ch1_data/(float)ch0_data;   
    if(val > 0 && val <= 0.50)
    {
        *lux = (0.0304*ch0_data) - (0.062*ch0_data*(pow(val, 1.4)));
    }
    else if(val > 0.50 && val <= 0.61)
    {
        *lux = (0.0224*ch0_data) - (0.031*ch1_data);
    }
    else if(val > 0.61 && val <= 0.80)
    {
        *lux = (0.0128*ch0_data) - (0.0153*ch1_data);
    }
    else if(val > 0.80 && val <= 1.30)
    {
        *lux = (0.00146*ch0_data) - (0.00112*ch1_data);
    }
    else //val > 1.30
    {
        *lux = 0.0;
    }
    return EXIT_SUCCESS;
}

int APDS9301_powerup()
{
    int ret1;
    ret1 = I2C_write_byte_data(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_CONTROL_REG,APDS9301_CONTROL_POWER_UP);
    if(ret1)
    {
        return EXIT_FAILURE;
    }
    uint8_t data = 0;
    ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_CONTROL_REG, &data);
    LOG_DEBUG(LIGHT_TASK, "Power Up state:0x%x",data);
    if(ret1 || (data & 0x03) != 0x03)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int APDS9301_intClear(void)
{
    int ret1;
    ret1 = I2C_write_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_CONTROL_REG | APDS9301_COMMAND_INT_CLEAR);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;   
}

int APDS9301_CheckInt(uint8_t *data)
{
    int ret1;
    ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_CONTROL_REG,data);
    *data &= (APDS9301_COMMAND_INT_CLEAR);
    *data = *data >> 6;
    if(*data)
    {
        printf("interrupt cleared\n");
    }
    else
    {
        printf("pending interrupt");
    }
    
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;   
}
int APDS9301_readIDreg(uint8_t *data)
{
    int ret1;
    ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_ID_REG,data);
    if(ret1)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int APDS9301_writeCMDreg(uint8_t data) // also write command register
{
    int ret1;
    ret1 = I2C_write_byte_data(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_CONTROL_REG,data);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;  
}

int APDS9301_readCTRLreg(uint8_t *data)
{
    int ret1;
    ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_CONTROL_REG,data);
    if(ret1)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int APDS9301_setTiming_gain(gain_mode_t gain) //default value is low
{
    uint8_t result;
    int ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_TIMING_REG,&result);
    if(ret1)
    {
        return EXIT_FAILURE;
    }
    
    if(gain == LOW)
    {
        result &= ~((uint8_t)APDS9301_TIMING_GAIN);
    }
    else if (gain == HIGH) // gain is high
    {
        result |= (uint8_t)APDS9301_TIMING_GAIN;
    }
    else //error
    {
        return EXIT_FAILURE;
    }
    ret1 = I2C_write_byte_data(&i2c_handler, APDS9301_SLAVE_ADDRESS, APDS9301_TIMING_REG,result);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;  
}

int APDS9301_setTiming_integ(integ_time_t integTime) //mode 2 is default
{
    uint8_t result;
    int ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_TIMING_REG,&result);
    if(ret1)
    {
        return EXIT_FAILURE;
    }

    if(integTime == MODE_ZERO)
    {
        result &= ~((uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT0);
        result &= ~((uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT1);
    }
    else if(integTime == MODE_ONE)
    {
        result &= ~((uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT1);
        result |= (uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT0;
    }
    else if(integTime == MODE_TWO)
    {
        result |= (uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT1;
        result &= ~((uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT0);
    }
    else
    {
        result |= (uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT0;
        result |= (uint8_t)APDS9301_TIMING_INTEGRATION_TIME_BIT1;
    }
    ret1 = I2C_write_byte_data(&i2c_handler, APDS9301_SLAVE_ADDRESS, APDS9301_TIMING_REG,result);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;  
      
}

int APDS9301_interruptCTRLreg(interrupt_mode_t option) //default is disabled
{
    uint8_t result;
    int ret1 = I2C_read_byte(&i2c_handler,APDS9301_SLAVE_ADDRESS,APDS9301_INTERRUPT_REG,&result);
    if(ret1)
    {
        return EXIT_FAILURE;
    }

    if(option == ENABLE)
    {
        result |= ((uint8_t)APDS9301_INTERRUPT_CTRL_ENABLE_DISABLE | (0x1));
    }
    else
    {
        result &= ~((uint8_t)APDS9301_INTERRUPT_CTRL_ENABLE_DISABLE);    
    }
    ret1 = I2C_write_byte_data(&i2c_handler, APDS9301_SLAVE_ADDRESS, APDS9301_INTERRUPT_REG,result);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;   
}

int APDS9301_writeTHRESH_lowlow(uint16_t th_lowlow)
{
    int ret1 = I2C_write_word(&i2c_handler, APDS9301_SLAVE_ADDRESS,APDS9301_THRESHLOWLOW_REG | APDS9301_COMMAND_WORD_RW,th_lowlow);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;   
}
int APDS9301_readTHRESH_lowlow(uint8_t *th_lowlow)
{
    int ret1 = I2C_read_byte(&i2c_handler, APDS9301_SLAVE_ADDRESS,APDS9301_THRESHLOWLOW_REG,th_lowlow);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;    
}
int APDS9301_writeTHRESH_highlow(uint16_t th_hilow)
{
    int ret1 = I2C_write_word(&i2c_handler, APDS9301_SLAVE_ADDRESS,APDS9301_THRESHHILOW_REG | APDS9301_COMMAND_WORD_RW,th_hilow);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;     
}
int APDS9301_readTHRESH_highlow(uint8_t *th_hilow)
{
    int ret1 = I2C_read_byte(&i2c_handler, APDS9301_SLAVE_ADDRESS,APDS9301_THRESHHILOW_REG,th_hilow);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;     
}

int APDS9301_readTHRESH_highhigh(uint8_t *th_hihi)
{
    int ret1 = I2C_read_byte(&i2c_handler, APDS9301_SLAVE_ADDRESS,APDS9301_THRESHHIHI_REG,th_hihi);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;     
}

int APDS9301_readTHRESH_lowhigh(uint8_t *th_lowhi)
{
    int ret1 = I2C_read_byte(&i2c_handler, APDS9301_SLAVE_ADDRESS,APDS9301_THRESHLOWHI_REG,th_lowhi);
    if(ret1)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;     
}

int APDS9301_setAllDefault()
{
    int ret = APDS9301_setTiming_gain(LOW);
    ret |=  APDS9301_setTiming_integ(MODE_TWO);
    ret |= APDS9301_interruptCTRLreg(DISABLE);
    if(ret)
    {
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;  
}