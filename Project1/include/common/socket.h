/**
 * @file socket.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef SOCKET_H_
#define SOCKET_H_
/**
 * @brief Call back function for socket from main thread
 * 
 * @param threadp 
 * @return void* 
 */
void *socket_task(void *threadp);

#endif