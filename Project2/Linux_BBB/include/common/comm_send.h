/**
 * @file comm_send.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef COMM_SEND_H_
#define COMM_SEND_H_
#include "socket.h"


void kill_send_thread(void);
void *commSend_task(void *threadp);
int commSendTask_enqueue(opcode_t opcode, void *data);


#define COMM_SEND(opcode, p_data) commSendTask_enqueue(opcode, p_data)

#endif