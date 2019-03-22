#include "includes.h"
#include "mytimer.h"
#include "logger.h"
#include "temp.h"
#include "light.h"

/*void DisplaySensorValue(void)
{
    printf("the temp value is %f\n",getTemp(0));
}*/

int maketimer(timer_t *timerID)
{
    int val = 0;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = giveSemSensor;
    event.sigev_value.sival_ptr = NULL;
    event.sigev_notify_attributes = NULL; 
    val = timer_create(CLOCK_REALTIME, &event, timerID); 
     
    return val;
}

void startTimer(timer_t timerID)
{
    int val = 0;
    struct itimerspec its;
    its.it_interval.tv_sec = 3;
    its.it_interval.tv_nsec = 0;//500000000;
    its.it_value.tv_sec = 3;
    its.it_value.tv_nsec = 0;//500000000;
    timer_settime(timerID, 0, &its,0);

}