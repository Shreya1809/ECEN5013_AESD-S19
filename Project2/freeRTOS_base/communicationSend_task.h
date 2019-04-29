/*
 * communicationSend_task.h
 *
 *  Created on:
 *      Author: Shreya
 */
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "priorities.h"
#include "logger_task.h"
#include "TCP_layer.h"
#include "includes.h"
#include "logger.h"
#include "communicationPacket.h"


#ifndef COMMUNICATIONSEND_TASK_H_
#define COMMUNICATIONSEND_TASK_H_


//typedef enum {
//    STARTUP,
//    NORMAL,
//    DEGRADED_OPERATION,
//    OUTOFSERVICE
//}node_operation_state_t;

//extern node_operation_state_t _thisNodeOperationState;

int SendTask_enqueue(opcode_t opcode, void *data);
void SendNodeInfo(void);
uint32_t StartCommSendTask();

#define COMM_SEND(opcode, data)   SendTask_enqueue(opcode, data)

#endif /* COMMUNICATIONSEND_TASK_H_ */
