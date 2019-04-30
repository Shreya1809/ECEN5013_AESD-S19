/*
 * temp_task.c
 *
 *  Created on: Apr 8, 2019
 *      Author: Shreya
 */
#include "includes.h"
#include "TMP102.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "i2c.h"
#include "logger.h"

float TMP102_getTemperature(void)
{
    uint32_t temperature = I2CGet2Bytes(TMP102_SLAVE_ADDRESS, TMP102_TEMP_REG);
    temperature &= 0x00000FFF;
    float temp = 0.0;
    if(temperature & 0x00000800) //check for negative temperature
    {
        temperature = ((~temperature) + 1) & 0x00000FFF;
        temp = (-1) * (float)temperature * 0.0625;
    }
    else{
        //LOG_DEBUG(MAIN_TASK,NULL,"Temp raw float is %f",(float)temperature);
        temp = 0.0625 * (float)temperature;
        //LOG_DEBUG(MAIN_TASK,NULL,"Temp float is %f",temp);
    }

    return temp;
}

