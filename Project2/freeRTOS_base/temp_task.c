/*
 * temp_task.c
 *
 *  Created on: Apr 8, 2019
 *      Author: Shreya
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "idle_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "driverlib/sysctl.h"
#include "alert.h"
#include "temp_task.h"
#include "logger_task.h"
#include "led_task.h"


extern uint32_t g_ui32SysClock;
//macro for delay
#define TEMP_RATE_MS   2000
//*****************************************************************************
//
// The stack size for the Logger task.
//
//*****************************************************************************
#define STACKSIZE_TEMPTASK       128
//
//*****************************************************************************
//
extern xSemaphoreHandle Alertsem;

void InitI2C0(void)
{
    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    //enable GPIO peripheral that contains I2C 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, g_ui32SysClock, false);

    //clear I2C FIFOs
    //HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;



}

uint16_t I2CGet2Bytes(uint8_t target_address, uint8_t register_address)
{
   //
   // Tell the master module what address it will place on the bus when
   // communicating with the slave.  Set the address to LUX_SENSOR
   // (as set in the slave module).  The receive parameter is set to false
   // which indicates the I2C Master is initiating a writes to the slave.  If
   // true, that would indicate that the I2C Master is initiating reads from
   // the slave.
   //
   I2CMasterSlaveAddrSet(I2C0_BASE, target_address, false);

   //
   // Place the data to be sent in the data register
   //
   I2CMasterDataPut(I2C0_BASE, register_address);

   //
   // Initiate send of data from the master.  Since the loopback
   // mode is enabled, the master and slave units are connected
   // allowing us to receive the same data that we sent out.
   //
   I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C0_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C0_BASE));

   /*
   ** now switch to read mode
   */
   I2CMasterSlaveAddrSet(I2C0_BASE, target_address, true);

   /*
   ** read one byte
   */
   I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C0_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C0_BASE));

   uint32_t data_one = I2CMasterDataGet(I2C0_BASE);

   /*
   ** read one byte
   */
   I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C0_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C0_BASE));

   uint32_t data_two = I2CMasterDataGet(I2C0_BASE);

   return (((data_one << 8) | data_two) >> 4);
}

float TMP102_getTemperature(void)
{
    int16_t temperature = I2CGet2Bytes(TMP102_SLAVE_ADDRESS, TMP102_TEMP_REG);
    temperature &= 0xFFF;  // this should do sign extension

    float temp = 0.0;
    if(temperature & 0x800) //check for negative temperature
    {
        temperature = ((~temperature) + 1) & 0xFFF;
        temp = (-1) * temperature * 0.0625;
    }
    else{
        temp = 0.0625 * temperature;
    }

    return temp;
}

int readAndUpdateTemp(void)
{
    float ret = 0.0;
    ret = TMP102_getTemperature();
    return 0;
}


static void myTempTask(void *params)
{
    InitI2C0();
    //const TickType_t xDelay = pdMS_TO_TICKS(TEMP_RATE_MS);
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(TEMP_RATE_MS);
    xLastWakeTime = xTaskGetTickCount();
    temp_data_t temperaturedata;
    temperaturedata.data.floatingpoint = 0.0;
    temperaturedata.unit = 'C';
    temperaturedata.connected = true;
    while(1)
    {
        temperaturedata.data.floatingpoint = TMP102_getTemperature();
        if(temperaturedata.data.floatingpoint > 26 || temperaturedata.data.floatingpoint < 24) //thresholds for alert
        {
            xSemaphoreGive(Alertsem);
        }
        LOG_INFO(TEMP_TASK,&temperaturedata," Celcius");
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

uint32_t TempTaskInit(void)
{
    if(xTaskCreate(myTempTask, (const portCHAR *)"TEMP", STACKSIZE_TEMPTASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_TEMP_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    return (0);
}


