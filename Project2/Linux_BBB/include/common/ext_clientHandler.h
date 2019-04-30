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


/* 
 * Data thresholds API
*/
void putIn_ReverseGear(void);
void removeFrom_ReverseGear(void);
bool getReverseGearStatus(void);
void SetRemoteNodeTempThresholdValues(float low,float high);
float getTempThresLowVal(void);
float getTempThresHighVal(void);
void SetRemoteNodeAccelDelta(int16_t x,int16_t y,int16_t z);
void getAccelDelta(int16_t *x,int16_t *y,int16_t *z);

void kill_ext_thread(void);
void *ext_clientHandle_task(void *threadp);

#endif