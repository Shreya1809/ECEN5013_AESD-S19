/**
 * @file logger.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <time.h>
#include <sys/time.h>
/**
 * @brief Get the Time Msec object
 * 
 * @return double 
 */
double getTimeMsec(void);
/**
 * @brief call back for logger task 
 * 
 * @param threadp 
 * @return void* 
 */
void *log_task(void *threadp);

#endif