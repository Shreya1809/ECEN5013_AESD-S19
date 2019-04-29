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
int TaskSendToRemoteNode(opcode_t opcode,packet_struct_t* send_comm_packet, int socket);
void *commSend_task(void *threadp);
#define SEND_COMMAND(opcode,packet,socket) TaskSendToRemoteNode(opcode, packet, socket)
#endif