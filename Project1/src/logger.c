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
#include "bbgled.h"
#include "mysignal.h"

static mqd_t mq_logger = -1;
static log_level_t currentLogLevel = LOG_DEBUG;

const char * logLevel[LOG_MAX] = {
    "LOG_ERR",              //Error conditions
    "LOG_WARNING",          //Warning conditions
    "LOG_INFO",             //Informational
    "LOG_DEBUG"            //Debug-level messages
};
static log_level_t logger_getCurrentLogLevel()
{

  return currentLogLevel;
}

static log_level_t logger_setCurrentLogLevel(log_level_t newLevel)
{
  currentLogLevel = newLevel;
  return currentLogLevel;
}

/**
 * @brief Get the Time Msec object
 * 
 * @return double 
 */
double getTimeMsec()
{
  struct timespec event_ts = {0, 0};
  clock_gettime(CLOCK_REALTIME, &event_ts);
  return ((event_ts.tv_sec) * 1000.0) + ((event_ts.tv_nsec) / 1000000.0);
}

FILE *fp;

#define PRINTLOGFILE(log)  do{\
             printf("[%lf] [%s] [%s] [PID:%d] [TID:%ld] Message: ",getTimeMsec(),logLevel[log.level], moduleIdName[log.srcModuleID],getpid(),syscall(SYS_gettid));\
             printf("%s\n",log.msg );\
             fflush(stdout);\
             fprintf(fp,"[%lf] [%s] [%s] [PID:%d] [TID:%ld] Message: ", getTimeMsec(),logLevel[log.level], moduleIdName[log.srcModuleID],getpid(),syscall(SYS_gettid));\
             fprintf(fp, "%s\n",log.msg );\
             fflush(fp);\
            }while(0) 

int LOG_ENQUEUE(log_level_t level, moduleId_t modId, char *msg, ...)
{
  if(level > currentLogLevel)
  {
    return 0;
  }
  if (mq_logger > -1 && level > LOG_INVALID && level < LOG_MAX)
  {
    log_struct_t send_log;
    send_log.level = level;
    send_log.timestamp = getTimeMsec();
    send_log.srcModuleID = modId;
    va_list arg_ptr;
    va_start(arg_ptr, msg);
    vsnprintf(send_log.msg, sizeof(send_log.msg), msg, arg_ptr);
    va_end(arg_ptr);

    int ret = mq_send(mq_logger, (const char *)&send_log, sizeof(send_log), 0);
    if (ret)
      return ret;

    return 0;
  }

  printf("Log enqueue Error\n");
  return 1;
}


/**
 * @brief mesg queue attribute initialisation
 * 
 * @return int 
 */
mqd_t logger_queue_init(void)
{
  struct mq_attr attr;

  attr.mq_flags = FLAGS;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = sizeof(log_struct_t);
  attr.mq_curmsgs = CURRENT_MSG;

  mq_unlink(LOG_QUEUE_NAME);
  if ((mq_logger = mq_open(LOG_QUEUE_NAME, O_CREAT | O_RDWR, QUEUE_PERMISSIONS, &attr)) == -1)
  {
    perror("Client: mq_open (client)");
    exit(1);
  }

  return mq_logger;
}

/**
 * @brief call back function for the logger
 * 
 * @param threadp 
 * @return void* 
 */
void *logger_task(void *threadp)
{
  //signal_init();
  struct loggerTask_param *params = (struct loggerTask_param *)threadp;
  logger_setCurrentLogLevel(params->loglevel);
  LOG_DEBUG(LOGGER_TASK, "Logger Filename:%s ", params->filename);
  LOG_DEBUG(LOGGER_TASK, "Logger level:%d", params->loglevel);
  LOG_INFO(LOGGER_TASK,"Logger Task thread spawned");

  //log file
  fp = fopen(params->filename, "w+");
  if (fp == NULL)
  {
    PRINTLOGCONSOLE("Could not open file %s\n", params->filename);
    exit(EXIT_FAILURE);
  }
  fprintf(fp, "log file created\n");
  log_struct_t recv_log = {0};
  //while(!done){
    while(1){
      //deque msg
       if (mq_receive (mq_logger, (char*)&recv_log, sizeof(recv_log), NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
      //put the log struct in file
      #ifdef LOG_STDOUT
      PRINTLOGFILE(recv_log);
      #endif


  }
  fclose(fp);
  LOG_INFO(LOGGER_TASK,"Logger Task thread exiting");
  return NULL;
}