/**
 * @file socket.c
 * @author Shreya Chakraborty
 * @brief Socket thread task functionality
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "includes.h"
#include "socket.h"
#include "temp.h"
#include "logger.h"
#include "light.h"
#include "bbgled.h"
#include "mysignal.h"
#include "tempSensor.h"
#include "heartbeat.h"
#include "bist.h"

static sig_atomic_t stop_thread_socket = 0;
extern pthread_t threads[MAX_TASKS];

void kill_socket_thread(void)
{
    LOG_DEBUG(SOCKET_TASK,"socket thread exit signal received");
    stop_thread_socket = 1;
    pthread_cancel(threads[SOCKET_TASK]);    
}

void *socket_task(void *threadp)
{
    sem_init(&temp_thread_sem,0,0);
    sem_wait(&socket_thread_sem);
    if(!CheckBistResult())
    {
        goto exit;
    }
    LOG_INFO(SOCKET_TASK,"Socket task thread spawned");
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1, m = 0; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0};
    char buffer1[1024] = {0};
    char buffer2[1024] = {0};
    char mesg[1024] ={0};
    char mesg1[1024] ={0}; 
    char mesg2[1024] ={0};
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
    LOG_INFO(SOCKET_TASK,"Socket has been created"); 
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    LOG_INFO(SOCKET_TASK,"socket binded"); 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
    LOG_INFO(SOCKET_TASK,"socket listening"); 
    while(!stop_thread_socket)
    {
        set_heartbeatFlag(SOCKET_TASK);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        LOG_INFO(SOCKET_TASK,"socket connection accepted from Remote Client"); 
        while(!stop_thread_socket)
        {
            mesg[1024] = '\0';
            mesg1[1024] = '\0';
            mesg2[1024] = '\0';
            valread = read( new_socket , buffer, 1024); 
            if(strcmp(buffer,"\0") == 0)
            {
                LOG_INFO(SOCKET_TASK,"Socket disconnected from remote client");
                break;
            }
            LOG_INFO(SOCKET_TASK,"Remote Client request number is %s",buffer ); 
            
            if(strcmp(buffer,"1") == 0)
            {
                sprintf(mesg,"Temperature Requested is %f",GET_TEMP_CELCIUS());
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"2") == 0)
            {
                sprintf(mesg,"Light value Requested is %f",GETLUX());
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"3") == 0)
            {
                sprintf(mesg,"Temperature value in kelvin is %f",GET_TEMP_KELVIN());
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"4") == 0)
            {
                sprintf(mesg,"Temperature value in celcius is %f",GET_TEMP_CELCIUS());
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"5") == 0)
            {
                sprintf(mesg,"Temperature value in Farenheit is %f",GET_TEMP_FARENHEIT());
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"a") == 0)
            {
                sprintf(mesg,"Request to Kill Temp thread");
                kill_temp_thread();
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"b") == 0)
            {
                sprintf(mesg,"Request to Kill Light thread");
                kill_light_thread();
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"c") == 0)
            {
                sprintf(mesg,"Request to Kill Logger thread");
                kill_logger_thread();
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"7") == 0)
            {
                char *p;
                int i = 0;
                float f_val[2] = {0.0};
                sprintf(mesg1,"threshold values for temp");
                send(new_socket , mesg1 , strlen(mesg1) , 0 ); 
                valread = read( new_socket , buffer1, 1024); 
                p = strtok(buffer1,",");
                while(p != NULL)
                {
                    f_val[i] = atof(p);
                    p = strtok(NULL,",");
                    i++;
                }
                LOG_DEBUG(SOCKET_TASK,"flow and fhigh are %f and %f",f_val[0],f_val[1]);
                RemoteThresholdValues(f_val[0],f_val[1]);
            }
            if(strcmp(buffer,"8") == 0)
            {
                char *p;
                int i = 0;
                uint16_t val[2] = {0.0};
                sprintf(mesg2,"threshold values for light");
                send(new_socket , mesg2 , strlen(mesg2) , 0 ); 
                valread = read( new_socket , buffer2, 1024); 
                p = strtok(buffer2,",");
                while(p != NULL)
                {
                    val[i] = atof(p);
                    p = strtok(NULL,",");
                    i++;
                }
                LOG_DEBUG(SOCKET_TASK,"low and high light threshold are %d and %d",val[0],val[1]);
                RemoteThresholdValueslight(val[0],val[1]);
            }
            
            if(strcmp(buffer,"9") == 0)
            {
                LOG_INFO(SOCKET_TASK,"Client has requested program to exit");
                SystemExit();
            } 
            if(strcmp(buffer,"0") == 0)
            {
                LOG_INFO(SOCKET_TASK,"Client has exited");
                break;
            } 
            LOG_INFO(SOCKET_TASK,"Client Request processed");
        }

    }
    close(new_socket);
exit:
    PRINTLOGCONSOLE("Socket task closed");
    LOG_INFO(SOCKET_TASK,"Socket task closed");
    return NULL;
}

