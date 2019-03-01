/*  Author - Shreya Chakraborty
    date - 02.23.2019
    HW4, Q1
    Code References:
    1. http://timmurphy.org/2009/09/29/nanosleep-in-c-c/
    2. https://stackoverflow.com/questions/9229333/how-to-get-overall-cpu-usage-e-g-57-on-linux

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>


//shared resource among all threads
typedef struct thread_data{
  char * filename;
  unsigned long thread_id;
  unsigned long linux_id; 
}data_t; 

data_t thread_info;
//thread ids for the two thread
pthread_t del_thread;

//mutex for locking and unlocking shared resource 
pthread_mutex_t mutexlock;

// semaphore for syn
sem_t cpu_util_sem, thread_sem;
//pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
//flags for child1
volatile sig_atomic_t done = 0;

FILE *fp, *fp1 ,*fc;

//function declarations
void *child1(void *ptr);
void *child2(void *ptr);
//void signal_handler(int signum);


int  iret1, iret2; // to validate return values for pthread create
const char alphabet[26] = "abcdefghijklmnopqrstuvwxyz"; 

double getTimeMsec()
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_REALTIME, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}

static void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        fprintf(fp ,"-------Received SIGUSR1! Quitting CHILD2 ....\n");
        fprintf(fp,"Thread Exit Time(CHILD2) : %lf\n",getTimeMsec());
        printf("Received SIGUSR1 signal..\n");
        pthread_cancel(del_thread); 
    }
    if (signum == SIGUSR2)
    {
        fprintf(fp ,"-------Received SIGUSR2! Quitting CHILD2....\n");
        fprintf(fp,"Thread Exit Time(CHILD2) : %lf\n",getTimeMsec());
        printf("Received SIGUSR2 signal..\n");
        pthread_cancel(del_thread); 
    }
    if (signum == SIGINT)
    {
        fprintf(fp ,"-------Received SIGINT! Quitting CHILD2....\n");
        fprintf(fp,"Thread Exit Time(CHILD2) : %lf\n",getTimeMsec());
        printf("Received SIGINT signal..\n");
        pthread_cancel(del_thread); 
    }
}

void terminate(int signum)
{
    fprintf(fp ," -------Received Signal to Quit CHILD1....\n");
    fprintf(fp,"Thread Exit Time(CHILD1) : %lf\n",getTimeMsec());
    printf("Received quit signal for child1..\n");
    done = 1;
}

void cpu_util(void)
{
    char *command ="grep 'cpu ' /proc/stat | awk '{usage=($2+$4)*100/($2+$4+$5)} END {print usage}'"; 
    char str[10];
    fc = popen(command,"r");
    int val = fscanf(fc,"%s",str); 
    fprintf(fp,"%s%c\n",str,37);
    fclose(fc);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGINT, signal_handler);
}
static void giveSemUtil(union sigval no)
{
    sem_post(&cpu_util_sem);
    fprintf(fp,"[%lf] CPU UTILISATION -> " ,getTimeMsec());
}
int maketimer(timer_t *timerID)
{
    int val = 0;
    void *ptr;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = giveSemUtil;
    event.sigev_value.sival_ptr = ptr;
    event.sigev_notify_attributes = NULL; 
    val = timer_create(CLOCK_REALTIME, &event, timerID); 
     
    return val;
}

int startTimer(timer_t timerID)
{
    int val = 0;
    struct itimerspec its;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 100000000;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 100000000;
    timer_settime(timerID, 0, &its,0);

}
int main(int argc, char** argv) 
{ 
    pid_t p_id,pp_id;
    p_id = getpid();
    pthread_t thread_child1, thread_child2;
    printf("Master thread process ID : %d\n", p_id);
    thread_info.thread_id = pthread_self();
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    if (pthread_mutex_init(&mutexlock, NULL) != 0) //for locking 
    {
        printf("\n mutex init has failed\n");
        return 1;
    }
     if (argc < 2)
	{
		printf("USAGE: <FILENAME>\n");
		exit(1);
	}
    fp = fopen(argv[1], "w+");// create a file with name as given in command line
    if(fp == NULL)
    {
        printf("Unable to create file %s\n",argv[1]);
        return 1;
    }
    
    thread_info.filename = argv[1];
    //print master thread identity
    fprintf(fp,"\n*******In Master thread*******\n");
    fprintf(fp,"Thread Entry Time : %lf\n",getTimeMsec());
    fprintf(fp,"POXIS Thread ID: %lu\nLINUX Thread ID %lu\n",thread_info.thread_id,thread_info.linux_id);
    //creation of pthreads
    iret1 = pthread_create( &thread_child1, NULL, child1, (void*) &thread_info);
    usleep(3000);
    iret2 = pthread_create( &thread_child2, NULL, child2, (void*) &thread_info);

    if(iret1)
    {
        printf("Error - pthread_create() child1 return code: %d\n",iret1);
        exit(EXIT_FAILURE);
    }
    if(iret2)
    {
        printf("Error - pthread_create() child2 return code: %d\n",iret1);
        exit(EXIT_FAILURE);
    }

    pthread_join(thread_child1, NULL); 
    pthread_join(thread_child2, NULL);
    fprintf(fp,"\n*******Master Thread Exiting*******\n");
    fprintf(fp,"Thread Exit Time(MASTER) : %lf\n",getTimeMsec());
    fclose(fp); 
    return 0; 
} 

void *child1(void *ptr)
{
    struct stat st; // to find the size of file gdb.txt
    int countarr[30][26];
    long alphabet_count[26];
    long result;
    char readbuffer[1024];
    del_thread = pthread_self();
    int rbytes = 0, size = 0, filesize = 0 ,count = 0, loop = 0;
    thread_info.thread_id = pthread_self();
    //for terminating child 1
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = terminate;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);


    printf("Child1 thread ID : %d\n", getpid());
    //mutex lock for file processing
    pthread_mutex_lock(&mutexlock);
    if(fopen(thread_info.filename,"w")!= NULL);
    //write thread identification to file
    fprintf(fp,"\n*******In Child1 thread*******\n");
    fprintf(fp,"Thread Entry Time : %lf\n",getTimeMsec());
    fprintf(fp,"POXIS Thread ID: %lu\nLINUX Thread ID %lu\n",thread_info.thread_id,thread_info.linux_id);
    fp1 = fopen("gdb.txt","r");
    if(fp1 == NULL)
    {
        printf("Error Opening file gdb.txt\n");
        exit(EXIT_FAILURE);    
    }
    fstat(fileno(fp1), &st); // to find size of file
    size = st.st_size;
    while(filesize < size)
    {
        loop++; // number of iterations to read the entire gdb.txt
        rbytes = fread(readbuffer,1,1024,fp1);
        filesize = filesize+ rbytes;
        char temp[rbytes];
        memcpy(temp,readbuffer,rbytes);
        // the following code finds the occurence of each alphabet in a loop
        for (int i = 0; i < 26; i++)
        {
            for (int j = 0; j < rbytes ; j++)
            {
                if((temp[j] == alphabet[i]) || (temp[j] == (alphabet[i]-32)))
                count++;
            }
            countarr[loop][i] = count;
            count = 0;
        }

    }
    // add the occurences of all alphabets in all loops
    for (int j = 0; j < 26; j++)
    {
        for ( int i = 1 ; i <= loop; i ++)
        {
            result += countarr[i][j];
        } 
        alphabet_count[j] = result;
        result = 0; 
    }
    fprintf(fp, "*******Characters having less than 100 occurences*******\n");
    for( int i = 0 ; i < 26 ; i ++)
    {
        if(alphabet_count[i] < 100)
        {
            fprintf(fp,"<%lf>    %c  :   %ld\n",getTimeMsec(),alphabet[i], alphabet_count[i]);
        }    
    }
    
    fclose(fp1);
    // file operation done, unlock
    pthread_mutex_unlock(&mutexlock);
    printf("CHILD 1 thread exits in 5 secs unless you give exit signal -> SIGINT(CTRL-C) or SIGUSR1 or SIGUSR2\n");
    int i = 0;
    while((i < 5) && (!done))
    {
        printf("%d sec remaining....\n",(5-i));
        sleep(1);
        i++;
    }
    fprintf(fp,"*******Child1 Thread Exiting*******\n");
    fprintf(fp,"Thread Exit Time(CHILD1) : %lf\n",getTimeMsec());
    
    sem_post(&thread_sem);
    
    
    
}

void *child2(void *ptr)
{
    sem_wait(&thread_sem);
    timer_t child2_timer;
    thread_info.thread_id = pthread_self();
    del_thread = pthread_self();
    printf("Child2 thread ID : %d\n", getpid());
    sem_init(&cpu_util_sem,0,0);
    pthread_mutex_lock(&mutexlock);
    if(fopen(thread_info.filename,"w") != NULL);
    fprintf(fp,"\n*******In Child2 thread*******\n");
    fprintf(fp,"Thread Entry Time : %lf\n",getTimeMsec());
    fprintf(fp,"POXIS Thread ID: %lu\nLINUX Thread ID %lu\n",thread_info.thread_id,thread_info.linux_id);
    pthread_mutex_unlock(&mutexlock);
    
    if(maketimer(&child2_timer) != 0)
    {
        perror("MakeTimer fail");
    }
    if(startTimer(child2_timer) != 0);
    {
        //perror("StartTimer fail");
    }
    printf("CHILD 2 thread will keep on running until you give exit signal -> SIGINT(CTRL-C) or SIGUSR1 or SIGUSR2\n");
    while(1)
    {
        if(sem_wait(&cpu_util_sem) == 0)
        {
            cpu_util();
        }
    }
    
    
}

