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