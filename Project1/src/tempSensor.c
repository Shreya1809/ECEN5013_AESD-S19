#include "includes.h"
#include "tempSensor.h"
#include "myI2C.h"
#include "logger.h"
#include "main.h"


int TMP102_getTemperature(float *temp_val)
{
    uint8_t tempbuff[2] = {0};
    uint8_t MSB, LSB;
    int ret = I2C_read_bytes(&i2c_handler,TMP102_SLAVE_ADDRESS , TMP102_TEMP_REG, tempbuff, sizeof(tempbuff));
    if(ret == -1)
    {
        return ret;
    }

    uint16_t result = 0;
    MSB = tempbuff[0];
    LSB = tempbuff[1];
    result = ((MSB << 8) | LSB) >> 4;
    if(result & 0x800)
    {
        result = ((~result) + 1); 
        *temp_val = (-1) * result * 0.0625;
    }
    else{
        *temp_val = result * 0.0625;
    }
    return 0;
}
