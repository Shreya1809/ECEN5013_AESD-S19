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
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <sys/syscall.h>
#include <stdbool.h>
#include <math.h>

static volatile sig_atomic_t done = 0;
sem_t temp_sem;
sem_t light_sem;
sem_t socket_thread_sem;
sem_t light_thread_sem;
sem_t logger_thread_sem;
sem_t temp_thread_sem;


#define PORT 8888



#endif