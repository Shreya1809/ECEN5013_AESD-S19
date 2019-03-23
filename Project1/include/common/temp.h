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
    default_unit = 0,
    celcius,
    kelvin,
    Farenheit
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
float getTemp(temp_unit unit);

#define TEMP_KELVIN()       getTemp(kelvin)
#define TEMP_CELCIUS()      getTemp(celcius)
#define TEMP_FARENHEIT()    getTemp(Farenheit)

#endif