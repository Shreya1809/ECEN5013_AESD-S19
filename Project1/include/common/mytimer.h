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
void startTimer(timer_t timerID);

#endif
