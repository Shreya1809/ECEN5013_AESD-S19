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
#include "heartbeat.h"

static volatile float temperature_val = 0.0;
static int stop_thread_temp = 0;
int t_count = 0;
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

void kill_temp_thread(void)
{
    stop_thread_temp = 1;    
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
    if(temp_val == 0)
    {
        t_count++;
    }
    if(t_count > 4)
    {
        stop_thread_temp = 1;   
    }
    return 0;
}

static void giveSemSensor(union sigval no)
{
    sem_post(&temp_sem);
}

void *temp_task(void *threadp)
{
    //printf("hello1\n");
    sem_init(&temp_thread_sem,0,0);
    //printf("hello2\n");
    usleep(1000);
    sem_wait(&temp_thread_sem);
    usleep(1000);
    //printf("hello3\n");
    LOG_INFO(TEMP_TASK,"Temperature Task thread spawned");
    I2C_init(&i2c_handler);
    timer_t temp_timer;
    sem_init(&temp_sem,0,0);
    if(maketimer(&temp_timer, &giveSemSensor) != 0)
    {
        perror("MakeTimer fail");
    }
    startTimer(temp_timer);
    while(!stop_thread_temp)
    {
        set_heartbeatFlag(TEMP_TASK);
        if(sem_wait(&temp_sem) == 0)
        {
            readAndUpdateTemp();
        }
    }        
    LOG_INFO(TEMP_TASK,"Temp task thread exiting");
    return NULL;
}