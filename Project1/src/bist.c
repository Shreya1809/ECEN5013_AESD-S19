/**
 * @file bist.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "bist.h"
#include "logger.h"
#include "main.h"
#include "bbgled.h"
#include "mysignal.h"
#include "lightSensor.h"
#include "tempSensor.h"
#include "myI2C.h"

extern int thread_flags[5];
static volatile uint32_t BIST_allOk = 0;
static pthread_mutex_t bistlock = PTHREAD_MUTEX_INITIALIZER;

int Test_LightSensor(void)
{
    int ret = 0;
    uint8_t data;
    ret = APDS9301_powerup();
    LOG_INFO(BIST_TASK, "Light Sensor(ID:0x%1x) powered up",data);
    ret += APDS9301_readIDreg(&data); 
    LOG_DEBUG(BIST_TASK,"Testing Light Sensor ID reg 0x%1x",data);
    if(ret)
    {
        return EXIT_FAILURE;
    }
    if((data & 0xF0) != 0x50)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int Test_AllThreads(void)
{
    int combined_flag = 1;
    for(int i = 0; i < MAX_TASKS; i++)
    {
        if(!thread_flag[i])
        {
            PRINTLOGCONSOLE("%s thread has not been spawned",moduleIdName[i]);
        }
        combined_flag &= thread_flag[i];
    }
    if (!combined_flag)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;    
}

int Test_I2C(void)
{
    int ret = I2C_init(&i2c_handler);
    return ret;
}

int Test_TempSensor(void)
{
    float temp_val;
    TMP102_getTemperature(&temp_val);
    if((temp_val > 128) || (temp_val < -40) || (temp_val == 0.0))
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int Test_loggerQ(void)
{
    int ret = LOG_ENQUEUE(LOG_DEBUG,BIST_TASK, "Testing Logger Queue");
    if(ret != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int BuiltInSelfTest(void)
{
    int ret;
    ret = Test_loggerQ();
    if(!ret)
    {
        PRINTLOGCONSOLE("Logger Queue is functional");   
    }
    else
    {
        PRINTLOGCONSOLE("Logger Queue is NOT functional"); 
    }
    int ret1 = Test_I2C();
    if(!ret1)
    {
        PRINTLOGCONSOLE("I2C bus initialisation Successfull");   
    }
    else
    {
        PRINTLOGCONSOLE("I2C bus initialisation NOT Successfull"); 
    }
    //int ret2 = 0;
    int ret2 = Test_AllThreads();
    if(!ret2)
    {
        PRINTLOGCONSOLE("All threads created and functional");   
    }
    else
    {
        PRINTLOGCONSOLE("Some thread/threads were mot created successfully");      
    }
    int ret3 = Test_LightSensor();
    if(!ret3)
    {
        PRINTLOGCONSOLE("Light Sensor connected and functional");   
    }
    else
    {
        PRINTLOGCONSOLE("Light Sensor disconnected and non functional");     
    }
    int ret4 = Test_TempSensor();
    if(!ret4)
    {
        PRINTLOGCONSOLE("Temp Sensor connected and functional");   
    }
    else
    {
        PRINTLOGCONSOLE("Temp Sensor disconnected and non functional");  
    }
    return (ret|ret1|ret2|ret3|ret4);

}

int CheckBistResult(void)
{
    int ret;
    pthread_mutex_lock(&bistlock);
    ret = BIST_allOk;/* code */
    pthread_mutex_unlock(&bistlock);
    sem_post(&logger_thread_sem);
    sem_post(&temp_thread_sem);
    sem_post(&light_thread_sem);
    sem_post(&socket_thread_sem); 
    return ret;   
}
void *bist_task(void *threadp)
{
    PRINT("-----Built in Self Test started-----\n");
    int ret = BuiltInSelfTest();
    if(ret)
    {
        REDLEDON();   
        GREENLEDOFF(); 
        PRINTLOGCONSOLE("Program Exiting.....");
        //return NULL; 
    }
    else
    {
        pthread_mutex_lock(&bistlock);
        BIST_allOk = 1;/* code */
        pthread_mutex_unlock(&bistlock);
    }
    PRINT("-----Built in Self Test Ended-----\n");
    //printf("bist flag value %d\n",BIST_allOk);
    return NULL;
}