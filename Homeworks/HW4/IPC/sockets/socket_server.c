#include "includes.h"

double getTimeMsec()
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_MONOTONIC, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}
volatile sig_atomic_t done = 0;

void terminate(int signum)
{
    printf("SIGINT!\n");
    FILE *fp2;
    fp2 = fopen("log.txt", "a");
    fprintf(fp2,"<%lf> SIGINT! Received! Terminating server....\n",getTimeMsec());
    fclose(fp2);
    done = 1;
}
int main()
{
    int server_fd, new_socket, valread; 
    struct sockaddr_in client_address; 
    int opt = 1, ret = 0, process_id,rbytes,sbytes; 
    int addrlen = sizeof(client_address); 
    char buffer[1024] = {0}; 
    char *mesgs[] = {"Hello from Server","IPC socket Programming","Server Process","Server says bye"}; 
    FILE *fp, *fp1;
    process_id = getpid();
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = terminate;
    sigaction(SIGINT, &action, NULL);
    
    //payload_struct_t rec_packet;
    fp1 = fopen("log.txt", "w+");
    fp = fopen("log.txt", "a");
    if(fp == NULL)
    {
        perror("file not opened");
    }
    fprintf(fp,"------------IPC MECHANISMS -> SOCKETS--------------\n");
    fprintf(fp,"<%lf> [SERVER LOG] Server Process ID: %d\n",getTimeMsec(),process_id);
    printf("[SERVER LOG] Server Process ID: %d\n",process_id);
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
    fprintf(fp,"<%lf> [SERVER LOG] Socket Created  %d\n",getTimeMsec(), server_fd); 
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
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&client_address,  
                                 sizeof(client_address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    printf("[SERVER LOG] Socket Binded\n");
    fprintf(fp,"<%lf> [SERVER LOG] Socket Binded\n",getTimeMsec());

    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    printf("[SERVER LOG] Socket is Listening \n");
    fprintf(fp,"<%lf> [SERVER LOG] Socket is Listening \n",getTimeMsec());
    fclose(fp);

    while(!done)
    {
        fopen("log.txt", "a");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address,  
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 

        printf("[SERVER LOG] Socket Connection Established\n");
        fprintf(fp,"<%lf> [SERVER LOG] Socket Connection Established\n",getTimeMsec());

        char clntName[50];
        if (inet_ntop(AF_INET, &client_address.sin_addr.s_addr, clntName,sizeof(clntName)) != NULL){
        printf("[SERVER LOG] Client Address = %s/%d\n",clntName,PORT);
        fprintf(fp,"<%lf> [SERVER LOG] Client Address = %s/%d\n",getTimeMsec(),clntName,PORT);
        } else {
        printf("Unable to get address\n");
        }
        fclose(fp); 
        
        for(int i = 0; i < 4; i++)
        {
            memcpy(s_packet.payload[i],mesgs[i],strlen(mesgs[i])+1);
            s_packet.payloadSize[i] = strlen(mesgs[i]);
        }
        s_packet.Timestamp = getTimeMsec();   
        s_packet.pid = process_id;
        s_packet.led_status = 0;
        s_packet.code = 1;
        s_packet.sock_fd = server_fd;
        
        
        //get size of payload from client
        size_t PayloadSize = 0;
        rbytes = read(new_socket, &PayloadSize, sizeof(size_t));
        fopen("log.txt", "a");
        printf("[SERVER LOG] Size of Client payload %ld\n",PayloadSize);
        fprintf(fp,"<%lf> [SERVER LOG] Size of Client payload %ld\n",getTimeMsec(),PayloadSize);
        size_t total_bytes = 0;

        char *recv_buffer =  (char*) malloc(PayloadSize);
        //get the actual payload
        while(total_bytes < PayloadSize)
        {
            rbytes = read(new_socket, recv_buffer+total_bytes, 1024);
            total_bytes+=rbytes;
        }
        printf("[SERVER LOG] Number of bytes recvd: %ld\n",total_bytes);
        payload_struct_t *packet_ptr= (payload_struct_t*)recv_buffer;
        for(int i = 0; i < 4; i++)
        {
            printf("[SERVER LOG][RECVD MESG] %s\n", packet_ptr->payload[i]);
            printf("[SERVER LOG][RECVD SIZE] %d\n", packet_ptr->payloadSize[i]);
        }
        printf("[SERVER LOG][RECVD TIME] %lf\n", packet_ptr->Timestamp);
        printf("[SERVER LOG][RECVD LED ] %d\n", packet_ptr->led_status);
        printf("[SERVER LOG][RECVD PID ] %d\n", packet_ptr->pid);
        printf("[SERVER LOG][RECVD CODE] %d\n", packet_ptr->code);
        printf("[SERVER LOG][RECVD SOCK] %d\n", packet_ptr->sock_fd);
    
        fprintf(fp,"<%lf> [SERVER LOG] Number of bytes recvd: %ld\n",getTimeMsec(),total_bytes);
        for(int i = 0; i < 4; i++)
        {
            fprintf(fp,"<%lf> [SERVER LOG][RECVD MESG] %s\n",getTimeMsec(), packet_ptr->payload[i]);
            fprintf(fp,"<%lf> [SERVER LOG][RECVD SIZE] %d\n",getTimeMsec(), packet_ptr->payloadSize[i]);
        }
        fprintf(fp,"<%lf> [SERVER LOG][RECVD TIME] %lf\n",getTimeMsec(), packet_ptr->Timestamp);
        fprintf(fp,"<%lf> [SERVER LOG][RECVD LED ] %d\n",getTimeMsec(), packet_ptr->led_status);
        fprintf(fp,"<%lf> [SERVER LOG][RECVD PID ] %d\n",getTimeMsec(), packet_ptr->pid);
        fprintf(fp,"<%lf> [SERVER LOG][RECVD CODE] %d\n",getTimeMsec(), packet_ptr->code);
        fprintf(fp,"<%lf> [SERVER LOG][RECVD SOCK] %d\n",getTimeMsec(), packet_ptr->sock_fd);
        
        //server ends messages back to client
        //size of server payload
        size_t PayloadSize2 = sizeof(s_packet);
        sbytes = send(new_socket,&PayloadSize2,sizeof(size_t), 0);
        fprintf(fp,"<%lf> SERVER sending <Packet Size %ld>\n",getTimeMsec(),PayloadSize);
        printf("[SERVER LOG] Size of Packet sent\n");
        fprintf(fp,"<%lf> [SERVER LOG] Size of Packet sent\n",getTimeMsec());
        
        //sending actual server message to client
        fprintf(fp,"<%lf> SERVER sending <Message %s>\n",getTimeMsec(),s_packet.payload[0]);
        fprintf(fp,"<%lf> SERVER sending <Message size %d>\n",getTimeMsec(),s_packet.payloadSize[0]);
        fprintf(fp,"<%lf> SERVER sending <Message %s>\n",getTimeMsec(),s_packet.payload[1]);
        fprintf(fp,"<%lf> SERVER sending <Message size %d>\n",getTimeMsec(),s_packet.payloadSize[1]);
        fprintf(fp,"<%lf> SERVER sending <Message %s>\n",getTimeMsec(),s_packet.payload[2]);
        fprintf(fp,"<%lf> SERVER sending <Message size %d>\n",getTimeMsec(),s_packet.payloadSize[2]);
        fprintf(fp,"<%lf> SERVER sending <Message %s>\n",getTimeMsec(),c_packet.payload[3]);
        fprintf(fp,"<%lf> SERVER sending <Message size %d>\n",getTimeMsec(),s_packet.payloadSize[3]);
        fprintf(fp,"<%lf> SERVER sending <Timestamp %lf>\n",getTimeMsec(),s_packet.Timestamp);
        fprintf(fp,"<%lf> SERVER sending <led status %d>\n",getTimeMsec(),s_packet.led_status);
        fprintf(fp,"<%lf> SERVER sending <Server pid %d>\n",getTimeMsec(),s_packet.pid);
        fprintf(fp,"<%lf> SERVER sending <Server code %d>\n",getTimeMsec(),s_packet.code);
        fprintf(fp,"<%lf> SERVER sending <Server sock %d>\n",getTimeMsec(),s_packet.sock_fd); 
        c_packet.Timestamp = getTimeMsec();  
        sbytes = send(new_socket , (char*)&s_packet , sizeof(s_packet), 0 );
        printf("[SERVER LOG] Transaction Complete\n");
        fprintf(fp,"<%lf> [SERVER LOG] Transaction Complete\n",getTimeMsec());
        fclose(fp);//----
        free(recv_buffer);     
    }
    fclose(fp1);
    return 0; 

}