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
#include <sys/time.h>

static mqd_t mq_logger;

typedef enum
{
	MAIN,
	TEMP,
	LIGHT,
	SOCKET,
	LOGGER,
	BIST,

}log_module_t;

typedef enum{

    KERN_EMERG = 0,        //System is unusable
    KERN_ALERT,            //Action must be taken immediately
    KERN_CRIT,             //Critical conditions
    KERN_ERR ,             //Error conditions
    KERN_WARNING,          //Warning conditions
    KERN_NOTICE,          //Normal but significant condition
    KERN_INFO,             //Informational
    KERN_DEBUG,            //Debug-level messages

}log_level_t;

typedef struct{

    log_level_t log_level;
    uint8_t log_mesg[1024];
    double Timestamp;
    log_module_t logger_modID;

}log_struct_t;

log_struct_t logger_packet;
/**
 * @brief queue initialisation
 * 
 * @return int 
 */
int mesg_queue_init(void);

/**
 * @brief Get the Time Msec object
 * 
 * @return double 
 */
double getTimeMsec(void);
/**
 * @brief call back for logger task 
 * 
 * @param threadp 
 * @return void* 
 */
void *log_task(void *threadp);

#endif