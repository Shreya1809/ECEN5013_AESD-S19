/**
 * @file mytimer.h
 * @author Shreya Chakraborty
 * @brief header file for timer functions
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef TIMER_H_
#define TIMER_H_

#include "includes.h"

/**
 * @brief initialise timer with call back functions
 * 
 * @param timerID 
 * @param callback 
 * @return int 
 */
int maketimer(timer_t *timerID, void (*callback)(union sigval));
/**
 * @brief starts timer for sensor threads
 * 
 * @param timerID 
 * @param sec 
 * @param nsec 
 * @return int 
 */
int startTimer(timer_t timerID,int sec,int nsec);
/**
 * @brief starts timer for heartbeat functions
 * 
 * @param timerID 
 * @param sec 
 * @param nsec 
 * @return int 
 */
int startTimerHB(timer_t timerID,int sec,int nsec);

/**
 * @brief stops the timer
 * 
 * @param timer_id 
 * @return int 
 */
int stopTimer(timer_t timer_id);

#endif
