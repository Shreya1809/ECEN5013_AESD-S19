
#include "includes.h"
#include "comm_send.h"
#include "socket.h"
#include "logger.h"
#include "bbgled.h"
#include "comm_recv.h"
#include "ext_clientHandler.h"
#include "main.h"
#include "comm_recv.h"
#include "sensorData.h"


static volatile bool reverse_gear_flag_state = false;
static volatile float temp_low_threshold = 10.0;
static volatile float temp_high_threshold = 30.0;
static volatile uint16_t x_delta = 0;
static volatile uint16_t y_delta = 0;
static volatile uint16_t z_delta = 0;
pthread_mutex_t thresholdlock = PTHREAD_MUTEX_INITIALIZER;

void putIn_ReverseGear(void)
{
    // pthread_mutex_lock(&thresholdlock);
    if(reverse_gear_flag_state  == false)
    {
        LOG_DEBUG(EXT_TASK," Reverse gear flag is set to true from false");
        reverse_gear_flag_state  = true;
        COMM_SEND(reverseGearStateControl, (void*)&reverse_gear_flag_state);
    }
  
    // pthread_mutex_unlock(&thresholdlock);
}

void removeFrom_ReverseGear(void)
{
    // pthread_mutex_lock(&thresholdlock);
    if(reverse_gear_flag_state  == true)
    {
        reverse_gear_flag_state  = false;
        COMM_SEND(reverseGearStateControl, (void*)&reverse_gear_flag_state);
    }
    // pthread_mutex_unlock(&thresholdlock);
}

bool getReverseGearStatus(void)
{
    bool status = false;
    // pthread_mutex_lock(&thresholdlock);
	  status = reverse_gear_flag_state;
    // pthread_mutex_unlock(&thresholdlock);
    return status;
}

void SetRemoteNodeTempThresholdValues(float low,float high)
{
    pthread_mutex_lock(&thresholdlock);
	  temp_low_threshold = low;
    temp_high_threshold = high;
	  pthread_mutex_unlock(&thresholdlock);    
}

float getTempThresLowVal(void)
{
    float lowval = 0.0;
    pthread_mutex_lock(&thresholdlock);
    lowval = temp_low_threshold;
    pthread_mutex_unlock(&thresholdlock); 
    return lowval;   
}

float getTempThresHighVal(void)
{
    float highval = 0.0;
    pthread_mutex_lock(&thresholdlock);
    highval = temp_high_threshold;
    pthread_mutex_unlock(&thresholdlock); 
    return highval;   
}

void SetRemoteNodeAccelDelta(int16_t x,int16_t y,int16_t z)
{
    pthread_mutex_lock(&thresholdlock);
    x_delta = x;
    y_delta = y;
    z_delta = z;
    pthread_mutex_unlock(&thresholdlock);
}

void getAccelDelta(int16_t *x,int16_t *y,int16_t *z)
{
    pthread_mutex_lock(&thresholdlock);
    *x = x_delta;
    *y = y_delta;
    *z = z_delta;
    pthread_mutex_unlock(&thresholdlock);
}


static sig_atomic_t stop_ext_thread = 0;

void kill_ext_thread(void)
{
    LOG_DEBUG(EXT_TASK,"Ext thread exit signal received");
    stop_ext_thread = 1;
    pthread_cancel(threads[EXT_TASK]);    
}

void *ext_clientHandle_task(void *threadp)
{
    LOG_INFO(EXT_TASK,"external client handler task thread spawned");
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1, m = 0; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0};
    char buffer1[1024] = {0};
    char buffer2[1024] = {0};
    char mesg[1024] ={0};
    char mesg1[1024] ={0}; 
    char mesg2[1024] ={0};
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
    LOG_INFO(EXT_TASK,"Socket has been created");
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( 9999 ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    LOG_INFO(EXT_TASK,"socket binded"); 
    if (listen(server_fd, 10) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
    LOG_INFO(EXT_TASK,"socket listening"); 
    while(!stop_ext_thread  && (strcmp(buffer,"7") != 0))
    {
        //set_heartbeatFlag(EXT_TASK);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                        (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        LOG_INFO(EXT_TASK,"socket connection accepted from Remote Client"); 
        while(!stop_ext_thread)
        {
            mesg[1024] = '\0';
            mesg1[1024] = '\0';
            mesg2[1024] = '\0';
            valread = read( new_socket , buffer, 1024); 
            if(strcmp(buffer,"\0") == 0)
            {
                LOG_INFO(EXT_TASK,"Socket disconnected from remote client");
                break;
            }
            LOG_INFO(EXT_TASK,"Remote Client request number is %s",buffer ); 

            if(strcmp(buffer,"1") == 0)
            {
                temp_data_t temp = getCurrentTemperature();
                sprintf(mesg,"[%d]Current Car Temperature %f %c",temp.connected, temp.floatingpoint, temp.unit);
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"2") == 0)
            {
                accel_data_t accel = getCurrentAccel();
                sprintf(mesg,"[%d][%x]Current Car Acceleration values are %d,%d and %d",accel.connected, accel.devId, accel.x,accel.y,accel.z);
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"3") == 0)
            {
                char *p;
                int i = 0;
                float f_val[2] = {0.0};
                sprintf(mesg1,"Setting threshold values for temp");
                send(new_socket , mesg1 , strlen(mesg1) , 0 ); 
                valread = read( new_socket , buffer1, 1024); 
                p = strtok(buffer1,",");
                while(p != NULL)
                {
                    f_val[i] = atof(p);
                    p = strtok(NULL,",");
                    i++;
                }
                LOG_DEBUG(EXT_TASK,"Low and High Thresholds are %f and %f",f_val[0],f_val[1]);
                SetRemoteNodeTempThresholdValues(f_val[0],f_val[1]);
            }
            if(strcmp(buffer,"4") == 0)
            {
                char *p;
                int i = 0;
                uint16_t val[3] = {0.0};
                sprintf(mesg2,"Setting delta threshold values for acceleration");
                send(new_socket , mesg2 , strlen(mesg2) , 0 ); 
                valread = read( new_socket , buffer2, 1024); 
                p = strtok(buffer2,",");
                while(p != NULL)
                {
                    val[i] = atof(p);
                    p = strtok(NULL,",");
                    i++;
                }
                LOG_DEBUG(EXT_TASK,"The acceleration delta values  are %d, %d and %d",val[0],val[1],val[2]);
                SetRemoteNodeAccelDelta(val[0],val[1],val[2]);
            }
            if(strcmp(buffer,"5") == 0)
            {
                //Set Car in reverse Gear for parking
                putIn_ReverseGear();
                sprintf(mesg,"Ultrasonic Sensor in the back of the car activated");
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"6") == 0)
            {
                removeFrom_ReverseGear();
                sprintf(mesg,"Ultrasonic Sensor in the back of the car deactivated");
                send(new_socket , mesg , strlen(mesg) , 0 );
            }
            if(strcmp(buffer,"7") == 0)
            {
                LOG_INFO(EXT_TASK,"Client has requested Control Node to Exit");
                kill_recv_thread();
	            kill_send_thread();
	            kill_ext_thread();
                close(new_socket);
                break;
            } 
            LOG_INFO(EXT_TASK,"Client Request processed");
            }

    }
    close(new_socket);
//exit:
    PRINTLOGCONSOLE("external client handler task closed");
    LOG_INFO(EXT_TASK,"external client handler task closed");
    return NULL;
}