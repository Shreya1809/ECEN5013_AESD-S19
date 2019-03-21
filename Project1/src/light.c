#include "includes.h"
#include "light.h"
#include "main.h"
#include "logger.h"
#include "bbgled.h"


float getLight(void)
{
    float i = 100.0;
    return i;
}

void *light_task(void *threadp)
{
    LOG_INFO(LIGHT_TASK,"Light task thread spawned");
    return NULL;
}