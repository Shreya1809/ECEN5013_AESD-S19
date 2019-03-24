/**
 * @file bist.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BIST_H_
#define BIST_H_
/**
 * @brief testing if light sensor is connected
 * 
 */
int Test_LightSensor(void);
/**
 * @brief call back function for bist task from main task
 * 
 * @param threadp 
 * @return void* 
 */
void *bist_task(void *threadp);


#endif