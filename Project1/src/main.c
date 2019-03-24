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
    //signal_init();
    logger_queue_init();
    GREENLEDON();
    REDLEDOFF();
    LOG_INFO(MAIN_TASK, "-----Project1 started main thread------");
    if (argc < 2)
    {
        PRINTLOGCONSOLE("Command line Arg Error: USAGE <LOG FILE NAME>");
        GREENLEDOFF();
        REDLEDON();
        sleep(3);
        REDLEDOFF();
        exit(EXIT_FAILURE);
    }

    struct loggerTask_param loggerParam = {
        .filename = argv[1],
        .loglevel = LOG_DEBUG
    };

    if(argc == 3){
        loggerParam.loglevel = atoi(argv[2]);
    }

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
    return 0;
}