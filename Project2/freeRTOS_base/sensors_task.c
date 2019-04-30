/*
 * sensors_task.c
 *
 *  Created on: Apr 22, 2019
 *      Author: Shreya
 */

#include <communicationSend_task.h>
#include "sensors_task.h"
#include "lcd_driver.h"

#define STACKSIZE_SENSORTASK    512
#define RATE_MS                 500
volatile int delayMS = 0;
extern uint32_t g_ui32SysClock;
int16_t prev_x = 0, prev_y = 0,prev_z = 0;

bool volatile inReverseGear = false;
bool volatile BBGConnectedFlag = false;
int32_t SensorDownFlag = 0;

void ControlNode_TempCommandHandler(temp_data_t temperaturedata, bool val, int disconnected)
{

    if(!disconnected)
    {
        if (val == true)
            {
                DCmotor(ON);
                LOG_INFO(TEMP_TASK,&temperaturedata,"***THRESHOLD EXCEEDED***");
            }
            else
            {
                DCmotor(OFF);
                LOG_INFO(TEMP_TASK,&temperaturedata," Celcius");
            }
    }
    else
    {
        LOG_WARN(TEMP_TASK,&temperaturedata , "DISCONNECTED");
    }

}

void ControlNode_AccelCommandHandler(accel_data_t readings, bool val, int disconnected)
{
    if(!disconnected)
    {
        if (val == true)
        {
            //DCmotor(ON);
            LOG_INFO(ACCEL_TASK,&readings,"***THRESHOLD EXCEEDED***");
        }
        else
        {
            LOG_INFO(ACCEL_TASK, &readings, " Accelerometer");
            //DCmotor(OFF);
        }
    }
    else
    {
        LOG_WARN(ACCEL_TASK,&readings , "DISCONNECTED");
    }

}

void ControlNode_DistCommandHandler(dist_data_t *data, int disconnected)
{
    if(!disconnected)
    {
        data->connected = true;
        if(data->distance > 50){
            generate_PWM(g_ui32SysClock, DC_OFF);
        }
        if(data->distance > 30 && data->distance < 50 ){
            generate_PWM(g_ui32SysClock, DC_50_PERCENT);
        }
        if(data->distance > 15 && data->distance < 30){
            generate_PWM(g_ui32SysClock/2, DC_50_PERCENT);
        }
        if(data->distance > 5 && data->distance < 15){
            generate_PWM(g_ui32SysClock/10, DC_50_PERCENT);
        }
        if(data->distance > 2 && data->distance < 5){
            generate_PWM(g_ui32SysClock/20, DC_50_PERCENT);
        }
        if(data->distance > 0 && data->distance < 2){
            generate_PWM(g_ui32SysClock, DC_100_PERCENT);
        }
        LOG_INFO(DIST_TASK, &data, " Distance");

    }
    else
    {
        data->connected = false;
        generate_PWM(g_ui32SysClock, DC_OFF);
        LOG_WARN(DIST_TASK,&data , "DISCONNECTED");
    }

}



bool BISTcheckSensorConnection(sensor_type_t sensor)
{

    bool connection = true;

    int16_t a,b,c;
    float temp_conn = 0.0;
    if(sensor == Ultrasonic)
    {
        if(get_Dist() <= 0) //val of distance is 0
        {

            SensorDownFlag |= (1<<0);
            connection = false;
        }
        else
        {
            SensorDownFlag &= ~(1<<0);
        }
    }
    if(sensor == Accelerometer)
    {
        if(!get_Accel(&a,&b,&c))
        {
            SensorDownFlag  |= (1<<1);
            connection = false;
        }
        else
        {
            SensorDownFlag &= ~(1<<1);
        }
    }
    if(sensor == Temperature)
    {
        temp_conn = TMP102_getTemperature();
        if(temp_conn == 0.0)
        {
            SensorDownFlag  |= (1<<2);
            connection = false;
        }
        else
        {
            SensorDownFlag &= ~(1<<2);
        }
    }
    if(SensorDownFlag == 0x00)
    {
        setThisNodeCurrentOperation(NORMAL);
    }
    if(SensorDownFlag == 0x07)
    {
        setThisNodeCurrentOperation(OUTOFSERVICE);
    }
    else
    {
        setThisNodeCurrentOperation(DEGRADED_OPERATION);
    }
    return connection;
}


int BBG_Disconnected_TempHandler(temp_data_t temperaturedata)
{
    temperaturedata.floatingpoint = 0.0;
    temperaturedata.floatingpoint = TMP102_getTemperature();
    if(temperaturedata.floatingpoint == 0.0)
    {
        temperaturedata.connected = false;
        LOG_WARN(TEMP_TASK,&temperaturedata , "DISCONNECTED");
        setThisNodeCurrentOperation(DEGRADED_OPERATION);
        return 1;
    }
    else
    {
        setThisNodeCurrentOperation(NORMAL);
    }
    if(temperaturedata.floatingpoint > 26.0f )
    {
        DCmotor(ON);
        LOG_INFO(TEMP_TASK,&temperaturedata,"***THRESHOLD EXCEEDED***");
    }
    else
    {
        DCmotor(OFF);
        LOG_INFO(TEMP_TASK,&temperaturedata," Celcius");
    }
    COMM_SEND(temperature,&temperaturedata);
    return 0;
}

int  BBG_Disconnected_AccelHandler(accel_data_t readings)
{
    if(!get_Accel(&readings.x, &readings.y, &readings.z))
    {
        readings.connected = false;
        setThisNodeCurrentOperation(DEGRADED_OPERATION);
        LOG_WARN(ACCEL_TASK,&readings , "DISCONNECTED");
        prev_x = 0;
        prev_y = 0;
        prev_z = 0;
        return 1;
    }
    else
    {
        readings.connected = true;
        setThisNodeCurrentOperation(NORMAL);
        if(prev_x != 0 && prev_y != 0 && prev_z != 0)
        {
            if((abs(prev_x - readings.x) > 150) || (abs(prev_y - readings.y) > 150) || (abs(prev_z - readings.z) > 150))
            {
                LOG_INFO(ACCEL_TASK,&readings,"***THRESHOLD EXCEEDED***");
                LCD_printString(0, 0, "**Accident**");
            }
            else
            {
                LCD_clear();
            }
        }

        //add new values into prev
        prev_x = readings.x;
        prev_y = readings.y;
        prev_z = readings.z;
        if(adxl345_checkINT(FREE_FALL))
        {
            readings.INT2 = true;
        }
        else readings.INT2 = false;

        LOG_INFO(ACCEL_TASK, &readings, " Accelerometer");
        return 0;
    }
}

int  BBG_Disconnected_DistHandler(dist_data_t data)
{
    data.distance = get_Dist();
    if (data.distance <= 0)
    {
        data.connected = false;
        generate_PWM(g_ui32SysClock, DC_OFF);
        setThisNodeCurrentOperation(DEGRADED_OPERATION);
        LOG_WARN(DIST_TASK,&data , "DISCONNECTED");
        return 1;
    }
    else{
        data.connected = true;
        setThisNodeCurrentOperation(NORMAL);
        if(data.distance > 50){
            generate_PWM(g_ui32SysClock, DC_OFF);
        }
        if(data.distance > 30 && data.distance < 50 ){
            generate_PWM(g_ui32SysClock, DC_50_PERCENT);
        }
        if(data.distance > 15 && data.distance < 30){
            generate_PWM(g_ui32SysClock/2, DC_50_PERCENT);
        }
        if(data.distance > 5 && data.distance < 15){
            generate_PWM(g_ui32SysClock/10, DC_50_PERCENT);
        }
        if(data.distance > 2 && data.distance < 5){
            generate_PWM(g_ui32SysClock/20, DC_50_PERCENT);
        }
        if(data.distance > 0 && data.distance < 2){
            generate_PWM(g_ui32SysClock, DC_100_PERCENT);
        }
        LOG_INFO(DIST_TASK, &data, " Distance");
        return 0;
    }
}

void LCD_printAccel(const accel_data_t *readings)
{
    char str[4];
    snprintf(str, 4, "%d", readings->x);
    LCD_printString(1,0,"X:");
    LCD_printString(1,1,str);
    snprintf(str, 4, "%d", readings->y );
    LCD_printString(1,8,"Y:");
    LCD_printString(1,9,str);
}

static void mySensorTasks(void *params)
{
    //Initializations
    InitI2C0();
    Dist_init();
    adxl345_setup();
    timer_Init();

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(RATE_MS);
    xLastWakeTime = xTaskGetTickCount();
    volatile uint32_t counter = 0;

    temp_data_t temperaturedata;
    temperaturedata.floatingpoint = 0.0;
    temperaturedata.unit = 'C';
    temperaturedata.connected = true;

    dist_data_t data = {0};

    accel_data_t readings = {0};
    readings.INT2 = false;
    readings.devId = adxl345_DeviceID();

    setThisNodeCurrentOperation(NORMAL);

    uint32_t SensorConnectionFlag = 0x00;
    vTaskDelay(2000);
    LOG_INFO(MAIN_TASK, NULL, " \t\t Start of Sensor Task");
    while(1)
    {
        if(!BBGConnectedFlag) //BBG is disconnected, TIVA independent working
        {   LOG_INFO(MAIN_TASK, NULL, "BBG disconnected, TIVA independent working");
            if(counter % 4 == 0) //temp task
            {
                if(BBG_Disconnected_TempHandler(temperaturedata)) //return 1 if sensor is not connected
                {
                    SensorConnectionFlag |= (1<<2);
                }
                else // 0 connected
                {
                    SensorConnectionFlag &= ~(1<<2);
                }
            }

            if(counter % 1 == 0) //accel task
            {
                if(BBG_Disconnected_AccelHandler(readings)) //return 1 if sensor is not connected
                {
                    SensorConnectionFlag |= (1<<1);
                }
                else // 0 connected
                {
                    SensorConnectionFlag &= ~(1<<1);
                }
            }

            if(counter % 1 == 0) // dist task
            {
                if(BBG_Disconnected_DistHandler(data)) //return 1 if sensor is not connected
                {
                    SensorConnectionFlag |= (1<<0);
                }
                else // 0 connected
                {
                    SensorConnectionFlag &= ~(1<<0);
                }
            }
            if((SensorConnectionFlag != 0x00) || (SensorConnectionFlag != 0x07) )
            {
                setThisNodeCurrentOperation(DEGRADED_OPERATION);
                //LOG_INFO(MAIN_TASK, NULL, " -----DEGRADED_OPERATION----");
            }
            if(SensorConnectionFlag == 0x07)
            {
                setThisNodeCurrentOperation(OUTOFSERVICE);
                LOG_INFO(MAIN_TASK, NULL, " -----OUTOFSERVICE----");
            }
            counter++;
        }
        else
        {
            //LOG_INFO(MAIN_TASK, NULL, "BBG Connected");
            if(counter % 6 == 0) //temp task
            {
                temperaturedata.floatingpoint = TMP102_getTemperature();
                COMM_SEND(temperature,&temperaturedata);
//                if(temperaturedata.floatingpoint == 0.0)
//                {
//                    temperaturedata.connected = false;
//                    //LOG_WARN(TEMP_TASK,&temperaturedata , "DISCONNECTED");
//                    setThisNodeCurrentOperation(DEGRADED_OPERATION);
//                    ETHERNET_SEND(temperature,&temperaturedata);
//                }
//                else
//                {
//                    temperaturedata.connected = true;
//                    setThisNodeCurrentOperation(NORMAL);
//                    ETHERNET_SEND(temperature,&temperaturedata);
//                }
            }
            if(counter % 1 == 0) //accel task
            {
                bool ret = get_Accel(&readings.x, &readings.y, &readings.z);
//                COMM_SEND(accelerometer,&readings);
                LCD_printAccel(&readings);
//                if(!get_Accel(&readings.x, &readings.y, &readings.z))
//                {
//                    readings.connected = false;
//                    setThisNodeCurrentOperation(DEGRADED_OPERATION);
//                    //LOG_WARN(ACCEL_TASK,&readings , "DISCONNECTED");
//                    ETHERNET_SEND(accelerometer,&readings);
//                }
//                else
//                {
//                    readings.connected = true;
//                    setThisNodeCurrentOperation(NORMAL);
//                    ETHERNET_SEND(accelerometer,&readings);
//                }
            }
            if(inReverseGear && counter % 1 == 0)
            {
                data.distance = get_Dist();
//                COMM_SEND(distance,&data);
//                if (data.distance <= 0)
//                {
//                    data.connected = false;
//                    setThisNodeCurrentOperation(DEGRADED_OPERATION);
//                    //LOG_WARN(DIST_TASK,&data , "DISCONNECTED");
//                    ETHERNET_SEND(Ultrasonic,&data);
//                }
//                else
//                {
//                    data.connected = true;
//                    setThisNodeCurrentOperation(NORMAL);
//                    ETHERNET_SEND(Ultrasonic,&data);
//                }
            }

        }
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

uint32_t SensorTaskInit(void)
{
    if(xTaskCreate(mySensorTasks, (const portCHAR *)"SENSORS", STACKSIZE_SENSORTASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_SENSOR_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    return (0);
}

