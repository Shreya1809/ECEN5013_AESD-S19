/*
 * alert.c
 *
 *  Created on: Apr 10, 2019
 *      Author: Shreya
 */

#include "includes.h"
#include "idle_task.h"
#include "led_task.h"
#include "logger_task.h"
#include "temp_task.h"
#include "alert.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "driverlib/sysctl.h"
#include "semphr.h"

#define STACKSIZE_ALERTTASK       128

extern xSemaphoreHandle Alertsem;

static void myAlertTask(void *params)
{
    while(1)
    {
        if(xSemaphoreTake(Alertsem,1000))
        {

            LOG_INFO(ALERT_TASK,0,"THRESHOLD EXCEEDED");
            //xSemaphoreGive(Alertsem);
        }
    }

}
uint32_t AlertTaskInit(void)
{
    if(xTaskCreate(myAlertTask, (const portCHAR *)"ALERT", STACKSIZE_ALERTTASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_ALERT_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    return (0);
}



