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

#define PORT 8888  
extern int thread_flag[4];

typedef enum {
    LOGGER_TASK = 0,
    EXT_TASK,
    RECV_TASK,
    SEND_TASK,
    MAIN_TASK,
    MAX_TASKS = MAIN_TASK, /*maximum no of tasks*/
}moduleId_t;

pthread_t threads[MAX_TASKS];

typedef struct{
    moduleId_t srcModuleID;
    char msg[100];
    int status;
}main_struct_t;

uint32_t startofprogramTime;
const char * moduleIdName[MAX_TASKS+1];


#endif