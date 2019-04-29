/*
 * ldc.c
 *
 *  Created on: Apr 16, 2019
 *      Author: Shreya
 */


#include "includes.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "lcd_driver.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "logger_task.h"

#define LCD_MAX_ROW 2
#define LCD_MAX_COLUMN  16
static char LCDBuffer[LCD_MAX_ROW][LCD_MAX_COLUMN+1] = {0};

#define REGISTER_SELECT    GPIO_PIN_0
#define ENABLE             GPIO_PIN_1
#define DATA4              GPIO_PIN_4
#define DATA5              GPIO_PIN_5
#define DATA6              GPIO_PIN_6
#define DATA7              GPIO_PIN_7
//R/W connected to ground, write functionality only

static void LCD_command(char command)
{
    //first 4 bits of command
    GPIOPinWrite(GPIO_PORTM_BASE, DATA4 | DATA5 | DATA6 | DATA7, (command & 0xF0));
    GPIOPinWrite(GPIO_PORTM_BASE, REGISTER_SELECT, 0x00);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x02);
    SysCtlDelay(50000);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x00);
    SysCtlDelay(50000);
    //next 4 bits
    GPIOPinWrite(GPIO_PORTM_BASE, DATA4 | DATA5 | DATA6 | DATA7, (command & 0x0F) << 4);
    GPIOPinWrite(GPIO_PORTM_BASE, REGISTER_SELECT, 0x00);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x00);
    SysCtlDelay(50000);
}

static void LCD_putchar(char character)
{
    GPIOPinWrite(GPIO_PORTM_BASE, DATA4 | DATA5 | DATA6 | DATA7, (character & 0xF0));
    GPIOPinWrite(GPIO_PORTM_BASE, REGISTER_SELECT, 0x01); //write data to address
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x02);
    SysCtlDelay(50000);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x00);
    SysCtlDelay(50000);

    GPIOPinWrite(GPIO_PORTM_BASE, DATA4 | DATA5 | DATA6 | DATA7, (character & 0x0F) << 4);
    GPIOPinWrite(GPIO_PORTM_BASE, REGISTER_SELECT, 0x01);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x00);
    SysCtlDelay(50000);
}

static void LCD_putstring(char *c)
{
    while (*c) {
        LCD_putchar(*c++);
    }
}

static void LCD_gotoXY(char row, char column) {
    if (row == 0) {
        LCD_command(0x80 + ((column) % 16));
    } else {
        LCD_command(0xC0 + ((column) % 16));
    }
}

static void LCD_home(void)
{
    LCD_command(0x02);
    SysCtlDelay(10);
}

void LCD_clear(void)
{
    memset(LCDBuffer,0, 34);
    LCD_command(0x01);
    SysCtlDelay(10);
}

void LCD_printString(int r, int c, const char *data)
{
    if(r < LCD_MAX_ROW)
    {
        int len = strlen(data);
        int dataLen = len <= (LCD_MAX_COLUMN - c) ? len : (LCD_MAX_COLUMN - c);
        strncpy(&LCDBuffer[r][c], data, dataLen);
    }
}

void LCD_update()
{
    LCD_gotoXY(0,0);
    LCD_putstring(&LCDBuffer[0][0]);
    LCD_gotoXY(1,0);
    LCD_putstring(&LCDBuffer[1][0]);
}



void LCD_updateCallback(TimerHandle_t timer)
{
    LCD_update();
}

void LCD_init(void) {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, 0xFF);
    //starting sequence 3 times 0x30
    SysCtlDelay(50000);
    GPIOPinWrite(GPIO_PORTM_BASE, REGISTER_SELECT, 0x00);
    for(int i = 0; i < 3; i++)
    {
        GPIOPinWrite(GPIO_PORTM_BASE, DATA4 | DATA5 | DATA6 | DATA7, 0x30);
        GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x02);
        SysCtlDelay(10);
        GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x00);
        SysCtlDelay(50000);
    }

    GPIOPinWrite(GPIO_PORTM_BASE, DATA4 | DATA5 | DATA6 | DATA7, 0x20); //cursor return home
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTM_BASE, ENABLE, 0x00);
    SysCtlDelay(50000);
    //Config commands
    LCD_command(0x28); //function set, 4 bit bus mode, DL is Low, N  is high 2 line display mode, F is low 5x8 dots format display
    LCD_command(0xC0); //set DDRAM address to AC
    LCD_command(0x06); //entry mode set, cursor moves to right
    LCD_command(0x80); //address to write data
    LCD_command(0x28); //function set
    LCD_command(0x0C); // display on, cursor,blink off
    LCD_clear(); //clear lcd
//    TimerHandle_t h_lcdUpdateTimer = xTimerCreate("LCDUpdate", 1000, pdTRUE, (void*)0, LCD_updateCallback);
//    if(h_lcdUpdateTimer == NULL )
//    {
//        /* The timer was not created. */
//    }
//    else
//    {
//        /* Start the timer.  No block time is specified, and
//                 even if one was it would be ignored because the RTOS
//                 scheduler has not yet been started. */
//        if( xTimerStart(h_lcdUpdateTimer, 0 ) != pdPASS )
//        {
//            /* The timer could not be set into the Active
//                     state. */
//        }
//    }

}
