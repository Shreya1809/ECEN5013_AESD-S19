/**
 * @file uart.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-29
 * 
 * @copyright Copyright (c) 2019
 * 
 */
// #include "includes.h"
#include <stdio.h>
#include <stdint.h>
#include "logger.h"
#include "bbgled.h"
#include "uart.h"
#include <termios.h>

static struct termios prev_config;
struct termios new_config;

int UART_config(void)
{
    int fd;
    char buffer[255];
    fd = open("/dev/ttyS4", O_RDWR | O_NOCTTY | O_SYNC );
    if (fd < 0) 
    { 
        perror("/dev/ttyS4"); 
        return -1;
    }

    tcgetattr(fd, &prev_config);
    bzero(&new_config, sizeof(new_config)); 

    new_config.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    cfmakeraw(&new_config);
    new_config.c_cc[VMIN]  = 0;
    new_config.c_cc[VTIME] = 1;

    tcflush(fd, TCIFLUSH);
    if(tcsetattr(fd, TCSAFLUSH, &new_config))
    {
        perror("Set Attr");
    }
    return fd;
}

void UART_close(int fd)
{
    tcsetattr(fd, TCSANOW, &prev_config);
    close(fd);
}

void UART_flush(int fd)
{
    (fd > 0) ? tcflush(fd, TCIFLUSH): 0;
}

int UART_printChar(int fd,char c)
{
    return write(fd, &c, 1);    
}

int UART_write(int fd, char *data, size_t size)
{
    if(fd < 0)
        return -1;

    int32_t val = 0, count = 0, i =0;
    while(i < size)
    {
        val = write(fd, data+i, size-i);
        if(val < 0)
        {
            break;    
        }
        else
        {
            i += val;
            count++;
        }
    }
    return i;
}


int UART_read(int fd, char *data, size_t size)
{
    if(fd < 0)
        return -1;

    int val = 0, count = 0, i =0;
    while(i < size)
    {
        val = read(fd, data+i, size-i);
        if(val > 0)
        {
            i += val;
            count++;
        }
    }
    return i;
}

#ifdef UART_TEST

int main()
{
    int fd = 0,val = 0;
    fd = UART_config();
    if(fd < 0)
    {
        printf("UART Config error\n");
        return 1;

    }
    else
    {
        printf("UART config OK : %d\n",fd);
    }
    
    char buffer[16] ="123456789012345";
    char rbuffer[20] = {0};
    val = UART_write(fd, buffer, strlen(buffer));
    perror("Write");
    printf("UART written:%d\n",val);
    val = UART_read(fd, rbuffer, strlen(buffer));
    perror("Read");
    printf("UART read:%d\n",val);
    return 0;
}

#endif
