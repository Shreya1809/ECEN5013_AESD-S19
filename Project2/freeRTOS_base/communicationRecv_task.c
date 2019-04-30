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
#include "communicationPacket.h"
#include "sensors_task.h"
#include <string.h>


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
            LOG_ERROR(RECV_TASK, NULL, "TCP Recv Error:%d",ret);
            break;
        }
        bytesRecv += ret;
    }
    return bytesRecv;
}
#define COMM_PHYRECV(data, len) xTCPReceive(data,len)
#else
#define COMM_PHYRECV(data,len)  UART3_getData(data,len)
#endif

#define STACKSIZE_COMM_RECV_TASK     512

TaskHandle_t communicationRecvTaskHandle;
extern volatile bool BBGConnectedFlag;
extern bool volatile inReverseGear;

static void myCommRecvTask(void *params)
{
        UART3_config(9600);
        packet_struct_t recv_packet;
        int valread = 0;

#ifdef TCP
            ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
#endif
            while(1)
            {
                    memset(&recv_packet, 0, sizeof(recv_packet));
                    valread = COMM_PHYRECV((char*)&recv_packet, sizeof(recv_packet));
                    if(valread == 0)
                    {
//                        vTaskDelay(50);
                        continue;
                    }
                    LOG_DEBUG(RECV_TASK,NULL , "Packet received: %u, CRC %d",valread,recv_packet.crc);
                    bool crcCheck = VerifyCRC(&recv_packet);
                    if(crcCheck == false)
                    {
                        LOG_ERROR(RECV_TASK,NULL, "CRC Check failed");
                        continue;
                    }
                    LOG_DEBUG(RECV_TASK,NULL , "Opcode received: %d",recv_packet.data.opcode);
                    LOG_DEBUG(RECV_TASK,NULL, "Packet: ");
                            LOG_DEBUG(RECV_TASK,NULL,   \
                                "Source Node: %d\n  \
                                Destination Node: %d\n\
                                Timestamp: %u\n\
                                Operation state: %d\n\
                                Node Type: %d",\
                                recv_packet.header.src_node,    \
                                recv_packet.header.dst_node,\
                                recv_packet.header.timestamp,\
                                recv_packet.header.node_state,\
                                recv_packet.header.node);

                            LOG_DEBUG(RECV_TASK, NULL,  \
                                "Opcode: %d\n  \
                                Data Size: %d\n\
                                CRC: %u\n",\
                                recv_packet.data.opcode,    \
                                recv_packet.data.dataSize, \
                                recv_packet.crc);

                    switch(recv_packet.data.opcode)
                    {
                    case reverseGearStateControl:
                        LOG_INFO(RECV_TASK, NULL, "Reverse Gear:%u",recv_packet.data.inReverseGear);
                        setReversGear(recv_packet.data.inReverseGear);
                        break;
                    case fanStateControl:
                        LOG_INFO(RECV_TASK, NULL, "Fan:%u",recv_packet.data.isFanOn);
                        DCmotor(recv_packet.data.isFanOn);
                        break;
                    case accelerometerDeltaThresholdControl:
                        setAccelDeltaThreshold(&recv_packet.data.accel);
                        break;
                    case temperatureThresholdControl:
                        setTemperatureThreshold(recv_packet.data.temperature.floatingpoint);
                        break;
                    case Heartbeat:
                        LOG_INFO(RECV_TASK,NULL,"Got Heart beat ACK");
                        BBGConnectedFlag = 0;
                        break;
                    }
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
