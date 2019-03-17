/**
 * @file logger.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h" 
#include "logger.h"

#define QUEUE_NAME   "/logger_q"
#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES 8
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 8
/**
 * @brief Get the Time Msec object
 * 
 * @return double 
 */
double getTimeMsec()
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_REALTIME, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}
/**
 * @brief mesg queue attribute initialisation
 * 
 * @return int 
 */
int mesg_queue_init(void)
{
  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = sizeof(int);
  attr.mq_msgsize = sizeof(logger_packet);
  attr.mq_curmsgs = 0;
}
/**
 * @brief call back function for the logger
 * 
 * @param threadp 
 * @return void* 
 */
void *log_task(void *threadp)
{
  printf("The logger task!!!\n");
  return NULL;
}