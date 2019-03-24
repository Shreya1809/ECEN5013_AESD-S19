/**
 * @file temp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * @reference https://github.com/jbdatko/tmp102/blob/master/tmp102.c
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
#include "i2c.h"

static volatile float temperature_val = 0.0;

pthread_mutex_t temp_var_lock = PTHREAD_MUTEX_INITIALIZER;

static inline void setTempVar(float temp)
{
    pthread_mutex_lock(&temp_var_lock);
    temperature_val = temp;
    pthread_mutex_unlock(&temp_var_lock);
} 

static inline float getTempVar()
{
    float temp = 0;
    pthread_mutex_lock(&temp_var_lock);
    temp = temperature_val;
    pthread_mutex_unlock(&temp_var_lock);
    return temp;
} 

float getTemperature(temp_unit unit)
{
    float temp_val = getTempVar();
    if(unit == KELVIN)
    {
        temp_val = (temp_val+273.15);
    }
    else if(unit == FARENHEIT)
    {
        temp_val = ((temp_val*9/5)+32);
    }
    return temp_val;
}

static int readAndUpdateTemp(void)
{
    float temp_val = 0.0 ;
    
    int ret = TMP102_getTemperature(&temp_val);
    if(ret)
    {
        LOG_ERROR(TEMP_TASK, "TMP102 Get Temperature: %d", ret);
        return ret;
    }

    setTempVar(temp_val);    
    LOG_INFO(TEMP_TASK,"The temperature is %f C",temp_val);
    return 0;
}

static void giveSemSensor(union sigval no)
{
    sem_post(&temp_sem);
}

void *temp_task(void *threadp)
{
    I2C_init(&i2c_handler);
    LOG_INFO(TEMP_TASK,"Temperature Task thread spawned");
    timer_t temp_timer;
    sem_init(&temp_sem,0,0);
    if(maketimer(&temp_timer, &giveSemSensor) != 0)
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
            readAndUpdateTemp();
        }
    }
    LOG_INFO(TEMP_TASK,"Temp task thread exiting");
    return NULL;
}