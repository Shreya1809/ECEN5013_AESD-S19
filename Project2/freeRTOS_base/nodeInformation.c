/*
 * nodeInformation.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */

#include "nodeInformation.h"


const char *_thisNodeInfoIDString[NODE_INFO_MAX] = {
    "MCUtype:",
    "SOCvendor:",
    "SOCtype:",
    "OStype:",
    "OSversion:",
    "SWversion:",
    "components:",
    "communicationType:",
    "communicationDetails:",
    "Hostname:",
    "MACAddress:"
};

const char *_thisNodeInfo[NODE_INFO_MAX] = {
"ARM-Cortex-M4\n",
"Texas Instruments\n",
"TIVA C SERIES\n",
"FreeRTOS\n",
"v10.0.0\n",
"v0.1\n",
"HCSR04, ADXL345, TMP102, DC Motor, LCD, Buzzer, LED\n",
"Ethernet\n",
"IPv4 - 10.0.0.15\n",
"ShCh_TIVA_FreeRTOS_10.0.0.15\n",
"00:bc:de:ef:12:4a\n"
};

volatile int OperationStateFlag = 0x00;

//global variable for seeting the operation state
static volatile node_operation_state_t _thisNodeOperationState = STARTUP;

node_operation_state_t getThisNodeCurrentOperation()
{
//    xSemaphoreTake(currentOpStateLock, portMAX_DELAY);
    node_operation_state_t state = _thisNodeOperationState;
//    xSemaphoreGive(currentOpStateLock);
    return state;
}

void setThisNodeCurrentOperation(node_operation_state_t state)
{
//    xSemaphoreTake(currentOpStateLock, portMAX_DELAY);
    _thisNodeOperationState = state;
//    xSemaphoreGive(currentOpStateLock);
}

