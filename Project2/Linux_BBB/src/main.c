/**
 * @file main.c
 * @author Shreya Chakraborty
 * @brief Main task which spawns the other threads 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "main.h"
#include "logger.h"
#include "socket.h"
#include "bbgled.h"
#include "mysignal.h"
#include "heartbeat.h"
#include "comm_recv.h"
#include "comm_send.h"
#include "ext_clientHandler.h"
#include "uart.h"

pthread_t threads[MAX_TASKS]={0};
int thread_flag[MAX_TASKS] = {0};
//thread entry functions
void* (*ThreadEntryFunction[MAX_TASKS]) (void*) = 
{
    logger_task,
    ext_clientHandle_task,
    commRecv_task,
    commSend_task
};

const char * moduleIdName[MAX_TASKS+1] = {
    
    "LOGGER_TASK",
    "EXT_TASK",
    "RECV_TASK",
    "SEND_TASK",
    "MAIN_TASK",
};
void *threadParamArgs[MAX_TASKS] = {0};

uint32_t startofprogramTime = 0;
#ifndef TEST_MODE
int main(int argc , char **argv)
{
    startofprogramTime = getStartofProgramTime();
    #ifdef TCP
    int server_fd, com_socket;
    struct sockaddr_in server_addr; 
    int opt = 1; 
    int addrlen = sizeof(server_addr);
    #endif  
    pthread_t thread;
    int rc;
    signal(SIGINT,signal_handler);
    logger_queue_init();
    PRINT("-----Project1 started main thread------\n");
    if (argc < 2)
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

    if(argc == 3){
        loggerParam.loglevel = atoi(argv[2]);
    }

    threadParamArgs[LOGGER_TASK] = (void*)&loggerParam;
    
        //creating logger thread and external client thread
    for(int i = 0; i < 2; i++)
    {
        rc = pthread_create(&threads[i],NULL,ThreadEntryFunction[i], threadParamArgs[i]);
        if(rc)
        {
            PRINT("pthread_create for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            //GREENLEDOFF();
            REDLEDON();
            exit(EXIT_FAILURE);
        }
        else
        {
            thread_flag[i]=1;
            PRINT("%s thread created\n",moduleIdName[i]);
            REDLEDOFF();

        }
    }
    #ifdef TCP    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    LOG_INFO(MAIN_TASK, "Communication socket file descriptor created %d",server_fd);

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons( 10000 ); 

    // Forcefully attaching socket to the port 8888 
    if (bind(server_fd, (struct sockaddr *)&server_addr,sizeof(server_addr))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    LOG_INFO(MAIN_TASK, "Communication Socket Binding...");

    if (listen(server_fd, 10) == -1) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    LOG_INFO(MAIN_TASK, "Communication Socket Listening...");

    while(1)
    {
        if ((com_socket = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        LOG_INFO(MAIN_TASK,"Communication Socket Accepting...") ;
   
        LOG_INFO(MAIN_TASK,"Got a connection from %s on port %d", inet_ntoa(server_addr.sin_addr), htons(server_addr.sin_port));
        /* Make a safe copy of newsock */
        int *safesock = (int*)malloc(sizeof(int));
        if (safesock) {
            *safesock = com_socket;
            if (pthread_create(&thread, NULL, commRecv_task, safesock) != 0) {
                fprintf(stderr, "Failed to create thread\n");
            }
            else{
                thread_flag[RECV_TASK]=1;
            LOG_INFO(MAIN_TASK,"COMM RECV TASK thread created");
            }

            // if (pthread_create(&thread, NULL, commSend_task, safesock) != 0) {
            //     fprintf(stderr, "Failed to create thread\n");
            // }
            // else{
            //     thread_flag[SEND_TASK]=1;
            // LOG_INFO(MAIN_TASK,"COMM SEND TASK thread created");
            // }

        }
        else {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }
    #else
    int fd = 0;
    fd = UART_config();
    if(fd < 0)
    {
        perror("uart config:");
    }
    if (pthread_create(&thread, NULL, commRecv_task, &fd) != 0) {
                fprintf(stderr, "Failed to create thread\n");
            }
            else{
                thread_flag[RECV_TASK]=1;
            LOG_INFO(MAIN_TASK,"COMM RECV TASK thread created");
            }

            if (pthread_create(&thread, NULL, commSend_task, &fd) != 0) {
                fprintf(stderr, "Failed to create thread\n");
            }
            else{
                thread_flag[SEND_TASK]=1;
            LOG_INFO(MAIN_TASK,"COMM SEND TASK thread created");
            }
    #endif
    //joining socket threads first followed by logger thread
    for(int i = 1; i< 4;i++)
    {
        rc = pthread_join(threads[i],NULL);
        if(rc)
        {
            PRINTLOGCONSOLE("pthread_join for thread %s failed\n", (char*)ThreadEntryFunction[i]);
            //GREENLEDOFF();
            REDLEDON();
            exit(EXIT_FAILURE);
        }  
    }
    #ifdef TCP 
    close(server_fd); 
    #endif    

    kill_logger_thread();
    //all tasks have exited. only then exit the logger task to make sure we have all logs
    rc = pthread_join(threads[LOGGER_TASK],NULL);
    if(rc)
    {
        PRINTLOGCONSOLE("pthread_join for thread %s failed\n", (char*)ThreadEntryFunction[LOGGER_TASK]);
        GREENLEDOFF();
        REDLEDON();
        exit(EXIT_FAILURE);   
    }
    
    PRINT("******Program Clean Exit******\n");
    GREENLEDOFF();
    return 0;
}

#endif