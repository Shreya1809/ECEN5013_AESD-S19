#ifndef TIMER_H_
#define TIMER_H_

#include "includes.h"

/**
 * @brief 
 * 
 * @param timerID 
 * @param callback 
 * @return int 
 */
int maketimer(timer_t *timerID, void (*callback)(union sigval));
/**
 * @brief starting the timer
 * 
 * @param timerID 
 * @return int 
 */
int startTimer(timer_t timerID);
/**
 * @brief start timer for heartbeat
 * 
 */
int startTimerHB(timer_t timerID);

/**
 * @brief stops the timer
 * 
 * @param timer_id 
 * @return int 
 */
int stopTimer(timer_t timer_id);

#endif
