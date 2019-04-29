
/*
 * communicationSend_task.c
 *
 *  Created on:
 *      Author: Shreya
 */

#include "communicationSend_task.h"
#include "communicationRecv_task.h"
#include "sensorData.h"
#include "communicationPacket.h"
#include "nodeInformation.h"
#include "commUART.h"

extern uint32_t g_ui32SysClock;

#define STACKSIZE_COMM_SEND_TASK     256

// queue for sending
xQueueHandle send_queue;

int SendTask_enqueue(opcode_t opcode, void *data)
{
    packet_struct_t packet;
    FillPacketHeader(&packet);
    packet.data.opcode = opcode;
    packet.data.dataSize = 0;

    if(data){
        switch(opcode){
        case temperature:
            packet.data.temperature = *(temp_data_t*)data;
            packet.data.dataSize = sizeof(temp_data_t);
            break;
        case Heartbeat:
            break;
        case accelerometer:
            packet.data.accel = *(accel_data_t*)data;
            packet.data.dataSize = sizeof(accel_data_t);
            break;
        case distance:
            packet.data.dist = *(dist_data_t*)data;
            packet.data.dataSize = sizeof(dist_data_t);
            break;
        default:
//            LOG_ERROR(SEND_TASK, NULL, "Invalid Opcode:%s",__FUNCTION__);
            return 1;
        }
    }
    packet.crc = 0;
    if(xQueueSend( send_queue, ( void * ) &packet, ( TickType_t ) 50 ) != pdTRUE)
    {
        return 1;
    }
    return 0;
}


#ifdef TCP
#define COMM_PHYSEND(data, len) vTCPSend(data,len)
#else
#define COMM_PHYSEND(data,len)  UART3_putData(data,len)
#endif

static void SendPacket(packet_struct_t *commpacket)
{

        size_t bytesToSend = sizeof(*commpacket);
        size_t bytesSent = COMM_PHYSEND((char*)commpacket, bytesToSend);
        LOG_DEBUG(ETHERNET_TASK,NULL , "Packet Header Sent: %u",bytesSent);
    //starting of frame
//    size_t bytesSent = COMM_PHYSEND(infostartframe, strlen(infostartframe));
//    LOG_DEBUG(ETHERNET_TASK,NULL , "Start frame sent: %u",bytesSent);

    //sending packet header
//    size_t bytesToSend = sizeof(commpacket->header);
//    bytesSent = COMM_PHYSEND((char*)&commpacket->header, bytesToSend);
//    LOG_DEBUG(ETHERNET_TASK,NULL , "Packet Header Sent: %u",bytesSent);

    //start sending the packet data separately
    //sending the opcode
//    bytesSent = COMM_PHYSEND((char*)&commpacket->data.opcode, sizeof(commpacket->data.opcode));
//    LOG_DEBUG(ETHERNET_TASK,NULL , "Opcode Sent: %u",bytesSent);

//    switch(commpacket->data.opcode){
//    case Heartbeat:
//        LOG_DEBUG(ETHERNET_TASK, NULL, "<<<<<<<> Heartbeat Sent <>>>>>>>");
//        break;
//    case temperature:
//        //sending the actual data size
//        bytesSent = COMM_PHYSEND((char*)&commpacket->data.dataSize, sizeof(commpacket->data.dataSize));
//        LOG_DEBUG(ETHERNET_TASK,NULL , "Data size Sent: %u",bytesSent);
//        bytesSent = COMM_PHYSEND((char*)&commpacket->data.temperature, sizeof(commpacket->data.dataSize));
//        LOG_DEBUG(ETHERNET_TASK,NULL , "Data Sent: %u",bytesSent);
//        break;
//    case accelerometer:
//        //sending the actual data size
//        bytesSent = COMM_PHYSEND((char*)&commpacket->data.dataSize, sizeof(commpacket->data.dataSize));
//        LOG_DEBUG(ETHERNET_TASK,NULL , "Data size Sent: %u",bytesSent);
//        bytesSent = COMM_PHYSEND((char*)&commpacket->data.accel, sizeof(commpacket->data.dataSize));
//        LOG_DEBUG(ETHERNET_TASK,NULL , "Data Sent: %u",bytesSent);
//        break;
//    case distance:
//        //sending the actual data size
//        bytesSent = COMM_PHYSEND((char*)&commpacket->data.dataSize, sizeof(commpacket->data.dataSize));
//        LOG_DEBUG(ETHERNET_TASK,NULL , "Data size Sent: %u",bytesSent);
//        bytesSent = COMM_PHYSEND((char*)&commpacket->data.dist, sizeof(commpacket->data.dataSize));
//        LOG_DEBUG(ETHERNET_TASK,NULL , "Data Sent: %u",bytesSent);
//        break;
//    default:
//        break;
//
//    }

//    bytesSent = COMM_PHYSEND((char*)&commpacket->crc, sizeof(commpacket->crc));
//    LOG_DEBUG(ETHERNET_TASK,NULL , "CRC Sent: %u",bytesSent);
//
//    //ending of frame
//    bytesSent = COMM_PHYSEND(infoendframe, strlen(infoendframe));
//    LOG_INFO(ETHERNET_TASK, NULL, "End frame sent: %u", bytesSent);

}


void SendNodeInfo(void)
{
    packet_struct_t infoPacket;
    infoPacket.header.timestamp = getTimeMsec();
    infoPacket.header.node_state = getThisNodeCurrentOperation();
    infoPacket.header.src_node = EK_TM4C1294XL;
    infoPacket.header.dst_node = BBG;
    infoPacket.header.node = REMOTE_NODE;
    infoPacket.data.opcode = NodeInfo;

    size_t infoDatasize = 0;
    for(int i = 0; i < NODE_INFO_MAX; i++)
    {
        infoDatasize += strlen(_thisNodeInfo[i]);
    }

    infoPacket.data.dataSize = infoDatasize;

    //log.crc = CRC_calculate((uint8_t*)&, uint8_t length)
    infoPacket.crc = 10;

    //starting of frame
//    size_t bytesSent = COMM_PHYSEND(infostartframe, strlen(infostartframe));
//    LOG_DEBUG(ETHERNET_TASK,NULL , "Start frame sent: %u",bytesSent);

    //sending packet header
//    size_t bytesToSend = sizeof(infoPacket.header);
//    bytesSent = COMM_PHYSEND((char*)&infoPacket.header, bytesToSend);
//    LOG_DEBUG(ETHERNET_TASK,NULL , "Packet Header Sent: %u",bytesSent);
//
//    //start sending the packet data separately
//    //sending the opcode
//    bytesSent = COMM_PHYSEND((char*)&infoPacket.data.opcode, sizeof(infoPacket.data.opcode));
//    LOG_DEBUG(ETHERNET_TASK,NULL , "Opcode Sent: %u",bytesSent);

    size_t bytesToSend = sizeof(infoPacket);
    size_t bytesSent = COMM_PHYSEND((char*)&infoPacket, bytesToSend);
    LOG_DEBUG(ETHERNET_TASK,NULL , "Packet Header Sent: %u",bytesSent);


    //sending the actual data size
    bytesSent = COMM_PHYSEND((char*)&infoPacket.data.dataSize, sizeof(infoPacket.data.dataSize));
    LOG_DEBUG(ETHERNET_TASK,NULL , "Data size Sent: %u",bytesSent);

    bytesSent = 0;
    for(int i = 0; i < NODE_INFO_MAX; i++)
    {
        bytesSent += COMM_PHYSEND((char*)_thisNodeInfo[i], strlen(_thisNodeInfo[i]));
    }
    LOG_DEBUG(ETHERNET_TASK,NULL , "Actual Data info Sent: %u",bytesSent);

    bytesSent = COMM_PHYSEND((char*)&infoPacket.crc, sizeof(infoPacket.crc));
    LOG_DEBUG(ETHERNET_TASK,NULL , "CRC Sent: %u",bytesSent);

    //ending of frame
//    bytesSent = COMM_PHYSEND(infoendframe, strlen(infoendframe));
//    LOG_INFO(ETHERNET_TASK, NULL, "End frame sent: %u", bytesSent);
}

TaskHandle_t communicationSendTaskHandle;

static void myCommSendTask(void *params)
{

#ifdef TCP
        TCP_init();
    #if 0
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        vCreateTCPClientSocket();
    #endif
#else
    UART3_config(9600);
#endif
    SendNodeInfo();
    packet_struct_t send_packet;
    while(1)
    {
        //             Receive a message on the created queue.  Block for 1000 ticks if a
        //             message is not immediately available.
        if( xQueueReceive(send_queue, &send_packet, (TickType_t)portMAX_DELAY) == pdPASS )
        {
            LOG_INFO(ETHERNET_TASK, NULL, "TCP Packet Sending");
            SendPacket(&send_packet);
        }
    }

}

uint32_t StartCommSendTask()
{
    send_queue = xQueueCreate(30,sizeof(packet_struct_t));
    if(send_queue == NULL )
    {
        return(1);
    }
    if(xTaskCreate(myCommSendTask, (const portCHAR *)"COMMSENDTASK", STACKSIZE_COMM_SEND_TASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_COMM_SEND_TASK, &communicationSendTaskHandle) != pdTRUE)
    {
        return(1);
    }
    return 0;
}



