/*
 * adxl345.c
 *
 *  Created on: Apr 17, 2019
 *      Author: Shreya
 */

#include "includes.h"
#include "adxl345.h"
#include "spi.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "logger_task.h"
#include "driverlib/gpio.h"
#include "hw_memmap.h"

#define ADXL345_CS_EN()     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
#define ADXL345_CS_DIS()    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);

void adxl345_setBW(void)
{

    uint8_t setupBW[2];
    setupBW[0] = ADXL345_BW_RATE;
    setupBW[1] = ADXL345_BW_1600;
    ADXL345_CS_EN();
    spi_WriteBytes(setupBW, 2);
    ADXL345_CS_DIS();
}

void adxl345_setDataFormat(void)
{

    uint8_t setupDF[2];
    setupDF[0] =  ADXL345_DATA_FORMAT;
    setupDF[1] = ADXL345_BW_100;
    ADXL345_CS_EN();
    spi_WriteBytes(setupDF, 2);
    ADXL345_CS_DIS();
}

void adxl345_setPowerCTL(void)
{

    uint8_t setupPC[2];
    setupPC[0] =  ADXL345_POWER_CTL;
    setupPC[1] = ADXL345_BW_12_5;
    ADXL345_CS_EN();
    spi_WriteBytes(setupPC, 2);
    ADXL345_CS_DIS();
}

uint8_t adxl345_DeviceID()
{
    uint8_t data = 0;
    ADXL345_CS_EN();
    spi_WriteByte(ADXL345_DEVID | ADXL345_READ_BIT);
    spi_ReadBytes(&data, 1);
    ADXL345_CS_DIS();
    return data;

}

void adxl345_setup(void)
{
    spi_Init();
    ADXL345_CS_DIS();
    adxl345_setBW();
    adxl345_setDataFormat();
    adxl345_setPowerCTL();
    adxl345_FreeFall();
}

bool adxl345_checkINT(interrupt_t int_type)
{
    uint8_t data = 0;
   ADXL345_CS_EN();
   spi_WriteByte(ADXL345_INT_SOURCE | ADXL345_READ_BIT);
   spi_ReadBytes(&data, 1);
   ADXL345_CS_DIS();
   if(data & (1 << int_type))
   {
       return true;
   }
   else return false;
}
void adxl345_FreeFall(void)
{
    uint8_t setupFFH[2] = {0};
    uint8_t setupFFT[2] = {0};
    uint8_t setupFFI[2] = {0};
    uint8_t setupFFIM[2] = {0};
    setupFFI[0] =  ADXL345_INT_ENABLE;
    setupFFI[1] = ADXL345_INT_FREE_FALL_BIT;
    ADXL345_CS_EN();
    spi_WriteBytes(setupFFI, 2);
    ADXL345_CS_DIS();
    setupFFIM[0] =  ADXL345_INT_MAP;
    setupFFIM[1] = ADXL345_FREE_FALL;
    ADXL345_CS_EN();
    spi_WriteBytes(setupFFIM, 2);
    ADXL345_CS_DIS();
    setupFFH[0] =  ADXL345_THRESH_FF;
    setupFFH[1] = 0X05;
    ADXL345_CS_EN();
    spi_WriteBytes(setupFFH, 2);
    ADXL345_CS_DIS();
    setupFFT[0] =  ADXL345_TIME_FF;
    setupFFT[1] = 0X05;
    ADXL345_CS_EN();
    spi_WriteBytes(setupFFT, 2);
    ADXL345_CS_DIS();
}

bool get_Accel(int16_t *x, int16_t *y, int16_t *z)
{
        uint8_t receive_buffer[6] = {0};

        ADXL345_CS_EN();
        spi_WriteByte(ADXL345_DATAX0 | ADXL345_READ_BIT);
        receive_buffer[0] = spi_ReadByte();
        ADXL345_CS_DIS();

        while(SSIBusy(SSI2_BASE)){}

        ADXL345_CS_EN();
        spi_WriteByte(ADXL345_DATAX1 | ADXL345_READ_BIT);
        receive_buffer[1] = spi_ReadByte();
        ADXL345_CS_DIS();

        while(SSIBusy(SSI2_BASE)){}

        *x = (((int16_t)receive_buffer[1]) << 8) | receive_buffer[0];
        //*x = x_value_raw*0.00390625; //full resolution mode scaling

        ADXL345_CS_EN();
        spi_WriteByte(ADXL345_DATAY0 | ADXL345_READ_BIT);
        receive_buffer[2] = spi_ReadByte();
        ADXL345_CS_DIS();

        while(SSIBusy(SSI2_BASE)){}

        ADXL345_CS_EN();
        spi_WriteByte(ADXL345_DATAY1 | ADXL345_READ_BIT);
        receive_buffer[3] = spi_ReadByte();
        ADXL345_CS_DIS();

        while(SSIBusy(SSI2_BASE)){}

        *y = (((int16_t)receive_buffer[3]) << 8) | receive_buffer[2];
        //*y = y_value_raw*0.00390625; //full resolution mode scaling
        ADXL345_CS_EN();
        spi_WriteByte(ADXL345_DATAZ0 | ADXL345_READ_BIT);
       receive_buffer[4] = spi_ReadByte();
        ADXL345_CS_DIS();

        while(SSIBusy(SSI2_BASE)){}

        ADXL345_CS_EN();
        spi_WriteByte(ADXL345_DATAZ1 | ADXL345_READ_BIT);
        receive_buffer[5] = spi_ReadByte();
        ADXL345_CS_DIS();
//
        while(SSIBusy(SSI2_BASE)){}
        *z = (((int16_t)receive_buffer[5]) << 8) | receive_buffer[4];
        //*z = z_value_raw*0.00390625; //full resolution mode scaling

        if(((*x == -1) && (*y == -1) && (*z == -1)) || ((*x == 0) && (*y == 0) && (*z == 0)))
        {
            return false;
        }

        else
        {
            return true;
        }
}




