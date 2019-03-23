/**
 * @file lightSensor.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-22
 * @reference : https://github.com/sparkfun/APDS-9301_Breakout/blob/master/Libraries/Arduino/src/Sparkfun_APDS9301_Library.h
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef LIGHTSENSOR_H_
#define LIGHTSENSOR_H_

#define APDS9301_SLAVE_ADDRESS      (0x39)
#define APDS9301_CONTROL_REG        (0x80)
#define APDS9301_TIMING_REG         (0x81)
#define APDS9301_THRESHLOWLOW_REG   (0x82)
#define APDS9301_THRESHLOWHI_REG    (0x83)
#define APDS9301_THRESHHILOW_REG    (0x84)
#define APDS9301_THRESHHIHI_REG     (0x85)
#define APDS9301_INTERRUPT_REG      (0x86)
#define APDS9301_ID_REG             (0x8A)
#define APDS9301_CH0_DATALOW_REG    (0x8C)
#define APDS9301_CH0_DATAHIGH_REG   (0x8D)
#define APDS9301_CH1_DATALOW_REG    (0x8E)
#define APDS9301_CH1_DATAHIGH_REG   (0x8F)

#define APDS9301_COMMAND_WORD_RW    (0x20)
#define APDS9301_COMMAND_INT_CLEAR  (0x40)

#define APDS9301_CONTROL_POWER_DOWN  (0x00)
#define APDS9301_CONTROL_POWER_UP    (0x03)
#define APDS9301_INTERRUPT_CTRL_ENABLE_DISABLE        (0x10)   
#define APDS9301_TIMING_GAIN        (0x10)
#define APDS9301_TIMING_INTEGRATION_TIME(i)    (i)
#define APDS9301_TIMING_MANUAL(i)   (i<<3)


#endif