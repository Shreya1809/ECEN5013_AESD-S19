/*
 * server.c: Server program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


#define QUEUE_NAME   "/posix_q"
#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES 8
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 8

typedef struct{
    uint32_t payloadSize[4];
    uint8_t payload[4][1024];
    double Timestamp;
    uint16_t led_status;
    uint16_t pid;
    //uint16_t token;

} payload_struct_t;

volatile sig_atomic_t done = 0;

double getTimeMsec()
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_REALTIME, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}

void terminate(int signum)
{
    FILE *fp2;
    printf("SIGINT!\n");
    fp2 = fopen("log.txt","a");
    fprintf(fp2,"SIGINT! Received! [SERVER] <%lf> Terminating posix mq program....\n",getTimeMsec());
    fclose(fp2);
    done = 1;
}

int main (int argc, char **argv)
{
    mqd_t qd_server;   // queue descriptors
    char client_queue_name [64];
    sprintf (client_queue_name, "/client_q-%d", getpid ());
    char *server_mesg[] = {"Hello from Server","IPC poxis queue Programming","Server Process","Server says bye"};
    uint16_t token_number = 1; // next token to be given to client
    payload_struct_t send_packet = {0};
    payload_struct_t recv_packet = {0};
    //printf ("Server: Hello, World!\n");
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = terminate;
    sigaction(SIGINT, &action, NULL);
    FILE *fp, *fp1;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = sizeof(int);
    attr.mq_msgsize = sizeof(send_packet);
    attr.mq_curmsgs = 0;
    fp1 = fopen("log.txt","w+");
    if(fp1 == NULL)
    {
        perror("file not created");
    }
    fp = fopen("log.txt","a");
    if(fp == NULL)
    {
        perror("file not opened");
    }
    int process_id = getpid();
    fprintf(fp,"------------IPC MECHANISMS -> POSIX MQ--------------\n");
    printf("----Server Process ID: %d----\n",process_id);
    fprintf(fp,"<%lf>[SERVER LOG] Server Process ID %d\n",getTimeMsec(),process_id);
    fclose(fp);
    //filing the struct
    for(int i = 0; i < 4; i++)
    {
        memcpy(send_packet.payload[i],server_mesg[i],strlen(server_mesg[i])+1);
        send_packet.payloadSize[i] = strlen(server_mesg[i]);
    }
    send_packet.Timestamp = getTimeMsec();   
    send_packet.pid = process_id;
    send_packet.led_status = 0;
    //s_packet.token = 1;

    if ((qd_server = mq_open (QUEUE_NAME, O_CREAT | O_RDWR, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    
    //for receiving data
    payload_struct_t *s_packet_ptr = (payload_struct_t*)&recv_packet;
    
    while (!done) {
        // get the oldest message with highest priority
        if (mq_receive (qd_server, (char*)&recv_packet, sizeof(recv_packet), NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }
        fopen("log.txt","a");
        //printf("In Parent: Reading from pipe 2 – Message is %s\n",p_packet_ptr->payload[0]);
        fprintf(fp,"<%lf>[SERVER LOG] -------Server Process Received data from Client-------\n",getTimeMsec());
        for(int i = 0; i < 4; i++)
        {
            fprintf(fp,"<%lf>[SERVER LOG] Received < Message : %s >\n",getTimeMsec(), s_packet_ptr->payload[i]);
            fprintf(fp,"<%lf>[SERVER LOG] Received < Message size :%d >\n",getTimeMsec(), s_packet_ptr->payloadSize[i]);
        }
        fprintf(fp,"<%lf>[SERVER LOG] Received < Timestamp %lf >\n",getTimeMsec(), s_packet_ptr->Timestamp);
        fprintf(fp,"<%lf>[SERVER LOG] Received < Led Status %d >\n",getTimeMsec(), s_packet_ptr->led_status);
        fprintf(fp,"<%lf>[SERVER LOG] Received < Client Pid %d >\n",getTimeMsec(), s_packet_ptr->pid);
        fclose(fp);
        printf ("Server: message received.\n");

        // send reply message to client
        
        fp = fopen("log.txt","a");
        fprintf(fp,"<%lf>[SERVER LOG] -------Server Process Sending data to Client-------\n",getTimeMsec());
        for(int i = 0; i < 4; i++)
        {
            fprintf(fp,"<%lf>[SERVER LOG] Sending < Message : %s >\n",getTimeMsec(), send_packet.payload[i]);
            fprintf(fp,"<%lf>[SERVER LOG] Sending < Message size :%d >\n",getTimeMsec(), send_packet.payloadSize[i]);
        }
        fprintf(fp,"<%lf>[SERVER LOG] Sending < Timestamp %lf >\n",getTimeMsec(), send_packet.Timestamp);
        fprintf(fp,"<%lf>[SERVER LOG] Sending < Led Status %d >\n",getTimeMsec(), send_packet.led_status);
        fprintf(fp,"<%lf>[SERVER LOG] Sending < Server Pid %d >\n",getTimeMsec(), send_packet.pid);
        fclose(fp);
        
        //printf()
        if (mq_send (qd_server, (char*)&send_packet, sizeof(send_packet), 0) == -1) {
            perror ("Server: Not able to send message to client");
            continue;
        }

        printf ("Server: response sent to client.\n");
        token_number++;
    }
    fclose(fp1);
    return 0;

}
