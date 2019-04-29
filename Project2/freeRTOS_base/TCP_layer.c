/*
 * TCP_layer.c
 *
 *  Created on: Apr 24, 2019
 *      Author: Shreya
 */
#include "TCP_layer.h"
#include "logger.h"

#define BUFFER_SIZE 512

/* The MAC address array is not declared const as the MAC address will
normally be read from an EEPROM and not hard coded (in real deployed
applications).*/
static uint8_t ucMACAddress[ 6 ] = {0x00, 0x1a, 0xb6, 0x03, 0x2b, 0xde};
//const uint8_t pui8MACArray[6] ={0x00, 0xbc, 0xde, 0xef, 0x12, 0x4a};
extern const uint8_t pui8MACArray[6];

/* Define the network addressing.  These parameters will be used if either
ipconfigUDE_DHCP is 0 or if ipconfigUSE_DHCP is 1 but DHCP auto configuration
failed. */
static const uint8_t ucIPAddress[ 4 ] = { 10, 0, 0, 12 };
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress[ 4 ] = { 10, 0, 0, 1 };
/* The following is the address of an Google //OpenDNS server//. */
static const uint8_t ucDNSServerAddress[ 4 ] = { 8, 8, 8, 8 };

Socket_t xClientSocket = NULL;

void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
      return;
}

//const char *pcApplicationHostnameHook( void )
//{
//    return _thisNodeInfo[Hostname];
//}

extern TaskHandle_t communicationSendTaskHandle;
#define SIGNAL_TASK_ON_NETWORK_UP()    xTaskNotifyGive(communicationSendTaskHandle)


extern TaskHandle_t communicationRecvTaskHandle;
#define SIGNAL_TASK_ON_TCP_RECV()    xTaskNotifyGive(communicationRecvTaskHandle)


void TCP_init(void)
{
    /* Initialise the RTOS's TCP/IP stack.  The tasks that use the network
        are created in the vApplicationIPNetworkEventHook() hook function
        below.  The hook function is called when the network connects. */
    FreeRTOS_IPInit( ucIPAddress,
                     ucNetMask,
                     ucGatewayAddress,
                     ucDNSServerAddress,
                     ucMACAddress );
//                     pui8MACArray);
    /*
     * Other RTOS tasks can be created here.
     */
#if 1
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
    vCreateTCPClientSocket();
#endif
}

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* Both eNetworkUp and eNetworkDown events can be processed here. */
    if( eNetworkEvent == eNetworkUp )
    {
        /* Create the tasks that use the TCP/IP stack if they have not already
        been created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /*
             * For convenience, tasks that use FreeRTOS+TCP can be created here
             * to ensure they are not created before the network is usable.
             */
            LOG_DEBUG(ETHERNET_TASK, NULL, "Network Up event");
            SIGNAL_TASK_ON_NETWORK_UP();

            xTasksAlreadyCreated = pdTRUE;
        }
    }
    else if( eNetworkEvent == eNetworkDown )
    {
        LOG_ERROR(ETHERNET_TASK, NULL, "Network Down");
    }
}

BaseType_t TCPRecvCallback(Socket_t xSocket, void * pData , size_t  xLength)
{
    if(xSocket == xClientSocket)
    {
        SIGNAL_TASK_ON_TCP_RECV();
    }
    return 0;

}

#define REMOTE_PORT     10000
#define REMOTE_IP       "10.0.0.16"

void vCreateTCPClientSocket( void )
{
    struct freertos_sockaddr xRemoteAddress = {0};
    static TickType_t xTimeOut = pdMS_TO_TICKS( 2000 );

    /* Set the IP address (192.168.0.50) and port (1500) of the remote socket
    to which this client socket will transmit. */
    xRemoteAddress.sin_port = FreeRTOS_htons( REMOTE_PORT );
    xRemoteAddress.sin_addr = FreeRTOS_inet_addr( REMOTE_IP );

    /* Attempt to open the socket. */
    xClientSocket = FreeRTOS_socket( FREERTOS_AF_INET,
                                     FREERTOS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
                                     FREERTOS_IPPROTO_TCP );

    /* Check the socket was created. */
    configASSERT( xClientSocket != FREERTOS_INVALID_SOCKET );
    if(xClientSocket == FREERTOS_INVALID_SOCKET)
    {
        LOG_ERROR(ETHERNET_TASK, NULL, "Invalid Socket");
        while(1);

    }

    /* If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
    FreeRTOS_setsockopt() to change the buffer sizes from their default then do
    it here!.  (see the FreeRTOS_setsockopt() documentation. */

    /* If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is to
    be used with FreeRTOS_setsockopt() to change the sliding window size from
    its default then do it here! (see the FreeRTOS_setsockopt()
    documentation. */

    FreeRTOS_setsockopt( xClientSocket,
                         0,
                         FREERTOS_SO_SNDTIMEO,
                         &xTimeOut,
                         sizeof( xTimeOut ) );

    xTimeOut = portMAX_DELAY;

    /* Set send and receive time outs. */
    FreeRTOS_setsockopt( xClientSocket,
                         0,
                         FREERTOS_SO_RCVTIMEO,
                         &xTimeOut,
                         sizeof( xTimeOut ) );

    /* Set send and receive time outs. */
    FreeRTOS_setsockopt( xClientSocket,
                         0,
                         FREERTOS_SO_TCP_RECV_HANDLER,
                         TCPRecvCallback,
                         sizeof( TCPRecvCallback ) );


    /* Bind the socket, but pass in NULL to let FreeRTOS+TCP choose the port number.
    See the next source code snipped for an example of how to bind to a specific
    port number. */
    FreeRTOS_bind(xClientSocket, &xRemoteAddress, sizeof( xRemoteAddress ));

    LOG_DEBUG(ETHERNET_TASK, NULL, "Socket Bind. Now trying to Connect#####");
    while(FreeRTOS_connect( xClientSocket, &xRemoteAddress, sizeof( xRemoteAddress )) != 0);
    if(pdTRUE != FreeRTOS_issocketconnected(xClientSocket))
    {
        LOG_ERROR(ETHERNET_TASK, NULL, "->Socket Not Connected#########");
    }
    else
    {
        struct freertos_sockaddr xConnectedAddress = {0};
        FreeRTOS_GetRemoteAddress( xClientSocket, &xConnectedAddress );
        char ip[16];
        FreeRTOS_inet_ntoa( xConnectedAddress.sin_addr, ip);
        LOG_INFO(ETHERNET_TASK, NULL, "Socket Connected");
        LOG_INFO(ETHERNET_TASK, NULL, "IP:%s Port:%d", ip, FreeRTOS_ntohs(xConnectedAddress.sin_port) );
    }

}

size_t vTCPSend(const char *pcBufferToTransmit, const size_t xTotalLengthToSend )
{

    BaseType_t xAlreadyTransmitted = 0, xBytesSent = 0;
    //TaskHandle_t xRxTask = NULL;
    size_t xLenToSend;
    /* Keep sending until the entire buffer has been sent. */
    while( xAlreadyTransmitted < xTotalLengthToSend )
    {
        /* How many bytes are left to send? */
        xLenToSend = xTotalLengthToSend - xAlreadyTransmitted;
        xBytesSent = FreeRTOS_send( /* The socket being sent to. */
                xClientSocket,
                /* The data being sent. */
                &( pcBufferToTransmit[ xAlreadyTransmitted ] ),
                /* The remaining length of data to send. */
                xLenToSend,
                /* ulFlags. */
                0 );

        if( xBytesSent >= 0 )
        {
            /* Data was sent successfully. */
            xAlreadyTransmitted += xBytesSent;
        }
        else
        {
            /* Error - break out of the loop for graceful socket close. */
            break;
        }

    }
    return xAlreadyTransmitted;

}

int vTCPReceive( char *pcBufferReceived, size_t bytesToRecv)
{

    BaseType_t lBytesReceived;


    /* Receive another block of data into the cRxedData buffer. */
    lBytesReceived = FreeRTOS_recv( xClientSocket, pcBufferReceived, bytesToRecv, 0 );

    if( lBytesReceived > 0 )
    {
        /* Data was received, process it here. */
        //prvPorcessData( pcBufferReceived, lBytesReceived );
        return lBytesReceived;
    }
    else if( lBytesReceived == 0 )
    {
        /* No data was received, but FreeRTOS_recv() did not return an error.
            Timeout? */
        return 0;
    }
    else
    {
        /* Error (maybe the connected socket already shut down the socket?).
            Attempt graceful shutdown. */
        //FreeRTOS_shutdown( xSocket, FREERTOS_SHUT_RDWR );
        return -1;
    }
}

