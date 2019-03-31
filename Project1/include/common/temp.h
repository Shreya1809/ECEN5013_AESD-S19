/**
 * @file temp.h
 * @author Shreya Chakraborty
 * @brief header file for temperature task thread functionality
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
 * @brief to kill temp thread
 * Sets the kill thread in the task
 * 
 */
void kill_temp_thread(void);
/**
 * @brief call back for temperature task 
 * waits on a semaphore from the timer periodically
 * @param threadp 
 * @return void* 
 */
void *temp_task(void *threadp);
/**
 * @brief Get the Temp object.
 * Gets the temperature value from the temp register
 * 
 * @param unit 
 * @return float 
 */
float getTemperature(temp_unit unit);
/**
 * @brief Construct a new tmp102 settempthreshold object
 * sets thresholds for the temperature val
 * @param tlow_val 
 * @param thigh_val 
 */
int TMP102_setTempThreshold(float tlow_val,float thigh_val);
/**
 * @brief take threshold values from remote client
 * 
 * @param flow 
 * @param fhigh 
 * @return int 
 */
int RemoteThresholdValues(float flow,float fhigh);

#define GET_TEMP_KELVIN()       getTemperature(KELVIN)
#define GET_TEMP_CELCIUS()      getTemperature(CELCIUS)
#define GET_TEMP_FARENHEIT()    getTemperature(FARENHEIT)

#endif