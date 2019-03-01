/*
 * client.c: Client program
 *           to demonstrate interprocess communication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define QUEUE_NAME   "/posix_q"
#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES 8
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

typedef struct{
    uint32_t payloadSize[4];
    uint8_t payload[4][1024];
    double Timestamp;
    uint16_t led_status;
    uint16_t pid;

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
   printf("SIGINT!\n");
   done = 1;
}

int main (int argc, char **argv)
{
    char client_queue_name [64];
    char *client_mesg[] = {"Hello from Client","IPC poxis queue Programming","Client Process","Client says bye"};
    mqd_t qd_client;   // queue descriptors
    payload_struct_t send_packet = {0};
    payload_struct_t recv_packet = {0};
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = terminate;
    sigaction(SIGINT, &action, NULL);
    FILE *fp;
    // create the client queue for receiving messages from server
    sprintf (client_queue_name, "/client_q-%d", getpid ());

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = sizeof(int);
    attr.mq_msgsize = sizeof(send_packet);
    attr.mq_curmsgs = 0;

    //printf("1 .sizeof(c_packet) : %d\n",sizeof(c_packet));
    fp = fopen("log.txt","a");
    if(fp == NULL)
    {
        perror("file not opened");
    }
    int process_id = getpid();
    printf("----Client Process ID: %d----\n",process_id);
    fprintf(fp,"<%lf>[CLIENT LOG] Client Process ID %d\n",getTimeMsec(),process_id);
    fclose(fp);
    //filing the struct
    for(int i = 0; i < 4; i++)
    {
        memcpy(send_packet.payload[i],client_mesg[i],strlen(client_mesg[i])+1);
        send_packet.payloadSize[i] = strlen(client_mesg[i]);
    }
    send_packet.Timestamp = getTimeMsec();   
    send_packet.pid = process_id;
    send_packet.led_status = 0;
    
    //for receiving data
    payload_struct_t *s_packet_ptr=  (payload_struct_t*)&recv_packet;

    if ((qd_client = mq_open (QUEUE_NAME, O_CREAT | O_RDWR, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: mq_open (client)");
        exit (1);
    }


    while (!done) {

        fp = fopen("log.txt","a");
        fprintf(fp,"<%lf>[CLIENT LOG] -------Client Process Sending data to Server-------\n",getTimeMsec());
        for(int i = 0; i < 4; i++)
        {
            fprintf(fp,"<%lf>[CLIENT LOG] Sending < Message : %s >\n",getTimeMsec(), send_packet.payload[i]);
            fprintf(fp,"<%lf>[CLIENT LOG] Sending < Message size :%d >\n",getTimeMsec(), send_packet.payloadSize[i]);
        }
        fprintf(fp,"<%lf>[CLIENT LOG] Sending < Timestamp %lf >\n",getTimeMsec(), send_packet.Timestamp);
        fprintf(fp,"<%lf>[CLIENT LOG] Sending < Led Status %d >\n",getTimeMsec(), send_packet.led_status);
        fprintf(fp,"<%lf>[CLIENT LOG] Sending < Client Pid %d >\n",getTimeMsec(), send_packet.pid);
        fclose(fp);

        // send message to server
        if (mq_send (qd_client, (char*)&send_packet , sizeof(send_packet), 0) == -1) {
            perror ("Client: Not able to send message to server");
            //continue;
            exit(1);
        }
        
        // receive response from server

        if (mq_receive (qd_client, (char*)&recv_packet, sizeof(recv_packet), NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        fopen("log.txt","a");
        //printf("In Parent: Reading from pipe 2 – Message is %s\n",p_packet_ptr->payload[0]);
        fprintf(fp,"<%lf>[CLIENT LOG] -------Client Process Received data from Server-------\n",getTimeMsec());
        for(int i = 0; i < 4; i++)
        {
            fprintf(fp,"<%lf>[CLIENT LOG] Received < Message : %s >\n",getTimeMsec(), s_packet_ptr->payload[i]);
            fprintf(fp,"<%lf>[CLIENT LOG] Received < Message size :%d >\n",getTimeMsec(), s_packet_ptr->payloadSize[i]);
        }
        fprintf(fp,"<%lf>[CLIENT LOG] Received < Timestamp %lf >\n",getTimeMsec(), s_packet_ptr->Timestamp);
        fprintf(fp,"<%lf>[CLIENT LOG] Received < Led Status %d >\n",getTimeMsec(), s_packet_ptr->led_status);
        fprintf(fp,"<%lf>[CLIENT LOG] Received < Server Pid %d >\n",getTimeMsec(), s_packet_ptr->pid);
        fclose(fp);
        // display token received from server
        
        printf ("client received\n");
        sleep(5);
    }
    fopen("log.txt","a");
    fprintf(fp,"SIGINT! Received! [CLIENT] <%lf> Terminating posix mq program....\n",getTimeMsec());
    fclose(fp);

    if (mq_close (qd_client) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_unlink (client_queue_name) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    printf ("Client: bye\n");

    return 0;
} 