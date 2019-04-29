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
void *commRecv_task(void *threadp);
int CommReceive_NodeInfoHandler(int clientsock);                  
int CommReceive_HeartBeatHandler(int clientsock);
int CommReceive_TempDataHandler(int clientsock);
int CommReceive_DistDataHandler(int clientsock);
int CommReceive_AccelDataHandler(int clientsock);
                   
float getCurrentTemperature(void);
int32_t getCurentDist(void);
int16_t getCurrentXaxis(void);
int16_t getCurrentYaxis(void);
int16_t getCurrentZaxis(void);
int16_t getPrevXaxis(void);
int16_t getPrevYaxis(void);
int16_t getPrevZaxis(void);


#endif