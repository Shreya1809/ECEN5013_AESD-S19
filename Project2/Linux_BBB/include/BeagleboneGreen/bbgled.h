/**
 * @file bbgled.h
 * @author Shreya Chakraborty
 * @brief header file for external led functionality connected to BBG
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef BBGLED_H_
#define BBGLED_H_
#include <stdbool.h>
/**
 * @brief turn on off red led
 * 
 * @param option 
 * @return int 
 */
int redLed_OnOff(bool option);
/**
 * @brief turn on off green led 
 * 
 * @param option 
 * @return int 
 */
int greenLed_OnOff(bool option);

#define REDLEDON()      redLed_OnOff(1)
#define REDLEDOFF()     redLed_OnOff(0)
#define GREENLEDON()    greenLed_OnOff(1) 
#define GREENLEDOFF()   greenLed_OnOff(0)

#endif