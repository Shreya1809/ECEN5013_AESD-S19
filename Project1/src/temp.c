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
#include "i2c.h"


float getTemp(temp_unit unit)
{

    float i = 25.3;
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
    i2c_init();
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