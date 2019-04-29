/*
 * communicationInit_task.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */


#include "communicationSend_task.h"
#include "communicationRecv_task.h"

uint32_t CommunicationTaskInit()
{
    return StartCommSendTask();
}
