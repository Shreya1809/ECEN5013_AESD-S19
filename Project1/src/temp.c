/**
 * @file temp.c
 * @author your name (you@domain.com)
 * @brief Temperature task thread functionality
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
#include "bist.h"

static volatile float temperature_val = 0.0;
static volatile float tlow_val = 75.0;
static volatile float thigh_val = 80.0;
static int t_sec = 1;
static int t_nsec = 0;
static sig_atomic_t stop_thread_temp = 0;
int t_count = 0;
pthread_mutex_t temp_var_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Set the Temp Var object "temp" as the value of temperature
 * 
 * @param temp 
 */
static inline void setTempVar(float temp)
{
    pthread_mutex_lock(&temp_var_lock);
    temperature_val = temp;
    pthread_mutex_unlock(&temp_var_lock);
} 

/**
 * @brief Get the Temp Var object "temp" from the actual global
 * temperature value variable
 * 
 * @return float 
 */
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
    LOG_DEBUG(TEMP_TASK,"Temperature task exit signal received");
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

int RemoteThresholdValues(float flow,float fhigh)
{
    pthread_mutex_lock(&temp_var_lock);
    tlow_val = flow;
    thigh_val = fhigh;
    pthread_mutex_unlock(&temp_var_lock); 
    return 0;
}
int TMP102_setTempThreshold(float tlow_val,float thigh_val)
{
    uint8_t bit;
    TMP102_setThigh(thigh_val);
    TMP102_setTlow(tlow_val);
    TMP102_setResolution(R0R1_SET);
    TMP102_setFaultbits(TWO_FAULTS);
    TMP102_readAL(&bit);
    LOG_INFO(TEMP_TASK,"thresholds are %f and %f",tlow_val,thigh_val);
    if(bit == 0)
    {
        GREENLEDOFF();
        REDLEDON();
        LOG_WARN(TEMP_TASK,"Current Temperature has exceeded the upper threshold");
    }
    else
    {
        REDLEDOFF();
        GREENLEDON();
    }
    return 0;
    
}


static int readAndUpdateTemp(void)
{
    float temp_val = 0.0 ;
    if(t_count > 4)
    {
        stop_thread_temp = 1;   
    }
    int ret = TMP102_getTemperature(&temp_val);
    if(ret)
    {
        LOG_ERROR(TEMP_TASK, "TMP102 Temperature Sensor disconnected: %d", ret);
        t_count++;
        return ret;
    }
    setTempVar(temp_val);    
    LOG_INFO(TEMP_TASK,"The temperature is %f C",temp_val);
    TMP102_setTempThreshold(tlow_val,thigh_val);
    return 0;
}

static void giveSemSensor(union sigval no)
{
    sem_post(&temp_sem);
}

void *temp_task(void *threadp)
{
    sem_init(&temp_thread_sem,0,0);
    usleep(1000);
    sem_wait(&temp_thread_sem);
    usleep(1000);
    if(!CheckBistResult())
    {
        goto exit;
    }
    LOG_INFO(TEMP_TASK,"Temperature Task thread spawned");
    I2C_init(&i2c_handler);
    timer_t temp_timer;
    sem_init(&temp_sem,0,0);
    if(maketimer(&temp_timer, &giveSemSensor) != 0)
    {
        perror("MakeTimer fail");
    }
    LOG_DEBUG(TEMP_TASK,"The thread frequency is %d sec and %d nsec",t_sec,t_nsec);
    startTimer(temp_timer,t_sec,t_nsec);
    while(!stop_thread_temp)
    {
        set_heartbeatFlag(TEMP_TASK);
        if(sem_wait(&temp_sem) == 0)
        {
            readAndUpdateTemp();
        }
    }        
exit:    
    LOG_INFO(TEMP_TASK,"Temp task thread exiting");
    return NULL;
}