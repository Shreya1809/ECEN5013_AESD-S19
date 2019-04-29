#include "includes.h"
#include "comm_recv.h"
#include "comm_send.h"
#include "socket.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "heartbeat.h"

pthread_mutex_t datalock = PTHREAD_MUTEX_INITIALIZER;
void timelimit(int sockfd, int io)
{
struct timeval timeout;      
timeout.tv_sec = 10;
timeout.tv_usec = 0;

    if (io == 1)
    {
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&timeout,sizeof(struct timeval)) < 0) 
        {
            printf("setsockopt for timeout-input failed");
        }
    }
    if(io == 2)
    {
        if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        {
            printf("setsockopt for timeout-output failed");
        }
    }
}

void Process_TemperatureData(packet_struct_t packet)
{
    float tempval = 0.0;
    bool motor_status = false;
    tempval = getCurrentTemperature();
    if(tempval == 0.0)
    {
        packet.data.temperature.connected = false;
    }
    else
    {
        packet.data.temperature.connected = true;
    }
    if(tempval < getTempThresLowVal() || tempval > getTempThresHighVal())
    {
        motor_status = true; 
        //packet.data.opcode = 
    }
    
}

void Process_UltrasonicData(packet_struct_t packet)
{
    int32_t val = 0;
    bool gear_status = false;
    gear_status = getReverseGearStatus();
    val = getCurentDist();
    if(val <= 0)
    {
        packet.data.dist.connected = false;
    }
    else
    {
        packet.data.dist.connected = true;
    }
    if(gear_status)
    {
        //
    }
        
} 

void Process_AccelerationData(packet_struct_t packet)
{
    uint16_t delx = 0,dely = 0,delz = 0;
    int16_t cur_x = 0,cur_y = 0,cur_z = 0;
    int16_t prev_x = 0,prev_y = 0,prev_z = 0;
    
    cur_x = getCurrentXaxis();
    cur_y = getCurrentYaxis();
    cur_z = getCurrentZaxis();
    delx = getXaxisDelta();
    dely = getYaxisDelta();
    delz = getZaxisDelta();
    prev_x = getPrevXaxis();
    prev_y = getPrevYaxis();
    prev_z = getPrevZaxis();

    if(prev_x != 0 && prev_y != 0 && prev_x != 0)
    {
        if((abs(prev_x - cur_x) > delx) || (abs(prev_y - cur_y) > dely) || (abs(prev_z - cur_z) > delz))
        {
            //set accident opcode.
        }
        else
        {
            //no accident
        }    
    }

}

