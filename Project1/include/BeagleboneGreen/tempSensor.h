/**
 * @file tempSensor.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-22
 * @reference https://github.com/torvalds/linux/blob/master/drivers/hwmon/tmp102.c
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H


/* Register address */
#define TMP102_SLAVE_ADDRESS            (0x48)
#define TMP102_TEMP_REG                 (0x00)
#define TMP102_CONFIG_REG               (0x01)
#define TMP102_TLOW_REG                 (0x02)
#define TMP102_THIGH_REG                (0x03)  
//config register is a 16 bit register with the following bit position byte1 msb followed by byte 2
#define		TMP102_CONF_SD		(1)  
#define		TMP102_CONF_TM		(1<<1)
#define		TMP102_CONF_POL		(1<<2)
#define		TMP102_CONF_F0		(1<<3)
#define		TMP102_CONF_F1		(1<<4)
#define		TMP102_CONF_R0		(1<<5)
#define		TMP102_CONF_R1		(1<<6)
#define		TMP102_CONF_OS		(1<<7)
#define		TMP102_CONF_EM		(1<<12)
#define		TMP102_CONF_AL		(1<<13)
#define		TMP102_CONF_CR0		(1<<14)
#define		TMP102_CONF_CR1		(1<<15)

#define TMP102_CONFREG_MASK	(TMP102_CONF_SD | TMP102_CONF_TM | \
				 TMP102_CONF_POL | TMP102_CONF_F0 | \
				 TMP102_CONF_F1 | TMP102_CONF_OS | \
				 TMP102_CONF_EM | TMP102_CONF_AL | \
				 TMP102_CONF_CR0 | TMP102_CONF_CR1)

#define TMP102_CONFIG_CLEAR	(TMP102_CONF_SD | TMP102_CONF_OS | \
				 TMP102_CONF_CR0)
#define TMP102_CONFIG_SET	(TMP102_CONF_TM | TMP102_CONF_EM | \
				 TMP102_CONF_CR1)
#define FAULTBITSMASK			0x1800

typedef enum{
	R0R1_SET,
	R1_SET,
	R0_SET,
	R0R1_CLEAR
}sensor_resolution_t;

typedef enum{
	ONE_FAULT,
	TWO_FAULTS,
	FOUR_FAULTS,
	SIX_FAULTS
}fault_setting_t;

typedef enum{
	NORMAL,
	EXTENDED
}EM_mode_t;

typedef enum{
	POWERSAVING,
	DEFAULT
}shutdown_mode_t;

typedef enum{
	ONE_FOURTH_HZ,
	ONE_HZ,
	FOUR_HZ,
	EIGHT_HZ
}conversion_rate_t;
/**
 * @brief 
 * 
 * @param temp_val 
 * @return int 
 */
int TMP102_getTemperature(float *temp_val);
/**
 * @brief read tlow reg
 * 
 * @param tlow_val 
 * @return int 
 */
int TMP102_getTlow(float *tlow_val);
/**
 * @brief read thigh register
 * 
 * @param thigh_val 
 * @return int 
 */
int TMP102_getThigh(float *thigh_val);
/**
 * @brief write to thigh reg
 * 
 * @param thigh_val 
 * @return int 
 */
int TMP102_setThigh(float thigh_val); // default value is 80 deg celcius 
/**
 * @brief write to tlow reg
 * 
 * @param tlow_val 
 * @return int 
 */
int TMP102_setTlow(float tlow_val); //default value is 75 deg celcius
/**
 * @brief read any reg
 * 
 * @param reg 
 * @param data 
 * @return int 
 */
int TMP102_readReg(uint8_t reg, uint8_t *data);
/**
 * @brief write to ptr reg 
 * 
 * @param reg 
 * @return int 
 */
int TMP102_writePTRreg(uint8_t reg);
/**
 * @brief set shutdown mode
 * 
 * @param option 
 * @return int 
 */
int TMP102_setShutdownMode(shutdown_mode_t option); //SD bit in config register default is DEFAULT
//sensor resolution R1/R0, option - 0 is default 12 bits
/**
 * @brief set resolution 
 * 
 * @param option 
 * @return int 
 */
int TMP102_setResolution(sensor_resolution_t option); //default R0R1 set
/**
 * @brief read fault bits
 * 
 * @param data 
 * @return int 
 */
int TMP102_getFaultbits(uint16_t *data); // default mode is F1 AND F0 clear
/**
 * @brief set fault bits
 * 
 * @param option 
 * @return int 
 */
int TMP102_setFaultbits(fault_setting_t option); //default is 1 fault
/**
 * @brief set EM mode
 * 
 * @param option 
 * @return int 
 */
int TMP102_setEM(EM_mode_t option);// default is normal mode
/**
 * @brief set conversion rate
 * 
 * @param option 
 * @return int 
 */
int TMP102_setCR(conversion_rate_t option);
/**
 * @brief read AL bit
 * 
 * @param bit 
 * @return int 
 */
int TMP102_readAL( uint8_t *bit);
/**
 * @brief set all registers to default value
 * 
 * @return int 
 */
int TMP102_setAllDefault();
#endif
