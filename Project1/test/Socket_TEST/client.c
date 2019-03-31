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
    int sock = 0, valread,m = 0; 
    char c,b,x;
    float t_hi,t_low;
    struct sockaddr_in serv_addr; 
    //char *hello = "Hello from client"; 
    char buffer[1024] = {0};
    char buffer1[1024] = {0}; 
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
    while(1)
    {
        TRY: printf("Remote Client Request Option:\n");
        printf("1. Request Current Temperature value\n");
        printf("2. Request Current Light value\n");
        printf("3. Request Temperature in Kelvin\n");
        printf("4. Request Temperature in Celsius\n");
        printf("5. Request Temperature in Farenheit\n");
        printf("6. Exit thread of choice\n");
        printf("7. Set Temperature Threshold\n");
        printf("8. Exit Client\n");
        printf("Enter any of the above option nos.\n");
        c = getchar();
        x = getchar(); //dummy enter
        printf("entered option is %c\n",c);
        if((c < '1') && (c > '8'))
        {
            printf("Invalid Option. Try again..\n");
            goto TRY;
        } 
        if(c == '6') 
        {
            printf("a. Temp Task Thread\n");
            printf("b. Light Task Thread\n");
            printf("c. Logger Task Thread\n"); 
            c = getchar();
            x = getchar(); //dummy enter
            if((c < 'a') && (c > 'c'))
            {
                printf("Invalid Option. Try again..\n");
                goto TRY;
            } 

        }
        if(c == '7') 
        {
            b = '7';
            char flow[20];
            char fhigh[20];
            printf("Enter the upper threshold\n");
            scanf("%f",&t_hi);
            x = getchar(); //dummy enter
            printf("Enter the lower threshold\n");
            scanf("%f",&t_low);
            x = getchar(); //dummy enter
            if((t_low < -56) && (t_hi > 128))
            {
                printf("Invalid Option. Try again..\n");
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
            //valread = read( sock , buffer, 1024); 
        }
        if(c == '8')
        {
            send(sock , (char*)&c , sizeof(c) , 0 );
            break; 
        }  
        if(c != '7')
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
