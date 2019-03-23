/**
 * @file temp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "temp.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "mytimer.h"
#include "myI2C.h"
#include "tempSensor.h"
i2c_struct_t i2c_handler;

float getTemp(temp_unit unit)
{
    uint8_t tempbuff[2] = {0};
    uint8_t MSB, LSB;
    int ret = I2C_read_bytes(i2c_handler,TMP102_SLAVE_ADDRESS , TMP102_TEMP_REG, tempbuff, sizeof(tempbuff));
    if(ret == -1)
        return ret;

    uint16_t result = 0;
    MSB = tempbuff[0];
    LSB = tempbuff[1]
    result = ((MSB << 8) | LSB) >> 4;
    float i = result * 0.0625;
    float value = 0.0;
    
    if(unit == default_unit)
    {
        value = i;
    }
    if(unit == celcius)
    {
        value = i;
    }
    if(unit == kelvin)
    {
        value = (i+273.15);
    }
    if(unit == Farenheit)
    {
        value = ((i*9/5)+32);
    }
    return value;
}

void *temp_task(void *threadp)
{
    //signal_init();
    I2C_init(i2c_handler);
    LOG_INFO(TEMP_TASK,"Temperature Task thread spawned");
    timer_t temp_timer;
    if(maketimer(&temp_timer) != 0)
    {
        perror("MakeTimer fail");
    }
    startTimer(temp_timer);
    //int i = 0;
    //while(!done)
    while(1)
    {
        if(sem_wait(&temp_sem) == 0)
        {
            LOG_INFO(TEMP_TASK,"The temp is %f",getTemp(0));
        }
    }
    LOG_INFO(TEMP_TASK,"Temp task thread exiting");
    return NULL;
}