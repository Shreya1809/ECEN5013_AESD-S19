/**
 * @file socket.h
 * @author Shreya Chakraborty
 * @brief header for socket functionality
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef COMM_RECV_H_
#define COMM_RECV_H_


/**
 * @brief to kill socket thread
 * 
 */
void kill_recv_thread(void);
/**
 * @brief Call back function for socket from main thread
 * 
 * @param threadp 
 * @return void* 
 */

#include "communicationPacket.h"
#include "sensorData.h"

void *commRecv_task(void *threadp);
int CommReceive_NodeInfoHandler(int fd, packet_struct_t *packet);                 
int CommReceive_HeartBeatHandler(int clientsock);
int CommReceive_TempDataHandler(int clientsock);
int CommReceive_DistDataHandler(int clientsock);
int CommReceive_AccelDataHandler(int fd, packet_struct_t *packet);
                   
temp_data_t getCurrentTemperature(void);
dist_data_t getCurentDist(void);
accel_data_t getCurrentAccel(void);
accel_data_t getPrevAccel(void);


#endif