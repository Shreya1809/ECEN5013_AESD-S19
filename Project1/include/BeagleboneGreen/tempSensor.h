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
#define		TMP102_CONF_SD		0x0100  
#define		TMP102_CONF_TM		0x0200
#define		TMP102_CONF_POL		0x0400
#define		TMP102_CONF_F0		0x0800
#define		TMP102_CONF_F1		0x1000
#define		TMP102_CONF_R0		0x2000
#define		TMP102_CONF_R1		0x4000
#define		TMP102_CONF_OS		0x8000
#define		TMP102_CONF_EM		0x0010
#define		TMP102_CONF_AL		0x0020
#define		TMP102_CONF_CR0		0x0040
#define		TMP102_CONF_CR1		0x0080

#define TMP102_CONFREG_MASK	(TMP102_CONF_SD | TMP102_CONF_TM | \
				 TMP102_CONF_POL | TMP102_CONF_F0 | \
				 TMP102_CONF_F1 | TMP102_CONF_OS | \
				 TMP102_CONF_EM | TMP102_CONF_AL | \
				 TMP102_CONF_CR0 | TMP102_CONF_CR1)

#define TMP102_CONFIG_CLEAR	(TMP102_CONF_SD | TMP102_CONF_OS | \
				 TMP102_CONF_CR0)
#define TMP102_CONFIG_SET	(TMP102_CONF_TM | TMP102_CONF_EM | \
				 TMP102_CONF_CR1)

#endif
