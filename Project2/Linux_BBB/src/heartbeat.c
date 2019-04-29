/**
 * @file heartbeat.c
 * @author Shreya Chakraborty
 * @brief heartbeat functionality from all threads
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "mytimer.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"
#include "socket.h"
#include "comm_recv.h"
#include "comm_send.h"
#include "ext_clientHandler.h"

static volatile uint32_t g_heartbeat_taskFlags = 0;
#define h_sec 2
#define h_nsec 0
int counter = 0;
static pthread_mutex_t heartbeatFlagsLock = PTHREAD_MUTEX_INITIALIZER;

//Use this function in each thread to set the respective flag for each thread. 
//And make sure about the shifting and all
void set_heartbeatFlag(moduleId_t moduleId)
{
	pthread_mutex_lock(&heartbeatFlagsLock);
	g_heartbeat_taskFlags |= (1<<moduleId);
	pthread_mutex_unlock(&heartbeatFlagsLock);
}

void SystemExit(void)
{
	//kill_temp_thread();
	//kill_light_thread();
	kill_recv_thread();
	kill_send_thread();
	kill_ext_thread();
	//kill_logger_thread();
}

static bool systemExitInitiated = false; //to break out of heartbeat loop

void heartbeat_timer_callback(union sigval no)
{
	if(systemExitInitiated)
	{
		return;
	}
	uint32_t copy_flags = 0;
	pthread_mutex_lock(&heartbeatFlagsLock);
	uint32_t flags = g_heartbeat_taskFlags;
	g_heartbeat_taskFlags = 0;
	pthread_mutex_unlock(&heartbeatFlagsLock);
	copy_flags = flags;
	for(int i = 1; i < MAX_TASKS-1; i++)
	{
		if(!(flags & (1<<i)))
		{
            LOG_ERROR(MAIN_TASK,"HEARTBEAT </3 </3 </3 %s thread is DEAD x_x",moduleIdName[i]);
            GREENLEDOFF();
            REDLEDON();
			systemExitInitiated = true;
			SystemExit();
			break;
		}
        else {
            flags &= ~(1<<i);
            if((flags == 0) && (copy_flags == 0x0e))
            {
                LOG_INFO(MAIN_TASK,"HEARTBEAT <3 <3 <3 All threads working");
				static uint8_t greenLedON = 0; 
				if(greenLedON)
				{
					GREENLEDON();
					greenLedON ^= 1;
				}
				else
				{
					GREENLEDOFF();
					greenLedON ^= 1;
				}
				
            }
        }
	}
}

//start this after you are sure that all the threads are up and running fine - i.e. in their while() loop
void startHearbeatCheck(void)
{
    timer_t hearbeat_timer_id;
	maketimer(&hearbeat_timer_id, &heartbeat_timer_callback);
	startTimer(hearbeat_timer_id,h_sec,h_nsec);
}

