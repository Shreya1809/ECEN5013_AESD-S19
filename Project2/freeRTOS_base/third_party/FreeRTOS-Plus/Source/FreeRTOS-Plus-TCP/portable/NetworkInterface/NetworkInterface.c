/*
 * NetworkInterface.c
 *
 *  Created on: Apr 24, 2019
 *      Author: Shreya
 *      Reference: FreeRTOSv10.2.0\FreeRTOS-Plus\Source\FreeRTOS-Plus-TCP\portable\NetworkInterface\LPC17xx
 *      FreeRTOSv10.2.0\FreeRTOS-Plus\Source\FreeRTOS-Plus-TCP\portable\NetworkInterface\ATSAM4E
 *      https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html
 */


/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkBufferManagement.h"

#define STACKSIZE_ETHERNET_TASK 512
#define DELAY                   200
/* Driver includes. */
#include "ethernet.h"

/* Demo includes. */
#include "NetworkInterface.h"

TaskHandle_t xEthernetTaskHandle;

static void prvEthernetHandlerTask(void * params)
{
    Ethernet_Init();
    NetworkBufferDescriptor_t *pxBuffer;
    size_t xReceivedBytes;
    IPStackEvent_t xRxEvent;

    for( ;; )
    {
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

        xReceivedBytes = Ethernet_FrameLength();

        if( xReceivedBytes > 0 )
        {
            pxBuffer = pxGetNetworkBufferWithDescriptor( xReceivedBytes, 0 );

            if( pxBuffer != NULL )
            {
                pxBuffer->xDataLength = Ethernet_PacketReceive(EMAC0_BASE, pxBuffer->pucEthernetBuffer, xReceivedBytes);

                xRxEvent.eEventType = eNetworkRxEvent;

                xRxEvent.pvData = ( void * ) pxBuffer;


                if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
                {

                    vReleaseNetworkBufferAndDescriptor( pxBuffer );

                    iptraceETHERNET_RX_EVENT_LOST();
                }
                else
                {

                    iptraceNETWORK_INTERFACE_RECEIVE();
                }

            }
            else
            {
                iptraceETHERNET_RX_EVENT_LOST();
            }
        }
    }
}

BaseType_t xNetworkInterfaceInitialise( void )
{
    xTaskCreate(prvEthernetHandlerTask, "Ethernet",
                 STACKSIZE_ETHERNET_TASK, NULL,
                 configMAX_PRIORITIES - 1, &xEthernetTaskHandle);
    configASSERT( xEthernetTaskHandle );
    vTaskDelay(DELAY);
    return pdPASS;
}

BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t xReleaseAfterSend )
{

    Ethernet_PacketTransmit(EMAC0_BASE,pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength)
    /* Not interested in a call-back after TX. */
    iptraceNETWORK_INTERFACE_TRANSMIT();

    if( xReleaseAfterSend != pdFALSE )
    {
        /* Finished with the network buffer. */
        vReleaseNetworkBufferAndDescriptor(pxNetworkBuffer);
    }

    return pdTRUE;
}






