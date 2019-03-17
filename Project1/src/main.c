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


void* (*ThreadEntryFunction[MAX]) (void*) = 
{
    bist_task,
    light_task,
    temp_task,
    log_task,
    socket_task,
};

int main(int argc , char **argv){

    FILE *fp;
    int rc;
    pthread_t threads[MAX];
    printf("-----Project1 started main thread------\n");
    if (argc != 2)
    {
        printf("USAGE <LOG FILE NAME>\n");
        exit(EXIT_FAILURE);
    }
    //log file
    fp = fopen(argv[1],"w+");
    if (fp == NULL)
    {
        printf("Could not open file %s\n",argv[1]);
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "log file created\n");

    //creating threads for tasks
    for(int i = 0; i < MAX; i++)
    {
        rc = pthread_create(&threads[i],NULL,ThreadEntryFunction[i], NULL);
        if(rc)
        {
            printf("pthread_create for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < MAX; i++)
    {
        rc = pthread_join(threads[i],NULL);
        if(rc)
        {
            printf("pthread_join for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            exit(EXIT_FAILURE);   
        }
    }    
    fclose(fp);
    return 0;
}