#include "includes.h"
#include "bbgled.h"
#include "mysignal.h"

/*void signal_handler(int signum)
{
    done = 1;
}*/

void signal_init(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = signal_handler;
    sigaction(SIGUSR1, &action, NULL);
}

void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        done = 1;
    }
    
}
