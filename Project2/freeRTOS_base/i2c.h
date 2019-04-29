/*
 * i2c.h
 *
 *  Created on: Apr 25, 2019
 *      Author: Shreya
 */

#ifndef I2C_H_
#define I2C_H_

#include "includes.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

extern uint32_t g_ui32SysClock;

void InitI2C0(void);
uint16_t I2CGet2Bytes(uint8_t target_address, uint8_t register_address);
void I2CSendByte(uint8_t target_address, uint8_t register_address, uint8_t data);
uint8_t I2CGetByte(uint8_t target_address, uint8_t register_address);

#endif /* I2C_H_ */
