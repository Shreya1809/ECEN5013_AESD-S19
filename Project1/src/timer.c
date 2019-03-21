#include "includes.h"
#include "timer.h"


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
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 100000000;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 100000000;
    timer_settime(timerID, 0, &its,0);

}