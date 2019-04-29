/**
 * @file ext_clientHandler.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef EXT_CLIENT_HANDLER_H_
#define EXT_CLIENT_HANDLER_H_


float getTempThresLowVal(void);
float getTempThresHighVal(void);
bool getReverseGearStatus(void);
uint16_t getXaxisDelta(void);
uint16_t getYaxisDelta(void);
uint16_t getZaxisDelta(void);
void kill_ext_thread(void);
void *ext_clientHandle_task(void *threadp);

#endif