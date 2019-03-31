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
#include "bist.h"
#include "heartbeat.h"

pthread_t threads[MAX_TASKS] = {0};
int thread_flag[MAX_TASKS] = {0};
void* (*ThreadEntryFunction[MAX_TASKS]) (void*) = 
{
    bist_task,
    logger_task,
    light_task,
    temp_task,
    socket_task
};

const char * moduleIdName[MAX_TASKS+1] = {
    "BIST_TASK",
    "LOGGER_TASK",
    "TEMP_TASK",
    "LIGHT_TASK",
    "SOCKET_TASK",
    "MAIN_TASK",
};
void *threadParamArgs[MAX_TASKS] = {0};


#ifndef TEST_MODE
int main(int argc , char **argv){

    int rc;
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

    threadParamArgs[1] = (void*)&loggerParam;
    
    //creating threads for tasks
    for(int i = 1; i < MAX_TASKS; i++)
    {
        rc = pthread_create(&threads[i],NULL,ThreadEntryFunction[i], threadParamArgs[i]);
        if(rc)
        {
            PRINT("pthread_create for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            exit(EXIT_FAILURE);
        }
        else
        {
            thread_flag[i]=1;
            PRINT("%s thread created\n",moduleIdName[i]);

        }
        
    }
    LOG_INFO(MAIN_TASK, "Threads spawned from the main");
    //bist thread generation
    //check for bist flag
    rc = pthread_create(&threads[0],NULL,ThreadEntryFunction[0], threadParamArgs[0]);
    if(rc)
    {
        PRINT("pthread_create for thread %s failed\n", (char*)ThreadEntryFunction[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        thread_flag[0]=1;
        PRINT("%s thread created\n",moduleIdName[0]);

    }

    rc = pthread_join(threads[0],NULL);
    if(rc)
    {
        PRINTLOGCONSOLE("pthread_join for thread %s failed\n", (char*)ThreadEntryFunction[0]);
        exit(EXIT_FAILURE);   
    }
    PostBistOkResult();
    if(CheckBistResult())
    {
        startHearbeatCheck();
    }
    //printf("here\n");
    for(int i = 1; i < MAX_TASKS; i++)
    {
        rc = pthread_join(threads[i],NULL);
        if(rc)
        {
            PRINTLOGCONSOLE("pthread_join for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            exit(EXIT_FAILURE);   
        }
    }   
    
    PRINT("******Program Clean Exit******\n");

    return 0;
}

#endif