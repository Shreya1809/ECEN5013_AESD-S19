/**
 * @file mysignal.c
 * @author Shreya Chakraborty
 * @brief handles the CTRL-C keyboard input from user to exit cleanly
 * @version 0.1
 * @date 2019-03-31
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "bbgled.h"
#include "mysignal.h"
#include "heartbeat.h"
#include "logger.h"


void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        LOG_WARN(MAIN_TASK,"User exit signal received....");
        SystemExit();
    }
    
}
