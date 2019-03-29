#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_
#include "main.h"
/**
 * @brief Set the heartbeatFlag object
 * 
 * @param moduleId 
 */
void set_heartbeatFlag(moduleId_t moduleId);
/**
 * @brief 
 * 
 * @param sigval 
 * @return void
 */
void heatbeat_timer_callback(union sigval no);
/**
 * @brief 
 * 
 */
void startHearbeatCheck(void);
/**
 * @brief 
 * 
 */
void startSystemExit(void);
#endif