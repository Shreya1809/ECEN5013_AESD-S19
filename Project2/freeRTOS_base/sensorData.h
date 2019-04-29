/*
 * sensorData.h
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */

#ifndef SENSORDATA_H_
#define SENSORDATA_H_

#include <stdbool.h>
#include <stdint.h>


typedef char unit_t;
typedef struct temp{

    union{
        struct {
            int16_t mantissa;
            uint16_t exponent;
        }fixedpoint;
        float floatingpoint;
    };
    bool connected;
    unit_t unit;
}temp_data_t;

typedef struct accel{

    uint8_t devId;
    int16_t x;
    int16_t y;
    int16_t z;
    bool connected;
    bool INT2;

}accel_data_t;

typedef struct dist{
    int32_t distance;
    bool connected;
}dist_data_t;

//typedef struct led{
//    uint32_t count;
//    char *name;
//}led_data_t;


#endif /* SENSORDATA_H_ */
