/*
 * i2c.c
 *
 *  Created on: Apr 25, 2019
 *      Author: Shreya
 */


#include "i2c.h"

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
    //motor pin
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);

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
   data_one &= 0xFF;

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
   data_two &= 0xFF;

   return (((data_one << 8) | data_two) >> 4);
}

void I2CSendByte(uint8_t target_address, uint8_t register_address, uint8_t data)
{
   //
   // Tell the master module what address it will place on the bus when
   // communicating with the slave.  Set the address to LUX_SENSOR
   // (as set in the slave module).  The receive parameter is set to false
   // which indicates the I2C Master is initiating a writes to the slave.  If
   // true, that would indicate that the I2C Master is initiating reads from
   // the slave.
   //
   I2CMasterSlaveAddrSet(I2C2_BASE, target_address, false);

   //
   // Place the data to be sent in the data register
   //
   I2CMasterDataPut(I2C2_BASE, register_address);

   //
   // Initiate send of data from the master.
   //
   I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C2_BASE));
#endif

   /*
   ** now wait for it to be non-busy
   */
   while(I2CMasterBusy(I2C2_BASE));

   //
   // Place the data to be sent in the data register
   //
   I2CMasterDataPut(I2C2_BASE, data);

   //
   // Initiate send of data from the master.
   //
   I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C2_BASE));
#endif
   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C2_BASE));

}

uint8_t I2CGetByte(uint8_t target_address, uint8_t register_address)
{
   //
   // Tell the master module what address it will place on the bus when
   // communicating with the slave.  Set the address to LUX_SENSOR
   // (as set in the slave module).  The receive parameter is set to false
   // which indicates the I2C Master is initiating a writes to the slave.  If
   // true, that would indicate that the I2C Master is initiating reads from
   // the slave.
   //
   I2CMasterSlaveAddrSet(I2C2_BASE, target_address, false);

   //
   // Place the data to be sent in the data register
   //
   I2CMasterDataPut(I2C2_BASE, register_address);

   //
   // Initiate send of data from the master.  Since the loopback
   // mode is enabled, the master and slave units are connected
   // allowing us to receive the same data that we sent out.
   //
   I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C2_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C2_BASE));

   /*
   ** now switch to read mode
   */
   I2CMasterSlaveAddrSet(I2C2_BASE, target_address, true);

   /*
   ** read one byte
   */
   I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

#ifdef BUSY_WAIT
   //
   // Wait until master module is says it's busy. Errata I2C#08
   //
   while(!I2CMasterBusy(I2C2_BASE));
#endif

   //
   // Wait until master module is done
   //
   while(I2CMasterBusy(I2C2_BASE));

   return I2CMasterDataGet(I2C2_BASE);
}

