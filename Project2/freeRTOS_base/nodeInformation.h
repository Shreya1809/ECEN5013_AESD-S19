/*
 * nodeInformation.h
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */

#ifndef NODEINFORMATION_H_
#define NODEINFORMATION_H_

typedef enum nodeInfo{
    MCUtype = 0,
    SOCvendor,
    SOCtype,
    OStype,
    OSversion,
    SWversion,
    components,
    communicationType,
    communicationDetails,
    Hostname,
    MACAddress,
    NODE_INFO_MAX
}nodeInfo_t;

typedef enum {
    STARTUP,
    NORMAL,
    CONN_DISCONNECTED,
    DEGRADED_OPERATION,
    OUTOFSERVICE
}node_operation_state_t;

#define TEMP_DISCONNECTED_FLAG  (1<<0)
#define ACCL_DISCONNECTED_FLAG  (1<<1)
#define DIST_DISCONNECTED_FLAG  (1<<2)
#define CONN_DISCONNECTED_FLAG  (1<<4)
extern volatile int OperationStateFlag;
#include "FreeRTOS.h"
#include "semphr.h"
SemaphoreHandle_t currentOpStateLock;
#define OPStateLOCKInit()   currentOpStateLock = xSemaphoreCreateBinary()

extern const char *_thisNodeInfoIDString[NODE_INFO_MAX];

extern const char *_thisNodeInfo[NODE_INFO_MAX];

//function declarations
node_operation_state_t getThisNodeCurrentOperation();
void setThisNodeCurrentOperation(node_operation_state_t state);


#endif /* NODEINFORMATION_H_ */
