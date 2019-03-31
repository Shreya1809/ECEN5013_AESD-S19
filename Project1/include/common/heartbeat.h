/**
 * @file heartbeat.h
 * @author Shreya Chakraborty
 * @brief header file for heartbeat functionality
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_
#include "main.h"
/**
 * @brief Set the heartbeatFlag bit corresponding to each
 *  module that sets the heartbeat to the static volatile 
 * flag called g_heartbeat_taskFlags
 * 
 * @param moduleId 
 */
void set_heartbeatFlag(moduleId_t moduleId);
/**
 * @brief the heartbeat timer call back function
 * checks the global static heartbeat flag to
 * see if all the threads are alive 
 * 
 * @param sigval 
 * @return void
 */
void heatbeat_timer_callback(union sigval no);
/**
 * @brief starts the timer for heartbeat and calls
 * the callback function above
 * 
 */
void startHearbeatCheck(void);
/**
 * @brief sets the thread exit flags in each task modules
 * for clean exit
 * 
 */
void SystemExit(void);
#endif