#include "includes.h"
#include "light.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "mytimer.h"
#include "lightSensor.h"
#include "heartbeat.h"

static volatile float lux_val = 0.0;
static int stop_thread_light = 0;
int l_count = 0;
pthread_mutex_t light_var_lock = PTHREAD_MUTEX_INITIALIZER;


static inline void setLuxVar(float lux)
{
    pthread_mutex_lock(&light_var_lock);
    lux_val = lux;
    pthread_mutex_unlock(&light_var_lock);
} 

static inline float getLuxVar()
{
    float lux = 0;
    pthread_mutex_lock(&light_var_lock);
    lux = lux_val;
    pthread_mutex_unlock(&light_var_lock);
    return lux;
} 

static int readAndUpdateLight(void)
{
    float lux = 0.0 ;
    int ret = APDS9301_getlight(&lux);
    if(ret)
    {
        LOG_ERROR(TEMP_TASK, "APDS9301 get light: %d", ret);
        return ret;
    } 
    LOG_INFO(LIGHT_TASK,"The light is %f",lux);
    setLuxVar(lux);   
    if(lux == 0)
    {
        l_count++;
    }
    if(l_count > 4)
    {
        stop_thread_light = 1;   
    }
    return 0;
}

void kill_light_thread(void)
{
    stop_thread_light = 1;    
}

float getLight(void)
{
    float lux = getLuxVar();
    return lux;
}

static void giveSemSensor(union sigval no)
{
    sem_post(&light_sem);
}

void *light_task(void *threadp)
{
    sem_init(&light_thread_sem,0,0);
    sem_wait(&light_thread_sem);
    LOG_INFO(LIGHT_TASK,"Light task thread spawned");
    timer_t light_timer;
    sem_init(&light_sem,0,0);
    if(maketimer(&light_timer, &giveSemSensor) != 0)
    {
        perror("MakeTimer fail");
    }
    startTimer(light_timer);
    while(!stop_thread_light)
    {
        set_heartbeatFlag(LIGHT_TASK);
        if(sem_wait(&light_sem) == 0)
        {
            readAndUpdateLight();
        }
    }
    LOG_INFO(LIGHT_TASK,"Light task thread exiting");
    return NULL;
}