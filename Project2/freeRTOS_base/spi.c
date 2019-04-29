/*
 * spi.c
 *
 *  Created on: Apr 13, 2019
 *      Author: Shreya
 */

#include "includes.h"

#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"
#include "driverlib/ssi.h"

#include "spi.h"

extern uint32_t g_ui32SysClock;

void spi_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2); // spi peripheral enable
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //gpio peripheral enable for spi
    GPIOPinConfigure(GPIO_PD3_SSI2CLK); //sck
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
//    GPIOPinConfigure(GPIO_PD2_SSI2FSS); //cs
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);//SDO->MOSI
    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);//SDI->MISO

    // The pins are assigned as follows:
    //      PD1 - SSI0Tx
    //      PD0 - SSI0Rx
    //      PD2 - SSI0Fss
    //      PD3 - SSI0CLK
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3);
    SSIConfigSetExpClk(SSI2_BASE, g_ui32SysClock, SSI_FRF_MOTO_MODE_3,
                               SSI_MODE_MASTER, 1000000, 8);
    SSIEnable(SSI2_BASE);
}


uint8_t spi_ReadByte()
{
    uint32_t data = 0;
    SSIDataPut(SSI2_BASE, 0xFF);
    SSIDataGet(SSI2_BASE, &data);
    return ((uint8_t)(data & 0x00FF));
}

void spi_WriteByte(uint8_t data)
{
    uint32_t write_data = ((uint32_t)data & 0x000000FF);
    SSIDataPut(SSI2_BASE, write_data);
    while(SSIBusy(SSI2_BASE));
    SSIDataGet(SSI2_BASE, &write_data);
}

void spi_WriteBytes(uint8_t* data, size_t size)
{
    size_t i=0;
    while ((data+i) && i<size)
    {
        spi_WriteByte(*(data+i));
        ++i;
    }
}

size_t spi_ReadBytes(uint8_t* data, size_t size)
{
    size_t i=0;
    while ((data+i) && i<size)
    {
        spi_WriteByte(0xFF); //dummy write
        *(data+i) = spi_ReadByte();//actual read
        ++i;
    }
    return i;
}

void spi_close(void)
{
    SysCtlPeripheralDisable(SYSCTL_PERIPH_SSI2); // spi peripheral enable
    SSIDisable(SSI2_BASE);
}
