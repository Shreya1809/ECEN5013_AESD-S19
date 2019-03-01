#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h> 
#include <time.h>
#include <stdint.h>
#include <semaphore.h>

#define SNAME "/mysem"  //name for opening named semaphore
#define name "/mySH"   //name of shared mem object

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

  
int main() 
{ 
	int process_id = getpid();
	const char const *proc1_mesg[] = {"Hello from Process 1","IPC poxis shared memory Programming","Process 1 string","Process 1 says bye"};
	payload_struct_t send_packet = {0};
    printf("Process 1 PID: %d\n",getpid());
    //payload_struct_t recv_packet = {0};
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = terminate;
    sigaction(SIGINT, &action, NULL);
    FILE *fp;
    fp = fopen("log.txt","a");
    if(fp == NULL)
    {
        perror("file not opened");
    }
    fprintf(fp,"<%lf>[PROCESS 1 LOG] Process ID %d\n",getTimeMsec(),process_id);
    sem_t *sem = sem_open(SNAME, O_CREAT, 0666, 0); /* Initial value is 3. */
    if(SEM_FAILED == sem)
    {
        perror("sem fail");
        return -1;
    } 
    fprintf(fp,"<%lf>[PROCESS 2 LOG] Named Semaphore Opened\n",getTimeMsec());
    /* the size (in bytes) of shared memory object */
    const int SIZE = sizeof(send_packet); 
  
    /* shared memory file descriptor */
    int shm_fd; 
  
    /* pointer to shared memory obect */
    void* ptr; 
  
    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if(shm_fd < 0)
    {
        perror("shmopen fail");
        return -1;
    } 
    fprintf(fp,"<%lf>[PROCESS 1 LOG] POSIX Shared Memory Opened\n",getTimeMsec());
    fclose(fp);
    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE); 
  
    for(int i = 0; i < 4; i++)
    {
        memcpy(send_packet.payload[i],proc1_mesg[i],strlen(proc1_mesg[i])+1);
        send_packet.payloadSize[i] = strlen(proc1_mesg[i]);
    }
    send_packet.Timestamp = getTimeMsec();   
    send_packet.pid = process_id;
    send_packet.led_status = 0;
    /* memory map the shared memory object */
    ptr = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if(MAP_FAILED == ptr)
    {
        perror("mmap fail");
        shm_unlink(name); 
        return -1;
    } 

    while(!done)
    {
        
        sleep(1);
        fopen("log.txt","a");
        fprintf(fp,"<%lf>[PROCESS 1 LOG] -----Copying Data by Process 1 to Shared Memory------\n",getTimeMsec());
        for(int i = 0; i < 4; i++)
        {
            fprintf(fp,"<%lf>[PROCESS 1 LOG] Sending < Message : %s >\n",getTimeMsec(), send_packet.payload[i]);
            fprintf(fp,"<%lf>[PROCESS 1 LOG] Sending < Message size :%d >\n",getTimeMsec(), send_packet.payloadSize[i]);
        }
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Sending < Timestamp %lf >\n",getTimeMsec(), send_packet.Timestamp);
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Sending < Led Status %d >\n",getTimeMsec(), send_packet.led_status);
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Sending < Process 1 Pid %d >\n",getTimeMsec(), send_packet.pid);
        //copy struct into shared memory, write process
        
        memcpy((char*)ptr,(char*)&send_packet, sizeof(send_packet));
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Posted SEM to process 2\n",getTimeMsec());
        fclose(fp);
        /*after write wait for the other process to read and then write to he shared memeory before
        reading it*/
        printf("Posting sem\n");
        sem_post(sem);//*
        sleep(1);
        sleep(1);
        printf("Waiting for sem\n");
        sem_wait(sem);
        fopen("log.txt","a");
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Waiting for SEM from Process 2\n",getTimeMsec());
        
        //read from shared memory, read process
        sleep(1);
        payload_struct_t *s_packet_ptr = (payload_struct_t*)ptr;
        fprintf(fp,"<%lf>[PROCESS 1 LOG] ------Recieved Data from Process 1-----\n",getTimeMsec());
        for(int i = 0; i < 4; i++)
        {
            fprintf(fp,"<%lf>[PROCESS 1 LOG] Received < Message : %s >\n",getTimeMsec(), s_packet_ptr->payload[i]);
            fprintf(fp,"<%lf>[PROCESS 1 LOG] Received < Message size :%d >\n",getTimeMsec(), s_packet_ptr->payloadSize[i]);
        }
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Received < Timestamp %lf >\n",getTimeMsec(), s_packet_ptr->Timestamp);
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Received < Led Status %d >\n",getTimeMsec(), s_packet_ptr->led_status);
        fprintf(fp,"<%lf>[PROCESS 1 LOG] Received < Process 2 Pid %d >\n",getTimeMsec(), s_packet_ptr->pid);
        printf("%s\n",s_packet_ptr->payload[0]);
        fclose(fp);
    }
    fopen("log.txt","a");
    fprintf(fp,"SIGINT! Received! [PROCESS 1] <%lf> Terminating shared mem program....\n",getTimeMsec());
    fclose(fp);
    shm_unlink(name);
    sem_unlink(SNAME); 
    return 0; 
} 