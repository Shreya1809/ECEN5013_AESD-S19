/*
 * logger_task.c
 *
 *  Created on: Apr 7, 2019
 *      Author: Shreya
 */


#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "logger_task.h"
#include "logger.h"

extern uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The stack size for the Logger task.
//
//*****************************************************************************
#define STACKSIZE_LOGGERTASK       512
//
//*****************************************************************************
// queue for logger
xQueueHandle logger_queue;

static void myLoggerTask(void *params)
{
    log_struct_t recv_log;
    while(1)
    {
        // Receive a message on the created queue.  Block for 10 ticks if a
        // message is not immediately available.
        if( xQueueReceive(logger_queue, &recv_log, (TickType_t)1000 ) == pdPASS )
        {
            LOG_PRINT(&recv_log);
        }
    }
}

uint32_t LoggerTaskInit(void)
{
    //create queue for logging
    LogInit();
    logger_queue = xQueueCreate(40,sizeof(log_struct_t));
    if(logger_queue == NULL )
    {
        return 1;
    }
    if(xTaskCreate(myLoggerTask, (const portCHAR *)"LOGGER", STACKSIZE_LOGGERTASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LOGGER_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    return (0);
}
