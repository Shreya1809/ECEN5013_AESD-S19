/*
 * pwm.h
 *
 *  Created on: Apr 13, 2019
 *      Author: Shreya
 */

#ifndef PWM_H_
#define PWM_H_

typedef enum
{
    DC_OFF = 0,
    DC_100_PERCENT = 1,
    DC_50_PERCENT,
    DC_66_PERCENT,
    DC_25_PERCENT,
    DC_20_PERCENT,
    DC_16_PERCENT,
    DC_14_PERCENT,
    DC_12_5_PERCENT,
    DC_11_PERCENT,
    DC_10_PERCENT,
    DC_9_PERCENT,
    DC_8_PERCENT,
    DC_7_PERCENT,
    DC_6_PERCENT,
}dutycycle_t;

void timer_Init(void);

void generate_PWM( uint32_t val , dutycycle_t DC);

void gpio_pwm_init(void);

void gpio_pwm(int delayMS);


#endif /* PWM_H_ */
