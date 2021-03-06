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
"ARM-Cortex-A\n",
"Texas Instruments\n",
"BBG\n",
"LINUX\n",
"v4.7\n",
"v0.1\n",
"MT\n",
"Ethernet/UART4\n",
"IPv4 - 10.0.0.15\n",
"ShCh_TIVA_FreeRTOS_10.0.0.15\n",
"00:bc:de:ef:12:4a\n"
};

volatile int remoteNodeDisconnected = 0;

//global variable for seeting the operation state
static node_operation_state_t _thisNodeOperationState = STARTUP;


node_operation_state_t getThisNodeCurrentOperation(){
    return _thisNodeOperationState;
}

void setThisNodeCurrentOperation(node_operation_state_t state){
    _thisNodeOperationState = state;
}

