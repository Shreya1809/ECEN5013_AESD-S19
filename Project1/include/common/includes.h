/**
 * @file includes.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/select.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>
#include <openssl/md5.h>
#include <time.h>
#include <fcntl.h>           /* For O_* constants */


#define PORT 8888

typedef enum
{ 
    TCP_SEND,
    TCP_ACK, 
    
}tcp_enum_t;

typedef struct{
    uint32_t log_level;
    uint8_t log_mesg[1024];
    double Timestamp;
    uint16_t logger_modID;

} payload_struct_t;

static mqd_t loggertask_q;

#endif