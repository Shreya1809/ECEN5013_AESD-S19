/**
 * @file logger.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include "includes.h"
#include "main.h"

#define LOG_QUEUE_NAME         "/logger_q"
#define QUEUE_PERMISSIONS   0666
#define MAX_MESSAGES        128
#define MAX_MSG_SIZE        256
#define CURRENT_MSG         0
#define FLAGS               0

#define LOG_STDOUT
static mqd_t mq_logger;

typedef enum{
    LOG_INVALID = -1,
    LOG_ERR = 0,          //Error conditions
    LOG_WARNING,          //Warning conditions
    LOG_INFO,             //Informational
    LOG_DEBUG,            //Debug-level messages
    LOG_MAX
}log_level_t;

const char * logLevel[LOG_MAX]; 
typedef struct{

    log_level_t level;
    char  msg[100];
    double timestamp;
    moduleId_t srcModuleID;

}log_struct_t;

struct loggerTask_param{

    char *filename;
    log_level_t loglevel;
};

#define PRINTLOGCONSOLE(f_, ...)    do{\
             printf("[%lf] [PID:%d] [TID:%ld] Message:",getTimeMsec(),getpid(),syscall(SYS_gettid));\
             printf(f_, ##__VA_ARGS__);\
             printf("\n");\
             fflush(stdout);\
            }while(0)

#define PRINT(f_, ...)   printf(f_, ##__VA_ARGS__)

#define LOG_INFO(modId, msg, ...)   LOG_ENQUEUE(LOG_INFO, modId, msg, ##__VA_ARGS__)
#define LOG_ERROR(modId, msg, ...)  LOG_ENQUEUE(LOG_ERR, modId, msg, ##__VA_ARGS__)
#define LOG_WARN(modId, msg, ...)   LOG_ENQUEUE(LOG_WARNING, modId, msg, ##__VA_ARGS__)
#define LOG_DEBUG(modId, msg, ...)  LOG_ENQUEUE(LOG_DEBUG, modId, msg, ##__VA_ARGS__)

int LOG_ENQUEUE(log_level_t level ,moduleId_t modId, char *msg, ...);
/**
 * @brief queue initialisation
 * 
 * @return int 
 */
mqd_t logger_queue_init(void);

/**
 * @brief call back for logger task 
 * 
 * @param threadp 
 * @return void* 
 */
void *logger_task(void *threadp);
/**
 * @brief Get the Time Msec object
 * 
 * @return double 
 */
double getTimeMsec(void);
#endif