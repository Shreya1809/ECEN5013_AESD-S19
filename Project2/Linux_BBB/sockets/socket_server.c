#include "includes.h"


int main()
{
    int server_fd, new_socket, valread; 
    struct sockaddr_in client_address; 
    int opt = 1, ret = 0, process_id,rbytes,sbytes; 
    int addrlen = sizeof(client_address); 
    char buffer[1024] = {0}; 
    char mesg[100] = {"Hello from Server"};//,"IPC socket Programming","Server Process","Server says bye"}; 

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
   
    printf("[SERVER LOG] Socket Created %d\n",server_fd); 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }
     
    client_address.sin_family = AF_INET; 
    client_address.sin_addr.s_addr = INADDR_ANY; 
    client_address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8888 
    if (bind(server_fd, (struct sockaddr *)&client_address,  
                                 sizeof(client_address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    printf("[SERVER LOG] Socket Binded\n");
   

    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    printf("[SERVER LOG] Socket is Listening \n");
   

    while(1)
    {
        //printf("[SERVER LOG] Socket is Waiting.. \n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address,  
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 

        printf("[SERVER LOG] Socket Connection Established\n");
        

        //char clntName[50];
        //if (inet_ntop(AF_INET, &client_address.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
        //printf("[SERVER LOG] Client Address = %s/%d\n",clntName,PORT);
        //} else {
        //printf("Unable to get address\n");
        //}
        
        rbytes = read(new_socket, buffer, sizeof(buffer));
        printf("[SERVER LOG] Client payload received %s\n",buffer);
        send(new_socket , mesg , strlen(mesg) , 0 ); 
        printf("[SERVER LOG] Server payload sent %s\n",buffer);
            
            //sbytes = send(new_socket,mesg,sizeof(mesg), 0);
    }
    return 0; 

}