#include<stdio.h>
#include<unistd.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <signal.h>

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
typedef struct{
    uint32_t payloadSize[4];
    uint8_t payload[4][1024];
    double Timestamp;
    uint16_t led_status;
    uint16_t pid;

} payload_struct_t;


int main() {
    int pipefds1[2], pipefds2[2];
    int ret1, ret2;
    int pid,rbytes,sbytes;
    char *c_to_p[] = {"Hello from Child","IPC pipes Programming","Child Process","Child says bye"}; 
    char *p_to_c[] = {"Hello from Parent","IPC pipes Programming","Parent Process","Parent says bye"};
    char readbuffer[20];
    int process_id = 0;
    payload_struct_t c_packet; //child packet
    payload_struct_t p_packet;// parent packet
    int buffsize = sizeof(payload_struct_t);
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = terminate;
    sigaction(SIGINT, &action, NULL);

    FILE *fp, *fp1;
    fp1 = fopen("log.txt","w+");
    if(fp1 == NULL)
    {
        perror("file not created" );
        return -1;
    }
    fp = fopen("log.txt","a");
    if(fp == NULL)
    {
        perror("file could not be opened");
        return -1;
    }

    fprintf(fp,"------------IPC MECHANISMS -> PIPES--------------\n");
    
    ret1 = pipe(pipefds1); //creating 2 pipes for 2 way communication, pipe 1
    if (ret1 == -1) {
        printf("Unable to create pipe 1 \n");
        return 1;
    }
    fprintf(fp,"<%lf>PIPE 1 CREATED SUCCESSFULLY\n",getTimeMsec());

    ret2 = pipe(pipefds2);  //creating 2 pipes for 2 way communication, pipe 1
    if (ret2 == -1) {
        printf("Unable to create pipe 2 \n");
        return 1;
    }

    fprintf(fp,"<%lf>PIPE 2 CREATED SUCCESSFULLY\n",getTimeMsec());
    pid = fork();
    if(pid >= 0)
    {
        fprintf(fp,"<%lf>FORK SUCCESSFULL\n",getTimeMsec());   
    }
    fclose(fp);
    while(!done)
    {
        if (pid != 0)
        { // Parent process 
            fopen("log.txt","a");
            process_id = getpid();
            printf("----Parent Process ID: %d----\n",process_id);
            fprintf(fp,"<%lf>[PARENT LOG] Parent Process ID %d\n",getTimeMsec(),process_id);
            //filing the struct
            for(int i = 0; i < 4; i++)
            {
                memcpy(p_packet.payload[i],p_to_c[i],strlen(p_to_c[i])+1);
                p_packet.payloadSize[i] = strlen(p_to_c[i]);
            }
            p_packet.Timestamp = getTimeMsec();   
            p_packet.pid = process_id;
            p_packet.led_status = 0;
            
            //send mesg to child, write
            close(pipefds1[0]); // Close the unwanted pipe1 read side
            close(pipefds2[1]); // Close the unwanted pipe2 write side
            fprintf(fp,"<%lf>[PARENT LOG] ----- Parent Process Sending data to Child -----\n",getTimeMsec());
            for(int i = 0; i < 4; i++)
            {
                fprintf(fp,"<%lf>[PARENT LOG] Sending < Message : %s >\n",getTimeMsec(), p_packet.payload[i]);
                fprintf(fp,"<%lf>[PARENT LOG] Sending < Message size :%d >\n",getTimeMsec(), p_packet.payloadSize[i]);
            }
            fprintf(fp,"<%lf>[PARENT LOG] Sending < Timestamp %lf >\n",getTimeMsec(), p_packet.Timestamp);
            fprintf(fp,"<%lf>[PARENT LOG] Sending < Led Status %d >\n",getTimeMsec(), p_packet.led_status);
            fprintf(fp,"<%lf>[PARENT LOG] Sending < Parent Pid %d >\n",getTimeMsec(), p_packet.pid);
            
            fclose(fp);
            char *pw_ptr = (char*)malloc(buffsize);
            pw_ptr = (char*)&p_packet;
            write(pipefds1[1], pw_ptr, buffsize);
            
            //get message from child, read
            char *pr_ptr = (char*)malloc(buffsize);
            size_t total_bytes = 0;
            while(total_bytes < buffsize)
            {
                if((rbytes = read(pipefds2[0], pr_ptr+total_bytes, 1024)) < 0)
                {
                    perror("parent read error");
                }
                total_bytes+=rbytes;
            }
            payload_struct_t *p_packet_ptr= (payload_struct_t*)pr_ptr;
            fopen("log.txt","a");
            fprintf(fp,"<%lf>[PARENT LOG] ----- Parent Process Received data from Child -----\n",getTimeMsec());
            for(int i = 0; i < 4; i++)
            {
                fprintf(fp,"<%lf>[PARENT LOG] Received < Message : %s >\n",getTimeMsec(), p_packet_ptr->payload[i]);
                fprintf(fp,"<%lf>[PARENT LOG] Received < Message size :%d >\n",getTimeMsec(), p_packet_ptr->payloadSize[i]);
            }
            fprintf(fp,"<%lf>[PARENT LOG] Received < Timestamp %lf >\n",getTimeMsec(), p_packet_ptr->Timestamp);
            fprintf(fp,"<%lf>[PARENT LOG] Received < Led Status %d >\n",getTimeMsec(), p_packet_ptr->led_status);
            fprintf(fp,"<%lf>[PARENT LOG] Received < Parent Pid %d >\n",getTimeMsec(), p_packet_ptr->pid);
            fclose(fp);
            sleep(5); 

        }
        else 
        { //child process
            process_id = getpid();
            fopen("log.txt","a");
            printf("----Child Process ID: %d----\n",process_id);
            fprintf(fp,"<%lf>[CHILD LOG] Child Process ID %d\n",getTimeMsec(),process_id);
            rbytes = 0;
            //filing the struct
            for(int i = 0; i < 4; i++)
            {
                memcpy(c_packet.payload[i],c_to_p[i],strlen(c_to_p[i])+1);
                c_packet.payloadSize[i] = strlen(c_to_p[i]);
            }
            c_packet.Timestamp = getTimeMsec();   
            c_packet.pid = process_id;
            c_packet.led_status = 1;

            //get message from parent, read
            char *cr_ptr = (char*)malloc(buffsize);
            size_t total_bytes = 0;
            close(pipefds1[1]); // Close the unwanted pipe1 write side
            close(pipefds2[0]); // Close the unwanted pipe2 read side
            
            while(total_bytes < buffsize)
            {
                if((rbytes = read(pipefds1[0], cr_ptr+total_bytes, 1024)) < 0)
                {
                    perror("child read error");
                }
                total_bytes+=rbytes;
                
            }
            
            payload_struct_t *c_packet_ptr= (payload_struct_t*)cr_ptr;
            fprintf(fp,"<%lf>[CHILD LOG] ----- Child Process Received data from parent -----\n",getTimeMsec());
            for(int i = 0; i < 4; i++)
            {
                fprintf(fp,"<%lf>[CHILD LOG] Received < Message : %s >\n",getTimeMsec(), c_packet_ptr->payload[i]);
                fprintf(fp,"<%lf>[CHILD LOG] Received < Message size :%d >\n",getTimeMsec(), c_packet_ptr->payloadSize[i]);
            }
            fprintf(fp,"<%lf>[CHILD LOG] Received < Timestamp %lf >\n",getTimeMsec(), c_packet_ptr->Timestamp);
            fprintf(fp,"<%lf>[CHILD LOG] Received < Led Status %d >\n",getTimeMsec(), c_packet_ptr->led_status);
            fprintf(fp,"<%lf>[CHILD LOG] Received < Child Pid %d >\n",getTimeMsec(), c_packet_ptr->pid);
            //send to parent write
            fprintf(fp,"<%lf>[CHILD LOG]----- Child Process Sending data to parent -----\n",getTimeMsec());
            for(int i = 0; i < 4; i++)
            {
                fprintf(fp,"<%lf>[CHILD LOG] Sending < Message : %s >\n",getTimeMsec(), c_packet.payload[i]);
                fprintf(fp,"<%lf>[CHILD LOG] Sending < Message size :%d >\n",getTimeMsec(), c_packet.payloadSize[i]);
            }
            fprintf(fp,"<%lf>[CHILD LOG] Sending < Timestamp %lf >\n",getTimeMsec(), c_packet.Timestamp);
            fprintf(fp,"<%lf>[CHILD LOG] Sending < Led Status %d >\n",getTimeMsec(), c_packet.led_status);
            fprintf(fp,"<%lf>[CHILD LOG] Sending < Child Pid %d >\n",getTimeMsec(), c_packet.pid);
            char *cw_ptr = (char*)malloc(buffsize);
            cw_ptr = (char*)&c_packet;
            write(pipefds2[1], cw_ptr, buffsize);
            fclose(fp);
        }
    }
    fopen("log.txt", "a");
    fprintf(fp,"SIGINT! Received! <%lf> Terminating pipes program....\n",getTimeMsec());
    fclose(fp);
    fclose(fp1);
    return 0;

}