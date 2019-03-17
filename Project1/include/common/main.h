/**
 * @file main.h
 * @author shreya
 * @brief includes for main
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef MAIN_H_
#define MAIN_H_


typedef enum {
    LOG_TASK = 0,
    TEMP_TASK = 1,
    LIGHT_TASK = 2,
    BIST_TASK = 3,
    SOCKET_TASK = 4,
    MAX = 5
}modID_status;

//thread entry functions
/*extern void *bist_task(void *threadp);
extern void *light_task(void *threadp);
extern void *temp_task(void *threadp);
extern void *log_task(void *threadp);
extern void *socket_task(void *threadp);*/


#endif