/**
 * @file light.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef LIGHT_H_
#define LIGHT_H_

/**
 * @brief Call back for light task from main thread
 * 
 * @param threadp 
 * @return void* 
 */
void *light_task(void *threadp);

#endif