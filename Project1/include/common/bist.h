/**
 * @file bist.h
 * @author Shreya Chakraborty
 * @brief header file for bist task thread functions
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BIST_H_
#define BIST_H_

/**
 * @brief testing i2c init
 * 
 * @return int 
 */
int Test_I2C(void);
/**
 * @brief checking all thread status after creation from the global flag
 * 
 * @return int 
 */
int Test_AllThreads(void);
/**
 * @brief testing temperature register
 * 
 * @return int 
 */
int Test_TempSensor(void);
/**
 * @brief testing logger enqueue
 * 
 * @return int 
 */
int Test_loggerQ(void);
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
/**
 * @brief checks BIST_allOk flag to see if the bist was successful
 * 
 * @return int 
 */
int CheckBistResult(void);

/**
 * @brief posts semaphores to rest of the threads
 * 
 */
void PostBistOkResult(void);

#endif