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
    LOGGER_TASK = 0,
    TEMP_TASK,
    LIGHT_TASK,
    BIST_TASK,
    SOCKET_TASK,
    MAIN_TASK,
    MAX_TASKS = MAIN_TASK, /*maximum no of tasks*/
}moduleId_t;

typedef struct{
    moduleId_t srcModuleID;
    char msg[100];
    int status;
}main_struct_t;


const char * moduleIdName[MAX_TASKS+1];






#endif