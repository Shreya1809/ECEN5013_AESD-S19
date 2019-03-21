/**
 * @file socket.c
 * @author your name (you@domain.com)
 * @brief 
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

/**
 * @brief 
 * 
 * @param threadp 
 * @return void* 
 */
void *socket_task(void *threadp)
{
    LOG_INFO(SOCKET_TASK,"Socket task thread spawned");
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0};
    char mesg[1024] ={0}; 
    //char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
    LOG_INFO(SOCKET_TASK,"Socket has been created"); 
     //printf("----socket created\n");  
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
    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        printf("socket accepted\n");
        LOG_INFO(SOCKET_TASK,"socket connection accepted from Remote Client"); 
        valread = read( new_socket , buffer, 1024); 
        LOG_INFO(SOCKET_TASK,"Remote Client request number is %s",buffer ); 
        
        if(strcmp(buffer,"1") == 0)
        {
            sprintf(mesg,"Temperature Requested is %f\n",getTemp(0));
        }
        if(strcmp(buffer,"2") == 0)
        {
           sprintf(mesg,"Light value Requested is %f\n",getLight());
        }
        if(strcmp(buffer,"3") == 0)
        {
            sprintf(mesg,"Temperature value in kelvin is %f",getTemp(2));
        }
        if(strcmp(buffer,"4") == 0)
        {
            sprintf(mesg,"Temperature value in celcius is %f",getTemp(1));
        }
        if(strcmp(buffer,"5") == 0)
        {
            sprintf(mesg,"Temperature value in Farenheit is %f",getTemp(3));
        }
        send(new_socket , mesg , strlen(mesg) , 0 ); 
        LOG_INFO(SOCKET_TASK,"Client Request is processed"); 
    }
    return NULL;
}

