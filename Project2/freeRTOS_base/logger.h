/*
 * logger.h
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */

#ifndef LOGGER_H_
#define LOGGER_H_


#include "FreeRTOS.h"
#include "task.h"
#include "nodeInformation.h"
#include "sensorData.h"

typedef enum {
    LOG_INVALID = -1,
    LOG_ERR = 0,          //Error conditions
    LOG_WARNING,          //Warning conditions
    LOG_INFO,             //Informational
    LOG_DEBUG,            //Debug-level messages
    LOG_MAX
}log_level_t;

typedef enum {
    LOGGER_TASK,
    TEMP_TASK,
    ETHERNET_TASK,
    ACCEL_TASK,
    DIST_TASK,
    MAIN_TASK,
    MAX_TASKS = MAIN_TASK, /*maximum no of tasks*/
}moduleId_t;

typedef struct log{

    log_level_t level;
    uint32_t timestamp;
    moduleId_t srcModuleID;
    node_operation_state_t state;
    union{
        temp_data_t temperature;
        //led_data_t led;
        accel_data_t accel;
        dist_data_t dist;
    };
    char msg[40];
    uint16_t crc;
}log_struct_t;

#define getTimeMsec()   (xTaskGetTickCount() / portTICK_PERIOD_MS)

void LogInit();
void LOG_NodeInformation();


extern void LOG_PRINT(const log_struct_t *log);
extern int LOG_ENQUEUE(log_level_t level, moduleId_t modId, void* data, char *msg, ...);

#define LOG_INFO(modId, p_data, msg, ...)   LOG_ENQUEUE(LOG_INFO, modId, p_data, msg, ##__VA_ARGS__)
#define LOG_ERROR(modId, p_data, msg, ...)  LOG_ENQUEUE(LOG_ERR, modId, p_data, msg, ##__VA_ARGS__)
#define LOG_WARN(modId, p_data, msg, ...)   LOG_ENQUEUE(LOG_WARNING, modId, p_data, msg, ##__VA_ARGS__)
#define LOG_DEBUG(modId, p_data,msg, ...)  LOG_ENQUEUE(LOG_DEBUG, modId, p_data, msg, ##__VA_ARGS__)

#endif /* LOGGER_H_ */
