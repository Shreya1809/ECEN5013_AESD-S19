/*
 * TCP_layer.h
 *
 *  Created on: Apr 24, 2019
 *      Author: Shreya
 */

#ifndef TCP_LAYER_H_
#define TCP_LAYER_H_

#include "includes.h"
#include "FreeRTOS.h"
#include "list.h"
#include "FreeRTOSIPConfig.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "logger.h"

void TCP_init(void);
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent );
size_t vTCPSend(const char *pcBufferToTransmit, const size_t xTotalLengthToSend );
int vTCPReceive( char *pcBufferReceived, size_t bytesToRecv);
void vCreateTCPClientSocket( void );
void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier);

#endif /* TCP_LAYER_H_ */
