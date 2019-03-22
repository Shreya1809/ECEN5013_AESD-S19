
#ifndef MYSIGNAL_H_
#define MYSIGNAL_H_


/**
 * @brief initialisation for signal
 * 
 */
void signal_init(void);
/**
 * @brief signal for signal received
 * 
 * @param signum 
 */
void signal_handler(int signum);

#endif