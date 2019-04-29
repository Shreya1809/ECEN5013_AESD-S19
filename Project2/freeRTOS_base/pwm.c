/*
 * pwm.c
 *
 *  Created on: Apr 13, 2019
 *      Author: Shreya
 */

#include "includes.h"

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"



#include "pwm.h"

extern uint32_t g_ui32SysClock;
#define STACKSIZE_OUTPUTTASK 128

extern int delayMS;

void timer_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }
    GPIOPinConfigure(GPIO_PD4_T3CCP0);
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_4);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3))
    {
    }
    TimerClockSourceSet(TIMER3_BASE, TIMER_CLOCK_PIOSC);
    //
    // Configure Timer1B as a 16-bit periodic timer.
    //| TIMER_CFG_A_ACT_TOGGLE
    TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM );
}

void generate_PWM( uint32_t val , dutycycle_t DC)
{
    TimerDisable(TIMER3_BASE, TIMER_A);
    if(DC == DC_OFF){
        GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_4);
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_4, 0);
        return;
    }
//    GPIOPinConfigure(GPIO_PD4_T3CCP0);
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_4);
    //timer_Init();
    // Set the Timer1B load value to val.  For this example a % duty cycle
    // PWM signal will be generated.  From the load value (i.e. 50000) down to
    // match value (set below) the signal will be high.  From the match value

    TimerPrescaleSet(TIMER3_BASE, TIMER_A, (val & 0xFF0000) >> 16);
    TimerLoadSet(TIMER3_BASE, TIMER_A, (val & 0xFFFF)); //65535

    uint32_t matchval = TimerLoadGet(TIMER3_BASE, TIMER_A);
    uint32_t presc = TimerPrescaleGet(TIMER3_BASE, TIMER_A);
    matchval = matchval | (presc<<16); //matchval = FFFFFF

    if(DC >= DC_50_PERCENT){
        matchval = (matchval/DC); //7fffff
    }
    else{
        matchval = matchval - (matchval/DC); //7fffff
    }
    TimerPrescaleMatchSet(TIMER3_BASE, TIMER_A, (matchval & 0xFF0000) >> 16); //7f
    TimerMatchSet(TIMER3_BASE, TIMER_A, (matchval & 0xFFFF)); //65535
    TimerEnable(TIMER3_BASE, TIMER_A);

}

void gpio_pwm_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_4);
}

void gpio_pwm(int delayMS)
{
    GPIOPinWrite(GPIO_PORTD_BASE, (uint8_t)GPIO_PIN_4,GPIO_PIN_4 );
    SysCtlDelay((g_ui32SysClock/ (1000 * 3))*delayMS);
    GPIOPinWrite(GPIO_PORTD_BASE, (uint8_t)GPIO_PIN_4,0 );
    SysCtlDelay((g_ui32SysClock/ (1000 * 3))*delayMS);
}

