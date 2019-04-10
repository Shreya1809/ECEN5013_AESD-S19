/*
 * logger_task.c
 *
 *  Created on: Apr 7, 2019
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
#include "idle_task.h"
#include "led_task.h"
#include "logger_task.h"
#include "temp_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "driverlib/sysctl.h"

extern uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The stack size for the Logger task.
//
//*****************************************************************************
#define STACKSIZE_LOGGERTASK       256
//
//*****************************************************************************
// queue for logger
xQueueHandle logger_queue;

#define BAUDRATE 921600

const char * logLevel[LOG_MAX] = {
    "LOG_ERR",              //Error conditions
    "LOG_WARNING",          //Warning conditions
    "LOG_INFO",             //Informational
    "LOG_DEBUG"            //Debug-level messages
};

const char * moduleIdName[MAX_TASKS+1] = {
    "LOGGER_TASK",
    "TEMP_TASK",
    "LED_TASK",
    "ALERT_TASK",
    "MAIN_TASK",
};

#define LOG_LEVEL_STR(level)    logLevel[level]
#define MODULE_ID_STR(id)       moduleIdName[id]

//function to print log
static void printlog(const log_struct_t *log)
{
    UARTprintf("[%u][%s][%s]MSG:%s",log->timestamp, LOG_LEVEL_STR(log->level),MODULE_ID_STR(log->srcModuleID),log->msg);
    switch(log->srcModuleID){
    case TEMP_TASK:
        UARTprintf("{Temperature:%f Unit:%c Connected:%u}",log->data.temperature.data.floatingpoint, log->data.temperature.unit, log->data.temperature.connected);
        break;
    case LED_TASK:
        UARTprintf("{Count:%u Name:%s}",log->data.led.count, log->data.led.name);
        break;
    default:
        break;

    };
    UARTprintf("\r\n\r\n");
}


int LOG_ENQUEUE(log_level_t level, moduleId_t modId, void *data, char *msg, ...)
{
    log_struct_t send_log;
    send_log.level = level;
    send_log.timestamp = getTimeMsec();
    send_log.srcModuleID = modId;
    switch(modId){

    case TEMP_TASK:
        send_log.data.temperature = *(temp_data_t*)data;
        break;
    case LED_TASK:
        send_log.data.led = *(led_data_t*)data;
        break;
    default:
        break;

    }
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    vsnprintf(send_log.msg, sizeof(send_log.msg), msg, arg_ptr);
    va_end(arg_ptr);
    if(xQueueSend( logger_queue, ( void * ) &send_log, ( TickType_t ) 0 ) != pdTRUE)
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

static void myLoggerTask(void *params)
{
    log_struct_t recv_log;
    while(1)
    {
        // Receive a message on the created queue.  Block for 10 ticks if a
        // message is not immediately available.
        if( xQueueReceive(logger_queue, &recv_log, (TickType_t)1000 ) == pdPASS )
        {
            printlog(&recv_log);
        }
    }
}

uint32_t LoggerTaskInit(void)
{
    //create queue for logging
    InitConsole(BAUDRATE);
    logger_queue = xQueueCreate(20,sizeof(log_struct_t));
    if(logger_queue == NULL )
    {
        return 1;
    }
    if(xTaskCreate(myLoggerTask, (const portCHAR *)"LOGGER", STACKSIZE_LOGGERTASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LOGGER_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    return (0);
}
