#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8888 
   
int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    char c,x;
    struct sockaddr_in serv_addr; 
    TRY: printf("Remote Client Request Option:\n");
    printf("1. Request Current Temperature value\n");
    printf("2. Request Current Light value\n");
    printf("3. Change Temperature unit to Kelvin\n");
    printf("4. Change Temperature unit to Celsius\n");
    printf("5. Change Temperature unit to Farenheit\n");
    printf("Enter any of the above option nos.\n");
    c = getchar();
    x = getchar(); //dummy enter

    if((c < '1') && (c > '5'))
    {
        printf("Invalid Option. Try again..\n");
        goto TRY;
    }
    //char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    send(sock , (char*)&c , sizeof(c) , 0 ); 
    printf("Client Request Sent\n"); 
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer ); 
    return 0; 
} 
