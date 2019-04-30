/*
 * temp_task.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Shreya
 */

#ifndef TMP102_H_
#define TMP102_H_

/* Register address */
#define TMP102_SLAVE_ADDRESS            (0x48)
#define TMP102_TEMP_REG                 (0x00)
#define TMP102_CONFIG_REG               (0x01)
#define TMP102_TLOW_REG                 (0x02)
#define TMP102_THIGH_REG                (0x03)
//config register is a 16 bit register with the following bit position byte1 msb followed by byte 2
#define     TMP102_CONF_SD      (1)
#define     TMP102_CONF_TM      (1<<1)
#define     TMP102_CONF_POL     (1<<2)
#define     TMP102_CONF_F0      (1<<3)
#define     TMP102_CONF_F1      (1<<4)
#define     TMP102_CONF_R0      (1<<5)
#define     TMP102_CONF_R1      (1<<6)
#define     TMP102_CONF_OS      (1<<7)
#define     TMP102_CONF_EM      (1<<12)
#define     TMP102_CONF_AL      (1<<13)
#define     TMP102_CONF_CR0     (1<<14)
#define     TMP102_CONF_CR1     (1<<15)

#define TMP102_CONFREG_MASK (TMP102_CONF_SD | TMP102_CONF_TM | \
                 TMP102_CONF_POL | TMP102_CONF_F0 | \
                 TMP102_CONF_F1 | TMP102_CONF_OS | \
                 TMP102_CONF_EM | TMP102_CONF_AL | \
                 TMP102_CONF_CR0 | TMP102_CONF_CR1)

#define TMP102_CONFIG_CLEAR (TMP102_CONF_SD | TMP102_CONF_OS | \
                 TMP102_CONF_CR0)
#define TMP102_CONFIG_SET   (TMP102_CONF_TM | TMP102_CONF_EM | \
                 TMP102_CONF_CR1)
#define FAULTBITSMASK           0x1800

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
    NORMAL_MODE,
    EXTENDED_MODE
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
 * @brief get temperature raw value
 *
 * @param temp_val
 * @return int
 */
float  TMP102_getTemperature(void);

#endif /* TMP102_H_ */
