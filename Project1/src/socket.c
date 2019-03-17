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

/**
 * @brief 
 * 
 * @param threadp 
 * @return void* 
 */
void *socket_task(void *threadp)
{
    printf("The socket task!!!\n");
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0};
    char *mesg = "mesg from client"; 
    //char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
     printf("----socket created\n");  
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
    printf("----socket binded\n"); 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
    printf("----socket listening\n"); 
    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        printf("----socket accepted\n"); 
        valread = read( new_socket , buffer, 1024); 
        printf("Remote Client request number is %s\n",buffer ); 
        if(strcmp(buffer,"1") == 0)
        {
            mesg = "Temperature Requested";
        }
        if(strcmp(buffer,"1") == 0)
        {
            mesg = "Temperature Requested";
        }
        if(strcmp(buffer,"2") == 0)
        {
            mesg = "Light Requested";
        }
        if(strcmp(buffer,"3") == 0)
        {
            mesg = "Temperature unit Kelvin";
        }
        if(strcmp(buffer,"4") == 0)
        {
            mesg = "Temperature unit Celsius";
        }
        if(strcmp(buffer,"5") == 0)
        {
            mesg = "Temperature unit Farenheit";
        }
        send(new_socket , mesg , strlen(mesg) , 0 ); 
        printf("Client Request is processed\n"); 
    }
    return NULL;
}

