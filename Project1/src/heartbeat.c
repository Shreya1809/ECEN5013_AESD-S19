#include "includes.h"
#include "mytimer.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"

static volatile uint32_t g_heartbeat_taskFlags = 0;

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

void startSystemExit(void)
{
    pthread_cancel(pthread_self());
	//execute system clean shutdown
	//set the flags to false that you use to check in the while() loop for each thread.
	//for starters, you can use pthread_cancel for each thread.	
}

//use this as the timer callback
void heatbeat_timer_callback(union sigval no)
{
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
			//make sure the while(1) loop in each thread does not block in any way - use timeouts for dequeue operation and use timeouts in socket thread
			//for ex: if you have this callback executing every 2 sec, the while loop of each thread should not block more than 1.5 sec. 
            //printf("heartbeat for %s NOT received\n",moduleIdName[i]);
            LOG_ERROR(MAIN_TASK,"HEARTBEAT </3 </3 </3 %s thread is DEAD x_x",moduleIdName[i]);
            GREENLEDOFF();
            REDLEDON();
            startSystemExit();
			break;
		}
        else {
            flags &= ~(1<<i);
			//printf("After flags is 0x%0x\n",flags);
            if((flags == 0) && (copy_flags == 0x0e))
            {
                LOG_INFO(MAIN_TASK,"HEARTBEAT <3 <3 <3 All threads working");
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

