#include "includes.h"
#include "mytimer.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"
#include "temp.h"
#include "light.h"
#include "socket.h"

static volatile uint32_t g_heartbeat_taskFlags = 0;
int counter = 0;
static pthread_mutex_t heartbeatFlagsLock = PTHREAD_MUTEX_INITIALIZER;

//Use this function in each thread to set the respective flag for each thread. 
//And make sure about the shifting and all
void set_heartbeatFlag(moduleId_t moduleId)
{
	pthread_mutex_lock(&heartbeatFlagsLock);
	g_heartbeat_taskFlags |= (1<<moduleId);
    //printf("the module %d and global taskflag 0x%x\n",moduleId,g_heartbeat_taskFlags);
	pthread_mutex_unlock(&heartbeatFlagsLock);
}

void SystemExit(void)
{
	kill_temp_thread();
	//usleep(1000);
	kill_light_thread();
	//usleep(1000);
	kill_socket_thread();
	//usleep(1000);
	kill_logger_thread();
}

static bool systemExitInitiated = false;

//use this as the timer callback
void heatbeat_timer_callback(union sigval no)
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
			//printf("%s\n",moduleIdName[i]);
			// counter++;
			// if(counter > 4)
			// {
				systemExitInitiated = true;
				SystemExit();
			// }
			break;
			// continue;
		}
        else {
            flags &= ~(1<<i);
			//printf("After flags is 0x%0x\n",flags);
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
	maketimer(&hearbeat_timer_id, &heatbeat_timer_callback);
	startTimerHB(hearbeat_timer_id);
}

