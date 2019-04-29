/*
 * communicationRecv_task.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */

#include <stdbool.h>
#include "communicationRecv_task.h"
#include "FreeRTOS.h"
#include "priorities.h"
#include "task.h"
#include "logger.h"
#include "commUART.h"

#ifdef TCP
#include "TCP_layer.h"
int xTCPReceive(char *buffer, size_t size)
{
    int ret = 0;
    int bytesRecv = 0;
    while(bytesRecv < size)
    {
        ret= vTCPReceive(&buffer[bytesRecv], size - bytesRecv);
        if(ret < 0)
        {
            LOG_ERROR(ETHERNET_TASK, NULL, "TCP Recv Error:%d",ret);
            break;
        }
        bytesRecv += ret;
    }
    return bytesRecv;
}
#define COMM_PHYRECV(data, len) vTCPSend(data,len)
#else
#define COMM_PHYRECV(data,len)  UART3_getData(data,len)
#endif

#define STACKSIZE_COMM_RECV_TASK     512

TaskHandle_t communicationRecvTaskHandle;
extern volatile bool BBGConnectedFlag;

static void myCommRecvTask(void *params)
{
        char start[15] = {0};
        int ret = 0;
        while(1)
        {
#ifdef TCP
            ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
#endif
            ret = COMM_PHYRECV(start, 15);
            if(ret == 15)
            {
                LOG_INFO(ETHERNET_TASK, NULL, "Bytes Recv:%d",ret);
                BBGConnectedFlag = true;
            }
            vTaskDelay(50);
        }
}

uint32_t StartCommRecvTask()
{
    if(xTaskCreate(myCommRecvTask, (const portCHAR *)"COMMRECVTASK", STACKSIZE_COMM_RECV_TASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_COMM_RECV_TASK, &communicationRecvTaskHandle) != pdTRUE)
    {
        return(1);
    }
    return 0;
}
