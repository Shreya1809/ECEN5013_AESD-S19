#include "includes.h"
#include "comm_recv.h"
#include "comm_send.h"
#include "socket.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "heartbeat.h"

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


void Process_AccelerationData(packet_struct_t packet)
{
    uint16_t delx = 0,dely = 0,delz = 0;
    int16_t cur_x = 0,cur_y = 0,cur_z = 0;
    int16_t prev_x = 0,prev_y = 0,prev_z = 0;
    
    getCurrentAccel();
    delx = getXaxisDelta();
    dely = getYaxisDelta();
    delz = getZaxisDelta();
    prev_x = getPrevXaxis();
    prev_y = getPrevYaxis();
    prev_z = getPrevZaxis();

    if(prev_x != 0 && prev_y != 0 && prev_x != 0)
    {
        if((abs(cur_x - prev_x) > delx) || (abs(prev_y - cur_y) > dely) || (abs(prev_z - cur_z) > delz))
        {
            //set accident opcode.
        }
        else
        {
            //no accident
        }    
    }

}

