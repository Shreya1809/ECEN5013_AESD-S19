/*
 * logger.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */

#include "includes.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "logger.h"
#include "nodeInformation.h"

#define BAUDRATE 921600

extern uint32_t g_ui32SysClock;

const char * logLevel[LOG_MAX] = {
    "LOG_ERR",              //Error conditions
    "LOG_WARNING",          //Warning conditions
    "LOG_INFO",             //Informational
    "LOG_DEBUG"            //Debug-level messages
};

const char * moduleIdName[MAX_TASKS+1] = {
    "LOGGER_TASK",
    "TEMP_TASK",
    "SEND_TASK",
    "ACCEL_TASK",
    "DIST_TASK",
    "RECV_TASK",
     "HB_TASK",
     "MAIN_TASK",
};

const char *operationalState[4] = {
       "STARTUP",
       "NORMAL",
       "DEGRADED_OPERATION",
       "OUTOFSERVICE"
};

#define LOG_LEVEL_STR(level)            logLevel[level]
#define MODULE_ID_STR(id)               moduleIdName[id]
#define OPERATIONAL_STATE_STR(state)   (state >=0 && state <4) ? operationalState[state] : ""

//function to print log
void LOG_PRINT(const log_struct_t *log)
{
    UARTprintf("[%u] [%s] [%s] [%s] MSG:%s ",log->timestamp, LOG_LEVEL_STR(log->level), OPERATIONAL_STATE_STR(log->state),MODULE_ID_STR(log->srcModuleID),log->msg);
    switch(log->srcModuleID){
    case TEMP_TASK:
        UARTprintf("{Temperature:%f Unit:%c Connected:%u}",log->temperature.floatingpoint, log->temperature.unit, log->temperature.connected);
        break;
    //case LED_TASK:
    //    UARTprintf("{Count:%u Name:%s}",log->led.count, log->led.name);
    //    break;
    case ACCEL_TASK:
        UARTprintf("{ID:0x%x 'X:%d Y:%d Z:%d' Interrupt:%d Connected:%d}",log->accel.devId, log->accel.x,log->accel.y,log->accel.z, log->accel.INT2, log->accel.connected);
        break;
    case DIST_TASK:
        UARTprintf("{The distance is %d Connected:%u}",log->dist.distance, log->dist.connected);
        break;
    default:
        break;

    };
    UARTprintf("\r\n\r\n");
}

void LOG_NodeInformation()
{
    UARTprintf("\033[2J\033[H");
    UARTprintf("***********************\n");
    UARTprintf("Node Information\n");
    UARTprintf("***********************\n");
    for(int i = 0; i < NODE_INFO_MAX; i++)
    {
        UARTprintf(_thisNodeInfoIDString[i]);
        UARTprintf(_thisNodeInfo[i]);
    }
    UARTprintf("***********************\n");
}

extern xQueueHandle logger_queue;

//log_level_t currentLogLevel = LOG_DEBUG;
log_level_t currentLogLevel = LOG_INFO;

int LOG_ENQUEUE(log_level_t level, moduleId_t modId, void *data, char *msg, ...)
{
    if(level > currentLogLevel)
    {
        return 0;
    }
    log_struct_t send_log;
    send_log.level = level;
    send_log.timestamp = getTimeMsec();
    send_log.srcModuleID = modId;
    send_log.state = getThisNodeCurrentOperation();

    if(data){
        switch(modId){

        case TEMP_TASK:
            send_log.temperature = *(temp_data_t*)data;
            break;
//        case ETHERNET_TASK:
//            //send_log.temperature = *(temp_data_t*)data;
//            break;
            //case LED_TASK:
            //    send_log.led = *(led_data_t*)data;
            //    break;
        case ACCEL_TASK:
            send_log.accel = *(accel_data_t*)data;
            break;
        case DIST_TASK:
            send_log.dist = *(dist_data_t*)data;
            break;
        default:
            break;

        }
    }

    va_list arg_ptr;
    va_start(arg_ptr, msg);
    vsnprintf(send_log.msg, sizeof(send_log.msg), msg, arg_ptr);
    va_end(arg_ptr);
    if(xQueueSend( logger_queue, ( void * ) &send_log, ( TickType_t ) 50 ) != pdTRUE)
    {
        return 1;
    }
    return 0;
}

void InitConsole(uint32_t baudRate)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, baudRate, g_ui32SysClock);
}

void LogInit()
{
    InitConsole(BAUDRATE);
    LOG_NodeInformation();
}
