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
    LOG_INFO(TEMP_TASK,"Temperature Task thread spawned");
    return NULL;
}