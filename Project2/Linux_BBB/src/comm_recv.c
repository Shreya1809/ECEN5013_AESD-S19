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
static volatile float Current_Temp = 0.0;
static volatile int32_t Current_Dist = 0;
static volatile int16_t Current_x_axis = 0;
static volatile int16_t Current_y_axis = 0;
static volatile int16_t Current_z_axis = 0;
static volatile int16_t prev_x_axis = 0;
static volatile int16_t prev_y_axis = 0;
static volatile int16_t prev_z_axis = 0;

static void SetCurrentTemp(float val)
{
    pthread_mutex_lock(&datalock);
    Current_Temp = val;
    pthread_mutex_unlock(&datalock); 
}

float getCurrentTemperature(void)
{
    float ftemp = 0.0;
    pthread_mutex_lock(&datalock);
    ftemp = Current_Temp;
    pthread_mutex_unlock(&datalock); 
    return ftemp;
}

static void SetCurrentDist(int32_t Distval)
{
    pthread_mutex_lock(&datalock);
    Current_Dist = Distval;
    pthread_mutex_unlock(&datalock);     
}

int32_t getCurentDist(void)
{
    int32_t distval;
    pthread_mutex_lock(&datalock);
    distval = Current_Dist;
    pthread_mutex_unlock(&datalock);
    return distval; 
}

static void setCurrentAccelValues(int16_t x,int16_t y, int16_t z)
{
    pthread_mutex_lock(&datalock); 
    Current_x_axis = x;
    Current_y_axis = y;
    Current_z_axis = z;
    pthread_mutex_unlock(&datalock);   
}

static void setPrevAccelValues(void)
{
    pthread_mutex_lock(&datalock);
    prev_x_axis = Current_x_axis;
    prev_y_axis = Current_y_axis;
    prev_z_axis = Current_z_axis;
    pthread_mutex_unlock(&datalock);      
}

int16_t getCurrentXaxis(void)
{
    int16_t x = 0;
    pthread_mutex_lock(&datalock); 
    x = Current_x_axis;
    pthread_mutex_unlock(&datalock); 
    return x;
}

int16_t getCurrentYaxis(void)
{
    int16_t Y = 0;
    pthread_mutex_lock(&datalock); 
    Y = Current_y_axis;
    pthread_mutex_unlock(&datalock); 
    return Y;
}

int16_t getCurrentZaxis(void)
{
    int16_t z = 0;
    pthread_mutex_lock(&datalock); 
    z = Current_z_axis;
    pthread_mutex_unlock(&datalock); 
    return z;
}

int16_t getPrevYaxis(void)
{
    int16_t y = 0;
    pthread_mutex_lock(&datalock); 
    y = prev_y_axis;
    pthread_mutex_unlock(&datalock); 
    return y;
}

int16_t getPrevXaxis(void)
{
    int16_t x = 0;
    pthread_mutex_lock(&datalock); 
    x = prev_x_axis;
    pthread_mutex_unlock(&datalock); 
    return x;
}

int16_t getPrevZaxis(void)
{
    int16_t z = 0;
    pthread_mutex_lock(&datalock); 
    z = prev_z_axis;
    pthread_mutex_unlock(&datalock); 
    return z;
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

void *commRecv_task(void *pnewsock)
{
    int valread = 0;
    char startFrame[5] = {0};
    char buffer[1024] = {0}; 
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
    memset(startFrame, 0, sizeof(startFrame));
    packet_struct_t packet;
    UART_flush(fd);
    while(!stop_recv_thread) //outer loop for just ther start frames
    {
        memset(&packet, 0, sizeof(packet));
        valread = COMM_PHYRECV(fd , (char*)&packet, sizeof(packet));
        LOG_DEBUG(RECV_TASK, "Valread: %d. sizeof packet:%u",valread, sizeof(packet)); 
        bool crcCheck = VerifyCRC(&packet);
        if(crcCheck == false)
        {
            LOG_ERROR(RECV_TASK, "CRC Check failed");
            UART_flush(fd);
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
                CommReceive_NodeInfoHandler(fd); 
                break;
            case Heartbeat :
                LOG_INFO(RECV_TASK,"<<<Heartbeat from: :%d" ,packet.header.src_node);
                break;
            case temperature:
                LOG_INFO(RECV_TASK,"Current Temperature : %f",packet.data.temperature.floatingpoint); 
                if(packet.data.temperature.floatingpoint > 26.0)
                {

                }
                break;
            case distance:
                LOG_INFO(RECV_TASK,"Current Distance : %d",packet.data.dist.distance); 
                // CommReceive_DistDataHandler(fd);
                break;
            case accelerometer:
                // CommReceive_AccelDataHandler(fd);
                break;
            default:
                UART_flush(fd);
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

int CommReceive_NodeInfoHandler(int fd)
{
    LOG_INFO(RECV_TASK,"Receiving Remote Node Information");
    char endFrame[5] = {0};
    size_t packetSize = 0;
    size_t dataSize = 0;
    uint16_t CRC = 0;
    char dataBuffer[1024] = {0};
    char mesg[15] = "NODE INFO ACK";
    
    //get actual data size
    COMM_PHYRECV( fd , (char*)&dataSize, sizeof(dataSize));
    LOG_INFO(RECV_TASK,"Data Size %u",dataSize);

    //get actual data
    int bytesRecv = 0;
    while(bytesRecv < dataSize)
    {
        int ret = COMM_PHYRECV( fd , &dataBuffer[bytesRecv], dataSize-bytesRecv);
        if(ret < 0)
        {
            perror("Read error info"); 
            LOG_ERROR(RECV_TASK,"Read error");
            return 1;
        }
        bytesRecv += ret;
    }
    
    LOG_DEBUG(RECV_TASK,"data buffer ret val is %d",bytesRecv);
    dataBuffer[dataSize] = '\0';
    LOG_INFO(RECV_TASK, "Remote Node Info:\n%s",dataBuffer);

    //get crc
    // COMM_PHYRECV( fd , (char*)&CRC, sizeof(CRC));
    // LOG_INFO(RECV_TASK,"crc is %u",CRC);

    // //get endframe
    // COMM_PHYRECV( fd , endFrame, sizeof(endFrame));
    // if (strcmp(endFrame ,"#*#*") == 0)
    // {
    //     LOG_INFO(RECV_TASK,"Control node received remote node info successfully"); 
    //     UART_write(fd , mesg , sizeof(mesg)); 
    // }
    return 0;     
}

int CommReceive_TempDataHandler(int fd)
{
    LOG_INFO(RECV_TASK,"Receiving Temperature data");
    // char endFrame[5] = {0};
    // size_t packetDataSize = 0;
    uint16_t CRC = 0;

    //get actual data size
    // COMM_PHYRECV( fd , (char*)&packetDataSize, sizeof(packetDataSize));
    // LOG_INFO(RECV_TASK,"Packet Data Size %u",packetDataSize);

    // //get actual temp data
    // int ret = COMM_PHYRECV( fd , (char*)&tempdata, packetDataSize);
    // LOG_DEBUG(RECV_TASK,"data buffer ret val is %d",ret);
    LOG_INFO(RECV_TASK,"Current Temperature : %f",tempdata.floatingpoint);

    //get crc
    // COMM_PHYRECV( fd , (char*)&CRC, sizeof(CRC));
    LOG_INFO(RECV_TASK,"crc is %u",CRC);

    //get endframe
    // COMM_PHYRECV( fd , endFrame, sizeof(endFrame));
    // if (strcmp(endFrame ,"#*#*") == 0)
    // {
    //     LOG_INFO(RECV_TASK,"Control node received Temperature data successfully");   
    // }
    return 0; 
}

int CommReceive_DistDataHandler( int fd)
{
    LOG_INFO(RECV_TASK,"Receiving distance data");
    char endFrame[5] = {0};
    size_t packetDataSize = 0;
    uint16_t CRC = 0;
    
    //get actual data size
    COMM_PHYRECV( fd , (char*)&packetDataSize, sizeof(packetDataSize));
    LOG_INFO(RECV_TASK,"Packet Data Size %u",packetDataSize);

    //get actual dist data
    int ret = COMM_PHYRECV( fd , (char*)&distdata, packetDataSize);
    LOG_DEBUG(RECV_TASK,"data buffer ret val is %d",ret);
    LOG_INFO(RECV_TASK,"Current distance : %d",distdata.distance);
    //get crc
    COMM_PHYRECV( fd , (char*)&CRC, sizeof(CRC));
    LOG_INFO(RECV_TASK,"crc is %u",CRC);

    //get endframe
    COMM_PHYRECV( fd , endFrame, sizeof(endFrame));
    if (strcmp(endFrame ,"#*#*") == 0)
    {
        LOG_INFO(RECV_TASK,"Control node received Distance data successfully");   
    }
    return 0; 

}

int CommReceive_AccelDataHandler( int fd)
{
    LOG_INFO(RECV_TASK,"Receiving acceleration data");
    char endFrame[5] = {0};
    size_t packetDataSize = 0;
    uint16_t CRC = 0;
    
    //get actual data size
    COMM_PHYRECV( fd , (char*)&packetDataSize, sizeof(packetDataSize));
    LOG_INFO(RECV_TASK,"Packet Data Size %u",packetDataSize);

    //get actual accel data
    int ret = COMM_PHYRECV( fd , (char*)&acceldata, packetDataSize);
    LOG_DEBUG(RECV_TASK,"data buffer ret val is %d",ret);
    LOG_INFO(RECV_TASK,"Accerelometer dev ID : 0x%x",acceldata.devId);
    //get crc
    COMM_PHYRECV( fd , (char*)&CRC, sizeof(CRC));
    LOG_INFO(RECV_TASK,"crc is %u",CRC);

    //get endframe
    COMM_PHYRECV( fd , endFrame, sizeof(endFrame));
    if (strcmp(endFrame ,"#*#*") == 0)
    {
        LOG_INFO(RECV_TASK,"Control node received Accel data successfully");   
    }
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


