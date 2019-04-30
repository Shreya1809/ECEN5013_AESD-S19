/*
 * sensors_task.h
 *
 *  Created on: Apr 22, 2019
 *      Author: Shreya
 */

#ifndef SENSORS_TASK_H_
#define SENSORS_TASK_H_

#include "includes.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "priorities.h"
#include "idle_task.h"
#include "logger_task.h"
#include "adxl345.h"
#include "ultrasonic.h"
#include "TMP102.h"
#include "pwm.h"
#include "i2c.h"

//typedef enum
//{
//    Ultrasonic,
//    Accelerometer,
//    Temperature
//
//}sensor_type_t;


#define OFF false
#define ON  true
typedef bool state_t;

void DCmotor(state_t option);
void setReversGear(bool state);
bool getReversGear();
void setTemperatureThreshold(float temp);
float getTemperatureThreshold();
void setAccelDeltaThreshold(accel_data_t *accel);
void getAccelDeltaThreshold(accel_data_t *accel);

//bool BISTcheckSensorConnection(sensor_type_t sensor);
uint32_t SensorTaskInit(void);
#endif /* SENSORS_TASK_H_ */
