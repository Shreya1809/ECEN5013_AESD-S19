/*
 * logger_task.h
 *
 *  Created on: Apr 7, 2019
 *      Author: Shreya
 */

#ifndef LOGGER_TASK_H_
#define LOGGER_TASK_H_

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
    LED_TASK,
    ALERT_TASK,
    MAIN_TASK,
    MAX_TASKS = MAIN_TASK, /*maximum no of tasks*/
}moduleId_t;

typedef char unit_t;
typedef struct temp{

    union{
        struct {
            int16_t mantissa;
            uint16_t exponent;
        }fixedpoint;
        float floatingpoint;
    }data;
    bool connected;
    unit_t unit;
}temp_data_t;

typedef struct led{
    uint32_t count;
    char *name;
}led_data_t;

typedef struct log{

    log_level_t level;
    uint32_t timestamp;
    moduleId_t srcModuleID;
    union{
        temp_data_t temperature;
        led_data_t led;

    }data;
    char msg[20];
    uint16_t crc;
}log_struct_t;

#define getTimeMsec()   (xTaskGetTickCount() / portTICK_PERIOD_MS)

extern uint32_t LoggerTaskInit(void);
extern int LOG_ENQUEUE(log_level_t level, moduleId_t modId, void* data, char *msg, ...);

#define LOG_INFO(modId, p_data, msg, ...)   LOG_ENQUEUE(LOG_INFO, modId, p_data, msg, ##__VA_ARGS__)
#define LOG_ERROR(modId, p_data, msg, ...)  LOG_ENQUEUE(LOG_ERR, modId, p_data, msg, ##__VA_ARGS__)
#define LOG_WARN(modId, p_data, msg, ...)   LOG_ENQUEUE(LOG_WARNING, modId, p_data, msg, ##__VA_ARGS__)
#define LOG_DEBUG(modId, p_data,msg, ...)  LOG_ENQUEUE(LOG_DEBUG, modId, p_data, msg, ##__VA_ARGS__)

#endif /* LOGGER_TASK_H_ */