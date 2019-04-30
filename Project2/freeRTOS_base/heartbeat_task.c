//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2009-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the DK-TM4C129X Firmware Package.
//
//*****************************************************************************


#include <communicationSend_task.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "idle_task.h"
#include "logger_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "driverlib/sysctl.h"
#include "driverlib/watchdog.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c1294ncpdt.h"               // Definitions for interrupt and register assignments on Tiva C
#include "heartbeat_task.h"
#include "TCP_layer.h"
#include "lcd_driver.h"
//*****************************************************************************
//
// Flag to tell the watchdog interrupt handler whether or not to clear the
// interrupt (feed the watchdog).
//
//*****************************************************************************
volatile bool g_bFeedWatchdog = true;
extern uint32_t g_ui32SysClock;

//*****************************************************************************
//
// The stack size for the LED toggle task.
//
//*****************************************************************************
#define STACKSIZE_HBTASK       256

//*****************************************************************************
//
// The amount of time to delay between toggles of the LED.
//
//*****************************************************************************
#define HB_RATE_MS   1000

extern volatile bool BBGConnectedFlag;
extern volatile bool BBGConnectedState;
#define SendHeartbeatToServer()         COMM_SEND(Heartbeat, NULL)

static void myHeartBeatTask(void *params)
{
    uint8_t ledval1 = (uint8_t)0;
    uint8_t ledval2 = (uint8_t)GPIO_PIN_1;
    TickType_t xLastWakeTime = 0;
    const TickType_t xFrequency = pdMS_TO_TICKS(HB_RATE_MS);
    uint32_t count = 0;
    xLastWakeTime = xTaskGetTickCount();
    static uint32_t disconnectedCounter = 0;
    while(1)
    {
        ledval1 ^= (uint8_t)GPIO_PIN_0;
        ledval2 ^= (uint8_t)GPIO_PIN_1;
        GPIOPinWrite(GPIO_PORTN_BASE, (uint8_t)GPIO_PIN_0, ledval1);
        GPIOPinWrite(GPIO_PORTN_BASE, (uint8_t)GPIO_PIN_1, ledval2);
        if(count % 5 == 0)
        {
            if(BBGConnectedFlag == 1)
            {
                disconnectedCounter++;
                if(disconnectedCounter > 2)
                {
                    LOG_ERROR(HB_TASK, NULL, "BBG Disconnected");
                    OperationStateFlag |= CONN_DISCONNECTED_FLAG;
                    BBGConnectedState = false;
                    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, GPIO_PIN_3);
                    disconnectedCounter = 0;
                }
            }
            BBGConnectedFlag = 1;
            SendHeartbeatToServer();
        }
        //        if(count % 1 == 0)
        //        {
        LCD_update();
        //        }

        //updating the opertion state
        if(OperationStateFlag == 0x00)
        {
            setThisNodeCurrentOperation(NORMAL);
        }
        //updating the opertion state
        else if(OperationStateFlag == CONN_DISCONNECTED_FLAG)
        {
            setThisNodeCurrentOperation(CONN_DISCONNECTED);
        }
        else if((OperationStateFlag != (TEMP_DISCONNECTED_FLAG | ACCL_DISCONNECTED_FLAG | DIST_DISCONNECTED_FLAG)) && (OperationStateFlag & ((TEMP_DISCONNECTED_FLAG | ACCL_DISCONNECTED_FLAG | DIST_DISCONNECTED_FLAG))) )
        {
            setThisNodeCurrentOperation(DEGRADED_OPERATION);
        }
        else if(OperationStateFlag == 0x0F)
        {
            setThisNodeCurrentOperation(OUTOFSERVICE);
            LOG_INFO(MAIN_TASK, NULL, " -----OUTOFSERVICE----");
        }

        count++;
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }

}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
uint32_t HBTaskInit(void)
{
    //
    // Initialize the GPIO used to drive the user LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);

    //
    // Create the LED task.
    //
    if(xTaskCreate(myHeartBeatTask, (const portCHAR *)"HB", STACKSIZE_HBTASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_HB_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
void WATCHDOG_init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
    // Unlock Watchdog
    if(WatchdogLockState(WATCHDOG0_BASE) == true){ //Gets the state of the watchdog timer lock mechanism.
        WatchdogUnlock(WATCHDOG0_BASE); //Disables the watchdog timer lock mechanism.
    }
    //enabling intterupts
    IntEnable(INT_WATCHDOG);
    WatchdogIntEnable(WATCHDOG0_BASE); //Enables the watchdog timer interrupt.
    WatchdogIntTypeSet(WATCHDOG0_BASE, WATCHDOG_INT_TYPE_INT); //Sets the type of interrupt generated by the watchdog.

    //watchdog properties
    WatchdogReloadSet(WATCHDOG0_BASE, g_ui32SysClock); // 1 sec watchdog timer
    WatchdogResetEnable(WATCHDOG0_BASE); //Enables the watchdog timer reset.
    WatchdogLock(WATCHDOG0_BASE); //Enables the watchdog timer lock mechanism.
    WatchdogEnable(WATCHDOG0_BASE); //Enables the watchdog timer.
}

void WatchdogHandler(void){
    WatchdogIntClear(WATCHDOG0_BASE);
    //do something

}
