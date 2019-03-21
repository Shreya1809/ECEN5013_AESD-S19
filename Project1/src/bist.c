/**
 * @file bist.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-03-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "includes.h"
#include "bist.h"
#include "logger.h"
#include "main.h"
#include "bbgled.h"

void bist_init(void)
{
   
}

void *bist_task(void *threadp)
{
    LOG_INFO(BIST_TASK,"Bist task thread spawned\n");
    return NULL;
}