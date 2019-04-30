/**
 * @file socket.c
 * @author Shreya Chakraborty
 * @brief Socket thread task functionality
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "includes.h"
#include "comm_recv.h"
#include "socket.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "heartbeat.h"
#include "uart.h"
#include "comm_send.h"
#include "sensorData.h"
#include "ext_clientHandler.h"

const char * srcnode[5] = {
    "EK_TM4C1294XL",
    "EK_TM4C123GXL",
    "EK_TM4C129EXL",
    "DK_TM4C129X",
    "DK_TM4C123G",   
};

static sig_atomic_t stop_recv_thread = 0;
pthread_mutex_t datalock = PTHREAD_MUTEX_INITIALIZER;
extern pthread_t threads[MAX_TASKS];
#define TRUE   1  
#define FALSE  0  

int retryFlag = 0;
static volatile bool currentRemoteFanState = false;
static volatile temp_data_t Current_Temp = {0};
static volatile dist_data_t Current_Dist = {0};
static volatile accel_data_t Current_accel = {0};
static volatile accel_data_t prev_accel = {0};

static void updateCurrentTemp(temp_data_t *temp)
{
    pthread_mutex_lock(&datalock);
    Current_Temp = *temp;
    pthread_mutex_unlock(&datalock); 
}

temp_data_t getCurrentTemperature(void)
{
    temp_data_t ftemp;
    pthread_mutex_lock(&datalock);
    ftemp = Current_Temp;
    pthread_mutex_unlock(&datalock); 
    return ftemp;
}

static void updateCurrentDist(dist_data_t *dist)
{
    pthread_mutex_lock(&datalock);
    Current_Dist = *dist;
    pthread_mutex_unlock(&datalock);     
}

dist_data_t getCurentDist(void)
{
    dist_data_t distval;
    pthread_mutex_lock(&datalock);
    distval = Current_Dist;
    pthread_mutex_unlock(&datalock);
    return distval; 
}
static void updatePrevAccelValues(void)
{
    pthread_mutex_lock(&datalock);
    prev_accel = Current_accel;
    pthread_mutex_unlock(&datalock);      
}

static void updateCurrentPrevAccelValues(accel_data_t *accel)
{
    pthread_mutex_lock(&datalock);
    prev_accel = Current_accel; 
    Current_accel = *accel;
    pthread_mutex_unlock(&datalock);   
}


accel_data_t getCurrentAccel(void)
{
    accel_data_t x;
    pthread_mutex_lock(&datalock); 
    x = Current_accel;
    pthread_mutex_unlock(&datalock); 
    return x;
}

accel_data_t getPrevAccel(void)
{
    accel_data_t x;
    pthread_mutex_lock(&datalock); 
    x = prev_accel;
    pthread_mutex_unlock(&datalock); 
    return x;
}

void kill_recv_thread(void)
{
    LOG_DEBUG(RECV_TASK,"socket thread exit signal received");
    stop_recv_thread = 1;
    //pthread_cancel(threads[RECV_TASK]);    
}

#ifdef TCP
#define COMM_PHYRECV(socket,data,len)   read(socket , data, len)     
#else
#define COMM_PHYRECV(fd,data,len)   UART_read(fd,data,len)
#endif

extern volatile int remoteNodeDisconnected;

void *commRecv_task(void *pnewsock)
{
    int valread = 0;
    int fd = 0;
    
    if(pnewsock == NULL){
        perror("Invalid socket thread param"); 
        exit(EXIT_FAILURE); 
    }
    fd = *(int*)pnewsock; 
    #ifdef TCP
    LOG_INFO(RECV_TASK,"fd: %d , pthread id: %lu", fd, pthread_self());
    LOG_INFO(RECV_TASK,"socket connection accepted from Remote Client");
    #else
    #endif
    packet_struct_t packet;
    UART_iflush(fd);
    while(!stop_recv_thread) //outer loop for just ther start frames
    {
        memset(&packet, 0, sizeof(packet));
        valread = COMM_PHYRECV(fd , (char*)&packet, sizeof(packet));
        UART_iflush(fd);
        LOG_DEBUG(RECV_TASK, "Valread: %d. sizeof packet:%u",valread, sizeof(packet)); 
        if(VerifyCRC(&packet) == false)
        {
            LOG_ERROR(RECV_TASK, "CRC Check failed");
            UART_iflush(fd);
            continue;
        }
        if(packet.header.dst_node != BBG)
        {
            LOG_WARN(RECV_TASK, "Incorrect Destination. Dropping packet");
            continue;

        }
        LOG_DEBUG(RECV_TASK,"opcode is %d",packet.data.opcode);
        LOG_DEBUG(RECV_TASK, "Packet: ");
        LOG_DEBUG(RECV_TASK,   \
            "Source Node: %d\n  \
            Destination Node: %d\n\
            Timestamp: %u\n\
            Operation state: %d\n\
            Node Type: %d",\
            packet.header.src_node,    \
            packet.header.dst_node,\
            packet.header.timestamp,\
            packet.header.node_state,\
            packet.header.node);

        LOG_DEBUG(RECV_TASK,   \
            "Opcode: %d\n  \
            Data Size: %d\n\
            CRC: %u\n",\
            packet.data.opcode,    \
            packet.data.dataSize, \
            packet.crc);


        switch(packet.data.opcode)
        {
            case NodeInfo : 
                LOG_DEBUG(RECV_TASK, "New Node: %d. State:%d -> Node information",packet.header.src_node, packet.header.node_state);
                CommReceive_NodeInfoHandler(fd, &packet); 
                break;
            case Heartbeat :
                LOG_INFO(RECV_TASK,"<<<Heartbeat from: :%d" ,packet.header.src_node);
                remoteNodeDisconnected = 1;
                COMM_SEND(Heartbeat, NULL);
                break;
            case temperature:
                LOG_INFO(RECV_TASK,"Current Temperature : %f",packet.data.temperature.floatingpoint); 
                updateCurrentTemp(&packet.data.temperature);
                if(packet.data.temperature.floatingpoint > getTempThresHighVal() && currentRemoteFanState != true)
                {
                    LOG_INFO(RECV_TASK, "Setting FAN ON");
                    currentRemoteFanState = true;
                    COMM_SEND(fanStateControl, (void*)&currentRemoteFanState);
                }
                else if (packet.data.temperature.floatingpoint <= getTempThresHighVal() && currentRemoteFanState == true)
                {
                    LOG_INFO(RECV_TASK, "Setting FAN OFF");
                    currentRemoteFanState = false;
                    COMM_SEND(fanStateControl, (void*)&currentRemoteFanState);
                }
                break;
            case distance:
                LOG_INFO(RECV_TASK,"Current Distance : %d",packet.data.dist.distance); 
                updateCurrentDist(&packet.data.dist);
                break;
            case accelerometer:
                    LOG_INFO(RECV_TASK,"Remote Accerelometer(0x%x)[%u]: X=%d, Y=%d, Z=%d",  
                    packet.data.accel.devId,   \
                    packet.data.accel.connected,   \
                    packet.data.accel.x,   \
                    packet.data.accel.y,   \
                    packet.data.accel.z);
                    updateCurrentPrevAccelValues(&packet.data.accel);
                break;
            default:
                UART_iflush(fd);
                break;
        }
       
    }
    #ifdef TCP
    LOG_INFO(RECV_TASK,"Socket task exiting fd: %d\n pthread id: %lu", fd, pthread_self());
    close(fd);
    #else
    LOG_INFO(RECV_TASK,"Receive task exiting fd %d", fd);
    #endif
    return (void*)0;
}

int CommReceive_NodeInfoHandler(int fd, packet_struct_t *packet)
{
    LOG_INFO(RECV_TASK,"Receiving Remote Node Information");
    size_t packetSize = 0;
    size_t dataSize = 0;
    uint16_t CRC = 0;
    char dataBuffer[1024] = {0};

    //get actual data
    int bytesRecv = 0;
    // while(bytesRecv < packet->data.dataSize)
    // {
        int ret = COMM_PHYRECV( fd , &dataBuffer[bytesRecv], packet->data.dataSize-bytesRecv);
        // if(ret < 0)
        // {
        //     perror("Read error info"); 
        //     LOG_ERROR(RECV_TASK,"Read error");
        //     return 1;
        // }
        bytesRecv += ret;
    // }
    
    LOG_DEBUG(RECV_TASK,"data buffer ret val is %d/%d",bytesRecv,packet->data.dataSize);
    dataBuffer[bytesRecv] = '\0';
    LOG_INFO(RECV_TASK, "Remote Node Info:\n%s",dataBuffer);
    return 0;     
}

int CommReceive_HeartBeatHandler( int fd)
{
    LOG_INFO(RECV_TASK,"Receiving HeartBeat");
    char endFrame[5] = {0};
    //char mesg[13] = "HB INFO ACK";
    size_t packetDataSize = 0;
    uint16_t CRC = 0;
    
    //get crc
    COMM_PHYRECV( fd , (char*)&CRC, sizeof(CRC));
    LOG_INFO(RECV_TASK,"crc is %u",CRC);

    //get endframe
    COMM_PHYRECV( fd , endFrame, sizeof(endFrame));
    if (strcmp(endFrame ,"#*#*") == 0)
    {
        LOG_INFO(RECV_TASK,"Control node received HeartBeat successfully"); 
        //send(fd , mesg , sizeof(mesg) , 0 );   
    }
    return 0;     
}


