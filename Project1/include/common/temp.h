/**
 * @file temp.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef TEMP_H_
#define TEMP_H_


typedef enum 
{
    CELCIUS,
    KELVIN,
    FARENHEIT
}temp_unit;
/**
 * @brief call back for temperature task 
 * 
 * @param threadp 
 * @return void* 
 */
void *temp_task(void *threadp);
/**
 * @brief Get the Temp object
 * 
 * @param unit 
 * @return float 
 */
float getTemperature(temp_unit unit);

#define GET_TEMP_KELVIN()       getTemperature(KELVIN)
#define GET_TEMP_CELCIUS()      getTemperature(CELCIUS)
#define GET_TEMP_FARENHEIT()    getTemperature(FARENHEIT)

#endif