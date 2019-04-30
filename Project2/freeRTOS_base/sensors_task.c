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

volatile accel_data_t prevAccelReadings = {
                                           .x = 0,
                                           .y = 0,
                                           .z = 0
};

bool volatile inReverseGear = false;
bool volatile BBGConnectedFlag = false;
SemaphoreHandle_t sensorDataLock;
int32_t SensorDownFlag = 0;
volatile accel_data_t accelDeltaThreshold = {
                                             .x = 200,
                                             .y = 200,
                                             .z = 200,
};

volatile temp_data_t temperatureThreshold = {
                                            .floatingpoint = 26.0,
                                            .unit = 'C'
};

volatile uint32_t SensorConnectionFlag = 0x00;


void DCmotor(state_t option)
{
    if(option)
    {
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_PIN_7);
    }
}

void setReversGear(bool state)
{
    xSemaphoreTake(sensorDataLock, portMAX_DELAY);
    inReverseGear = state;
    xSemaphoreGive(sensorDataLock);
    state ? LCD_printString(0,13,"1") : LCD_printString(0,13,"0");
}

bool getReversGear()
{
    bool state;
    xSemaphoreTake(sensorDataLock, portMAX_DELAY);
    state = inReverseGear;
    xSemaphoreGive(sensorDataLock);
    return state;
}


void setTemperatureThreshold(float temp)
{
    xSemaphoreTake(sensorDataLock, portMAX_DELAY);
    temperatureThreshold.floatingpoint = temp;
    xSemaphoreGive(sensorDataLock);
}

float getTemperatureThreshold()
{
    float t;
    xSemaphoreTake(sensorDataLock, portMAX_DELAY);
    t = temperatureThreshold.floatingpoint;
    xSemaphoreGive(sensorDataLock);
    return t;
}

void setAccelDeltaThreshold(accel_data_t *accel)
{
    xSemaphoreTake(sensorDataLock, portMAX_DELAY);
    accelDeltaThreshold = *accel;
    xSemaphoreGive(sensorDataLock);
}

void getAccelDeltaThreshold(accel_data_t *accel)
{
    xSemaphoreTake(sensorDataLock, portMAX_DELAY);
    *accel = accelDeltaThreshold;
    xSemaphoreGive(sensorDataLock);
}
#if 0
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
#endif

static inline void LCD_printAccel(const accel_data_t *readings)
{
    char str[4] = {0};
    LCD_printString(1,0,"                ");
    snprintf(str, 4, "%d", readings->x);
    LCD_printString(1,0,"X");
    LCD_printString(1,1,str);
    snprintf(str, 4, "%d", readings->y );
    LCD_printString(1,5," Y");
    LCD_printString(1,7,str);
    snprintf(str, 4, "%d", readings->z );
    LCD_printString(1,10," Z");
    LCD_printString(1,13,str);
}

static inline int AccelHandler(accel_data_t *readings)
{
    if(!get_Accel(&readings->x, &readings->y, &readings->z))
    {
        readings->connected = false;
        setThisNodeCurrentOperation(DEGRADED_OPERATION);
        LOG_WARN(ACCEL_TASK,&readings , "DISCONNECTED");
        prevAccelReadings.x = 0;
        prevAccelReadings.y = 0;
        prevAccelReadings.z = 0;
        SensorConnectionFlag |= (1<<1);
        return 1;
    }
    else
    {
        readings->connected = true;
        setThisNodeCurrentOperation(NORMAL);
        if(prevAccelReadings.x != 0 && prevAccelReadings.y != 0 && prevAccelReadings.z != 0)
        {
            accel_data_t th = {0};
            getAccelDeltaThreshold(&th);
            if((abs(prevAccelReadings.x - readings->x) > th.x))
            {
                LOG_INFO(ACCEL_TASK,&readings,"***ACCEL THRESHOLD EXCEEDED***");
                LCD_printString(1, 0, "****ACCIDENT****");
            }
            else
            {
                LCD_printAccel(readings);
            }
        }

        //add new values into prev
        prevAccelReadings.x = readings->x;
        prevAccelReadings.y = readings->y;
        prevAccelReadings.z = readings->z;
        if(adxl345_checkINT(FREE_FALL))
        {
            readings->INT2 = true;
        }
        else readings->INT2 = false;
        SensorConnectionFlag &= ~(1<<1);
        return 0;
    }
}

static inline int DistHandler(dist_data_t *data)
{
    data->distance = get_Dist();
    if (data->distance <= 0)
    {
        data->connected = false;
        generate_PWM(g_ui32SysClock, DC_OFF);
        LOG_WARN(DIST_TASK, &data , "DISCONNECTED");
        SensorConnectionFlag |= (1<<0);
        return 1;
    }
    else{
        data->connected = true;
        setThisNodeCurrentOperation(NORMAL);
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
        SensorConnectionFlag &= ~(1<<0);
        return 0;
    }
}

static inline int TempHandler(temp_data_t *temperaturedata)
{
    temperaturedata->floatingpoint = TMP102_getTemperature();
    if(BBGConnectedFlag == false)
    {
        //we will decide on the fan as BBG is not connected
        if(temperaturedata->floatingpoint > getTemperatureThreshold())
        {
            DCmotor(ON);
            LOG_INFO(TEMP_TASK,temperaturedata,"***THRESHOLD EXCEEDED***");
        }
        else
        {
            DCmotor(OFF);
            LOG_INFO(TEMP_TASK,temperaturedata," Celcius");
        }
    }
    else
    {
        //send to control node
        COMM_SEND(temperature,temperaturedata);
    }
    return 0;

}
static void mySensorTasks(void *params)
{
    //Initializations
    InitI2C0();
    Dist_init();
    adxl345_setup();
    timer_Init();
    sensorDataLock = xSemaphoreCreateMutex();

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(RATE_MS);
    xLastWakeTime = xTaskGetTickCount();
    volatile uint32_t counter = 0;

    temp_data_t temperaturedata;
    temperaturedata.floatingpoint = 0.0;
    temperaturedata.unit = 'C';
    temperaturedata.connected = true;

    dist_data_t distanceData = {0};

    accel_data_t accelReadings = {0};
    accelReadings.INT2 = false;
    accelReadings.devId = adxl345_DeviceID();

    setThisNodeCurrentOperation(NORMAL);

    vTaskDelay(2000);
    LOG_INFO(MAIN_TASK, NULL, " \t\t Start of Sensor Task");
    while(1)
    {
        if(BBGConnectedFlag == false)
        {
            LOG_DEBUG(TEMP_TASK, NULL, "BBG Disconnected");
        }
        if(counter % 6 == 0)
        {
            //temperature
            TempHandler(&temperaturedata);
        }

        //accel
        AccelHandler(&accelReadings);
        LCD_printAccel(&accelReadings);
        if(counter % 2 && BBGConnectedFlag)
        {
            LOG_INFO(ACCEL_TASK, &accelReadings, "Accelerometer");
            COMM_SEND(accelerometer, &accelReadings);
        }


        if(getReversGear())
        {
            DistHandler(&distanceData);
            if(counter % 2 && BBGConnectedFlag)
            {
                LOG_INFO(DIST_TASK, &data, " Distance");
                COMM_SEND(distance, &distanceData);
            }
        }

        //updating the opertion state
        if(SensorConnectionFlag == 0x00)
        {
            setThisNodeCurrentOperation(NORMAL);
        }
        else if(SensorConnectionFlag != 0x07)
        {
            setThisNodeCurrentOperation(DEGRADED_OPERATION);
            //LOG_INFO(MAIN_TASK, NULL, " -----DEGRADED_OPERATION----");
        }
        else if(SensorConnectionFlag == 0x07)
        {
            setThisNodeCurrentOperation(OUTOFSERVICE);
            LOG_INFO(MAIN_TASK, NULL, " -----OUTOFSERVICE----");
        }
        counter++;
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

uint32_t SensorTaskInit(void)
{
//    sensorDataLock = xSemaphoreCreateMutex();
//    return 0;
    if(xTaskCreate(mySensorTasks, (const portCHAR *)"SENSORS", STACKSIZE_SENSORTASK, NULL,
                   tskIDLE_PRIORITY + PRIORITY_SENSOR_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    return (0);
}

