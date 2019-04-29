/**
 * @file mytimer.c
 * @author Shreya Chakraborty
 * @brief Posix timer functions for light and temp thread and heartbeat
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "mytimer.h"
#include "logger.h"


int maketimer(timer_t *timerID, void (*callback)(union sigval))
{
    int val = 0;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = callback;
    event.sigev_value.sival_ptr = NULL;
    event.sigev_notify_attributes = NULL; 
    val = timer_create(CLOCK_REALTIME, &event, timerID); 
    return val;
}

int startTimer(timer_t timerID,int sec,int nsec)
{
    int val = 0;
    struct itimerspec its;
    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec = nsec;//500000000;
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = nsec;//500000000;
    return timer_settime(timerID, 0, &its,0);

}

int stopTimer(timer_t timer_id)
{		
	struct itimerspec its;
	
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;

	int ret = timer_settime(timer_id, 0, &its, 0);
	return ret;
}