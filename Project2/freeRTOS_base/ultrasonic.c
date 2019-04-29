/*
 * dist_task.c
 *
 *  Created on: Apr 13, 2019
 *      Author: Shreya
 */

#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include <stdlib.h>                         // Library of Standard Datatype Conversions
#include "ultrasonic.h"
#include "inc/tm4c1294ncpdt.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_timer.h"                   // Defines and macros used when accessing the timer
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "driverlib/debug.h"                // Macros for assisting debug of the driver library
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/timer.h"                // Defines and macros for Timer API of driverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/uart.h"                 // Defines and Macros for the UART
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "driverlib/sysctl.h"
#include "semphr.h"
#include "pwm.h"
#include "logger.h"
/* -----------------------      Global Variables        --------------------- */
volatile bool Trigger = 1;        // Variable to control the Trigger Pin Switching
volatile uint32_t EchoDuration = 0;     // Variable to store duration for which Echo Pin is high
volatile int32_t ObstacleDist = 0;     // Variable to store distance of the Obstacle

extern uint32_t g_ui32SysClock;
SemaphoreHandle_t xdistCalculationDoneSignal;

void Dist_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    // Master interrupt enable API for all interrupts
    IntMasterEnable();
    // Set the PE0 port as Output. Trigger Pin
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0);
    //for the buzzer
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);
    // Set the PE1 port as Input with a weak Pull-down. Echo Pin
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);
    // Configure and enable the Interrupt on both edges for PE1. Echo Pin
    IntEnable(INT_GPIOE);
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_INT_PIN_1);

    // Configure Timer0 to run in one-shot down-count mode
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);
    // Enable the Interrupt specific vector associated with Timer0A
    IntEnable(INT_TIMER0A);
    // Enables a specific event within the timer to generate an interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Configure Timer2 to run in one-shot up-count mode
    TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT_UP);

    xdistCalculationDoneSignal = xSemaphoreCreateBinary();

}

int32_t get_Dist(void)
{
    int32_t ret;
    if (Trigger){
        // Load the Timer with value for generating a  delay of 10 uS.
        TimerLoadSet(TIMER0_BASE, TIMER_A, (g_ui32SysClock / 100000) -1);
        // Make the Trigger Pin (PE0) High
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_PIN_0);
        // Enable the Timer0 to cause an interrupt when timeout occurs
        TimerEnable(TIMER0_BASE, TIMER_A);
        // Disable the condition for Trigger Pin Switching
        Trigger = 0;
    }
    BaseType_t wait = xSemaphoreTake( xdistCalculationDoneSignal, ( TickType_t ) 50 );
    if(wait != pdTRUE){
        TimerDisable(TIMER2_BASE, TIMER_A);
        Trigger = 1;
        LOG_ERROR(DIST_TASK, NULL, "Semaphore Take timeout");
        return -1;
    }
    ret = ObstacleDist;
    return ret;
}
void Timer0IntHandler(void){
    // The ISR for Timer0 Interrupt Handling
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Disable the timer
    TimerDisable(TIMER0_BASE, TIMER_A);
    // Make the Trigger Pin (PE0) Low
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 0x00);
}

void PortEIntHandler(void){
    // The ISR for GPIO PortE Interrupt Handling
    // Clear the GPIO Hardware Interrupt
    GPIOIntClear(GPIO_PORTE_BASE , GPIO_INT_PIN_1);

    // Condition when Echo Pin (PE0) goes high
    if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) == GPIO_PIN_1){
        // Initialize Timer2 with value 0
        HWREG(TIMER2_BASE + TIMER_O_TAV) = 0;
        // Enable Timer2 to start measuring duration for which Echo Pin is High
        TimerEnable(TIMER2_BASE, TIMER_A);
    }
    else{
        EchoDuration = TimerValueGet(TIMER2_BASE, TIMER_A);
        // Disable Timer2 to stop measuring duration for which Echo Pin is High
        TimerDisable(TIMER2_BASE, TIMER_A);
        // Convert the Timer Duration to Distance Value according to Ultrasonic's formula
        ObstacleDist = EchoDuration / 928;
        Trigger = 1;
        xSemaphoreGive(xdistCalculationDoneSignal);
    }

}

