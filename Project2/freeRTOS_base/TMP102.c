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


void DCmotor(state_t option)
{
    if(option)
    {
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_PIN_7);
    }
}

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
        temp = 0.0625 * (float)temperature;
    }

    return temp;
}

