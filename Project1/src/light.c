#include "includes.h"
#include "light.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "mytimer.h"


float getLight(void)
{
    float i = 100.0;
    return i;
}

void *light_task(void *threadp)
{
    LOG_INFO(LIGHT_TASK,"Light task thread spawned");
    timer_t light_timer;
    if(maketimer(&light_timer) != 0)
    {
        perror("MakeTimer fail");
    }
    startTimer(light_timer);
    //while(!done)
    while(1)
    {
        if(sem_wait(&light_sem) == 0)
        {
            LOG_INFO(LIGHT_TASK,"The light is %f lux",getLight());
        }
    }
    //signal_init();
    LOG_INFO(LIGHT_TASK,"Light task thread exiting");
    return NULL;
}