#include "includes.h"

double getTimeMsec()
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_MONOTONIC, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}

int main()
{
    struct sockaddr_in address; 
    int sock = 0, valread, process_id,sbytes,rbytes; 
    struct sockaddr_in serv_addr; 
    char *mesgs[] = {"Hello from client","IPC socket Programming","Client Process","Client says bye"}; 
    char buffer[1024] = {0}; 
    process_id = getpid();
    
    FILE *fp;
    fp = fopen("log.txt", "a");
    if(fp == NULL)
    {
        perror("file not opened");
        return -1;
    }
    fprintf(fp,"<%lf> [CLIENT LOG] Client Process ID: %d\n",getTimeMsec(),process_id);
    printf("[CLIENT LOG] Client Process ID: %d\n",process_id);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
    
    printf("[CLIENT LOG] Socket Created\n");
    fprintf(fp,"<%lf> [CLIENT LOG] Socket Created\n",getTimeMsec());  
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
    printf("[CLIENT LOG] Socket Connection Established\n"); 
    fprintf(fp,"<%lf> [CLIENT LOG] Socket Connection Established\n",getTimeMsec()); 
    fclose(fp);

    for(int i = 0; i < 4; i++)
    {
        memcpy(c_packet.payload[i],mesgs[i],strlen(mesgs[i])+1);
        c_packet.payloadSize[i] = strlen(mesgs[i]);
    }
    c_packet.Timestamp = getTimeMsec();   
    c_packet.pid = process_id;
    c_packet.led_status = 1;
    c_packet.code = 0;
    c_packet.sock_fd = sock;

    fopen("log.txt", "a");
    //send size of the packet first
    size_t PayloadSize = sizeof(c_packet);
    sbytes = send(sock,&PayloadSize,sizeof(size_t), 0);
    fprintf(fp,"<%lf> CLIENT sending <Packet Size %ld>\n",getTimeMsec(),PayloadSize);
    printf("[CLIENT LOG] Size of Packet sent\n");
    fprintf(fp,"<%lf> [CLIENT LOG] Size of Packet sent\n",getTimeMsec());
     //sending 1st message
    c_packet.Timestamp = getTimeMsec(); 
    sbytes = send(sock , (char*)&c_packet , sizeof(c_packet), 0 );
    fclose(fp);
    fopen("log.txt", "a");
    fprintf(fp,"<%lf> CLIENT sending <Message %s>\n",getTimeMsec(),c_packet.payload[0]);
    fprintf(fp,"<%lf> CLIENT sending <Message size %d>\n",getTimeMsec(),c_packet.payloadSize[0]);
    fprintf(fp,"<%lf> CLIENT sending <Message %s>\n",getTimeMsec(),c_packet.payload[1]);
    fprintf(fp,"<%lf> CLIENT sending <Message size %d>\n",getTimeMsec(),c_packet.payloadSize[1]);
    fprintf(fp,"<%lf> CLIENT sending <Message %s>\n",getTimeMsec(),c_packet.payload[2]);
    fprintf(fp,"<%lf> CLIENT sending <Message size %d>\n",getTimeMsec(),c_packet.payloadSize[2]);
    fprintf(fp,"<%lf> CLIENT sending <Message %s>\n",getTimeMsec(),c_packet.payload[3]);
    fprintf(fp,"<%lf> CLIENT sending <Message size %d>\n",getTimeMsec(),c_packet.payloadSize[3]);
    fprintf(fp,"<%lf> CLIENT sending <Timestamp %lf>\n",getTimeMsec(),c_packet.Timestamp);
    fprintf(fp,"<%lf> CLIENT sending <led status %d>\n",getTimeMsec(),c_packet.led_status);
    fprintf(fp,"<%lf> CLIENT sending <CLient pid %d>\n",getTimeMsec(),c_packet.pid);
    fprintf(fp,"<%lf> CLIENT sending <Client code %d>\n",getTimeMsec(),c_packet.code);
    fprintf(fp,"<%lf> CLIENT sending <Client sock %d>\n",getTimeMsec(),c_packet.sock_fd); 
    
    fclose(fp);//****
    //get size of payload form server
    size_t PayloadSize2 = 0;
    rbytes = read(sock, &PayloadSize2, sizeof(size_t));
    fopen("log.txt", "a");
    printf("[CLIENT LOG] Size of Server payload %ld\n",PayloadSize2);
    fprintf(fp,"<%lf> [CLIENT LOG] Size of Server payload %ld\n",getTimeMsec(),PayloadSize2);
    size_t total_bytes = 0;
    char *recv_buffer =  (char*) malloc(PayloadSize2);
    //get the actual payload
    while(total_bytes < PayloadSize)
    {
        rbytes = read(sock, recv_buffer+total_bytes, 1024);
        total_bytes+=rbytes;
    }
    printf("[CLIENT LOG] Number of bytes recvd: %ld\n",total_bytes);
    payload_struct_t *packet_ptr= (payload_struct_t*)recv_buffer;
    for(int i = 0; i < 4; i++)
    {
        printf("[CLIENT LOG][RECVD MESG] %s\n", packet_ptr->payload[i]);
        printf("[CLIENT LOG][RECVD SIZE] %d\n", packet_ptr->payloadSize[i]);
    }
    printf("[CLIENT LOG][RECVD TIME] %lf\n", packet_ptr->Timestamp);
    printf("[CLIENT LOG][RECVD LED ] %d\n", packet_ptr->led_status);
    printf("[CLIENT LOG][RECVD PID ] %d\n", packet_ptr->pid);
    printf("[CLIENT LOG][RECVD CODE] %d\n", packet_ptr->code);
    printf("[CLIENT LOG][RECVD SOCK] %d\n", packet_ptr->sock_fd);

    fprintf(fp,"<%lf> [CLIENT LOG] Number of bytes recvd: %ld\n",getTimeMsec(),total_bytes);
    for(int i = 0; i < 4; i++)
    {
        fprintf(fp,"<%lf> [CLIENT LOG][RECVD MESG] %s\n",getTimeMsec(), packet_ptr->payload[i]);
        fprintf(fp,"<%lf> [CLIENT LOG][RECVD SIZE] %d\n",getTimeMsec(), packet_ptr->payloadSize[i]);
    }
    fprintf(fp,"<%lf> [CLIENT LOG][RECVD TIME] %lf\n",getTimeMsec(), packet_ptr->Timestamp);
    fprintf(fp,"<%lf> [CLIENT LOG][RECVD LED ] %d\n",getTimeMsec(), packet_ptr->led_status);
    fprintf(fp,"<%lf> [CLIENT LOG][RECVD PID ] %d\n", getTimeMsec(),packet_ptr->pid);
    fprintf(fp,"<%lf> [CLIENT LOG][RECVD CODE] %d\n",getTimeMsec(), packet_ptr->code);
    fprintf(fp,"<%lf> [CLIENT LOG][RECVD SOCK] %d\n",getTimeMsec(), packet_ptr->sock_fd);
   
    fclose(fp);
    return 0; 
} 
