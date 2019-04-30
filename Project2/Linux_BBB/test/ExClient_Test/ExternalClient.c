// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
   
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#define PORT 9999
   
int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread,m = 0; 
    char c,b,x;
    float t_hi,t_low;
    uint16_t a_x,a_y,a_z;
    uint32_t secs,nanosec;
    struct sockaddr_in serv_addr; 
    char startFrame[4] = "<::>";
    char buffer[1024] = {0};
    char buffer1[1024] = {0}; 
    char buffer2[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "10.0.0.19", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    while(1)
    {
        TRY: //send(sock , startFrame , sizeof(startFrame) , 0 ); 
        printf("EXternal Client Request Option:\n");
        printf("1. Request Current Car Temperature value\n");
        printf("2. Request Current  Car Acceleration value\n");
        printf("3. Set Temperature Thresholds\n");
        printf("4. Set Acceleration Thresholds\n");
        printf("5. Set Car in reverse Gear for parking\n");
        printf("6. Set Car in Front Gear for driving\n");
        printf("7. Close the Control Node and exit the client\n");
        printf("Enter any of the above option.\n");
        c = getchar();
        x = getchar(); //dummy enter
        printf("entered option is %c\n",c);
        if((c < '0') && (c > '7'))
        {
            printf("Invalid Option. Try again..\n");
            goto TRY;
        } 
        if(c == '3') 
        {
            b = '3';
            char flow[20];
            char fhigh[20];
            printf("Enter the upper Temp threshold\n");
            scanf("%f",&t_hi);
            x = getchar(); //dummy enter
            printf("Enter the lower Temp threshold\n");
            scanf("%f",&t_low);
            x = getchar(); //dummy enter
            if((t_low < -56) && (t_hi > 128))
            {
                printf("Invalid Thresholds. Range -56 < x < 128 Try again..\n");
                goto TRY;
            }
            send(sock , (char*)&b , sizeof(b) , 0 ); //sending request number
            valread = read( sock , buffer1, 1024);
            gcvt(t_hi,6,fhigh);
            gcvt(t_low,6,flow);
            strcat(flow,",");
            strcat(flow,fhigh);
            printf("low and high values entered are %s\n",flow);
            send(sock , (char*)&flow , sizeof(flow) , 0 );
        }
        if(c == '4') 
        {
            b = '4';
            char del_x[20];
            char del_y[20];
            char del_z[20];
            printf("Enter the max acceptable delta for x axis\n");
            scanf("%hd",&a_x);
            x = getchar(); //dummy enter
            printf("Enter the max acceptable delta for y axis\n");
            scanf("%hd",&a_y);
            x = getchar(); //dummy enter
            printf("Enter the max acceptable delta for z axis\n");
            scanf("%hd",&a_z);
            x = getchar(); //dummy enter
            if((a_x < 100) || (a_x > 300))
            {
                printf("Invalid value.Range 100<val<300,Try again..\n");
                goto TRY;
            }
            if((a_y < 100) || (a_y > 300))
            {
                printf("Invalid value.Range 100<val<300,Try again..\n");
                goto TRY;
            }
            if((a_z < 100) || (a_z > 300))
            {
                printf("Invalid value.Range 100 < val < 300,Try again..\n");
                goto TRY;
            }
            send(sock , (char*)&b , sizeof(b) , 0 ); //sending request number
            valread = read( sock , buffer1, 1024);
            sprintf(del_x,"%d",a_x);
            sprintf(del_y,"%d",a_y);
            sprintf(del_z,"%d",a_z);
            strcat(del_x,",");
            strcat(del_x,del_y);
            strcat(del_x,",");
            strcat(del_x,del_z);
            printf("The delta valus are %s for x,y and z axis\n",del_x);
            send(sock , (char*)&del_x , sizeof(del_x) , 0 );
        }
        if(c == '7')
        {
            send(sock , (char*)&c , sizeof(c) , 0 ); //exit client and send close control node indeication
            break; 
        }  
        if(c != '4' && c != '3')
        {
            send(sock , (char*)&c , sizeof(c) , 0 ); 
            printf("Client Request Sent\n");
            buffer[1024]= '\0';
            valread = read( sock , buffer, 1024); 
            printf("%s\n",buffer ); 
            
        }
    }
    printf("Client exiting....\n");
    close(sock);
    return 0; 
} 