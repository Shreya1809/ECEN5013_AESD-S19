/**
 * @file bist.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "bist.h"
#include "logger.h"
#include "main.h"
#include "bbgled.h"
#include "mysignal.h"
#include "lightSensor.h"

int Test_LightSensor(void)
{
    int ret = 0;
    uint8_t data;
    ret = APDS9301_powerup();
    LOG_INFO(BIST_TASK, "Light Sensor(ID:0x%1x) powered up",data);
    ret += APDS9301_readIDreg(&data); 
    LOG_DEBUG(BIST_TASK,"Testing Light Sensor ID reg 0x%1x",data);
    if(ret)
    {
        return EXIT_FAILURE;
    }
    if((data & 0xF0) != 0x50)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void *bist_task(void *threadp)
{
    //signal_init();
    int ret;
    LOG_INFO(BIST_TASK,"Bist task thread spawned");
    ret = Test_LightSensor();
    if(!ret)
    {
        LOG_INFO(BIST_TASK,"Light Sensor connected and functional");   
    }
    else
    {
        LOG_ERROR(BIST_TASK,"Light Sensor disconnected and non functional"); 
        REDLEDON();   
    }
     
    sleep(2);
    LOG_INFO(BIST_TASK,"Bist task thread exiting");
    return NULL;
}