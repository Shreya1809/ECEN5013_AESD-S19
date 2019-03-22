/**
 * @file main.c
 * @author shreya
 * @brief main thread 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "main.h"
#include "logger.h"
#include "temp.h"
#include "socket.h"
#include "light.h"
#include "bist.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"


void* (*ThreadEntryFunction[MAX_TASKS]) (void*) = 
{
    logger_task,
    bist_task,
    light_task,
    temp_task,
    socket_task
};

const char * moduleIdName[MAX_TASKS+1] = {
    "LOGGER_TASK",
    "TEMP_TASK",
    "LIGHT_TASK",
    "BIST_TASK",
    "SOCKET_TASK",
    "MAIN_TASK",
};
void *threadParamArgs[MAX_TASKS] = {0};


int main(int argc , char **argv){

    int rc;
    pthread_t threads[MAX_TASKS];
    sem_init(&temp_sem,0,0);
    sem_init(&light_sem,0,0);
    //signal_init();
    logger_queue_init();
    /*if(BBGinit()){
        LOG_ERROR(MAIN_TASK, "LED Init failed");
        return -1;
    }*/
    GREENLEDON();
    LOG_INFO(MAIN_TASK, "-----Project1 started main thread------");
    if (argc != 2)
    {
        PRINTLOGCONSOLE("Command line Arg Error: USAGE <LOG FILE NAME>");
        GREENLEDOFF();
        REDLEDON();
        exit(EXIT_FAILURE);
    }

    struct loggerTask_param loggerParam = {
        .filename = argv[1],
        .loglevel = LOG_DEBUG
    };

    threadParamArgs[0] = (void*)&loggerParam;

    //creating threads for tasks
    for(int i = 0; i < MAX_TASKS; i++)
    {
        rc = pthread_create(&threads[i],NULL,ThreadEntryFunction[i], threadParamArgs[i]);
        if(rc)
        {
            PRINT("pthread_create for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            exit(EXIT_FAILURE);
        }
    }
    LOG_INFO(MAIN_TASK, "Threads spawned from the main");
    for(int i = 0; i < MAX_TASKS; i++)
    {
        rc = pthread_join(threads[i],NULL);
        if(rc)
        {
            PRINTLOGCONSOLE("pthread_join for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            exit(EXIT_FAILURE);   
        }
    }  
    //BBGled_off();  
    return 0;
}