/**
 * @file light.h
 * @author Shreya Chakraborty
 * @brief header file for the light Sensor task functionality
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef LIGHT_H_
#define LIGHT_H_
/* mraa header */
#include "mraa/gpio.h"
/**
 * @brief sets the threshold values for the light sensor
 * 
 * @param l_low 
 * @param l_high 
 * @return int 
 */
int APDS9301_setLightThreshold(uint16_t l_low,uint16_t l_high);
/**
 * @brief gets the streshold values from the remote client 
 * and changes the default values 
 * 
 * @param llow 
 * @param lhigh 
 * @return int 
 */
int RemoteThresholdValueslight(uint16_t llow,uint16_t lhigh);
/**
 * @brief checks the INT pin on the APDS9301 sensor
 * 
 * @param lightSensorIntPin 
 * @return int 
 */
int APDS9301_checkINTPIN(mraa_gpio_context lightSensorIntPin);
/**
 * @brief pin setup using gpio mraa library
 * 
 */
mraa_gpio_context APDS9301_IntPinSetup(int mraaPinNumber);
/**
 * @brief to kill light thread
 * sets the stop thread flag
 * 
 */
void kill_light_thread(void);
/**
 * @brief Call back for light task from main thread
 * 
 * @param threadp 
 * @return void* 
 */
void *light_task(void *threadp);
/**
 * @brief Get the Light object
 * 
 * @return float 
 */
float getLight(void);


#define GETLUX()        getLight()
#endif