/**
 * @file socket.h
 * @author Shreya Chakraborty
 * @brief header for socket functionality
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef SOCKET_H_
#define SOCKET_H_
/**
 * @brief to kill socket thread
 * 
 */
void kill_socket_thread(void);
/**
 * @brief Call back function for socket from main thread
 * 
 * @param threadp 
 * @return void* 
 */
void *socket_task(void *threadp);

#endif