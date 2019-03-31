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
#define APDS9301_TIMING_INTEGRATION_TIME_BIT0 (0x01)
#define APDS9301_TIMING_INTEGRATION_TIME_BIT1 (0x02)


typedef enum{
    LOW,
    HIGH
}gain_mode_t;

typedef enum{
    MODE_ZERO,
    MODE_ONE,
    MODE_TWO,
    MODE_THREE,
}integ_time_t;

typedef enum{
    ENABLE,
    DISABLE
}interrupt_mode_t;
/**
 * @brief get ADC channel0 data
 * 
 * @param data 
 * @return int 
 */
int APDS9301_getCh0(uint16_t *data);
/**
 * @brief get ADC channel1 data
 * 
 * @param data 
 * @return int 
 */
int APDS9301_getCh1(uint16_t *data);
/**
 * @brief clears pending inteerupt
 * 
 * @return int 
 */
int APDS9301_intClear(void);
/**
 * @brief 
 * 
 * @param data 
 * @return int 
 */
int APDS9301_CheckInt(uint8_t *data);
/**
 * @brief combine channel0 and 1 data to get lux
 * 
 * @param lux 
 * @return int 
 */
int APDS9301_getlight(float *lux);
/**
 * @brief read light id register
 * 
 * @param data 
 * @return int 
 */
int APDS9301_readIDreg(uint8_t *data);

/**
 * @brief 
 * 
 * @return int 
 */
int APDS9301_powerup();
/**
 * @brief write to command register
 * 
 * @param data 
 * @return int 
 */
int APDS9301_writeCMDreg(uint8_t data); // also write command register
/**
 * @brief read control register
 * 
 * @param data 
 * @return int 
 */
int APDS9301_readCTRLreg(uint8_t *data);
/**
 * @brief set gain in timing register
 * 
 * @param gain 
 * @return int 
 */
int APDS9301_setTiming_gain(gain_mode_t gain); //default value is low
/**
 * @brief set integration time in timing register
 * 
 * @param integTime 
 * @return int 
 */
int APDS9301_setTiming_integ(integ_time_t integTime);//mode 2 is default
/**
 * @brief set interrupt enable disable
 * 
 * @param option 
 * @return int 
 */
int APDS9301_interruptCTRLreg(interrupt_mode_t option); //default is disabled
/**
 * @brief write threshhold low low register
 * 
 * @param th_lowlow 
 * @return int 
 */
int APDS9301_writeTHRESH_lowlow(uint16_t th_lowlow);
/**
 * @brief read threshold low low register
 * 
 * @param th_lowlow 
 * @return int 
 */
int APDS9301_readTHRESH_lowlow(uint8_t *th_lowlow);
/**
 * @brief write threshold high low register
 * 
 * @param th_hilow 
 * @return int 
 */
int APDS9301_writeTHRESH_highlow(uint16_t th_hilow);
/**
 * @brief read threshold high low register
 * 
 * @param th_hilow 
 * @return int 
 */
int APDS9301_readTHRESH_highlow(uint8_t *th_hilow);
/**
 * @brief high high thres register
 * 
 * @param th_hihi 
 * @return int 
 */
int APDS9301_readTHRESH_highhigh(uint8_t *th_hihi);
/**
 * @brief low high threshold register
 * 
 * @param th_lowhi 
 * @return int 
 */
int APDS9301_readTHRESH_lowhigh(uint8_t *th_lowhi);
/**
 * @brief set all register values default
 * 
 * @return int 
 */
int APDS9301_setAllDefault();


#endif