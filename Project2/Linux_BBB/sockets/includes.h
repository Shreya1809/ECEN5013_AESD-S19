#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/select.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h> 
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>
// #include <openssl/md5.h>
#include <time.h>



#define PORT 8888

typedef enum
{ 
    TCP_SEND,
    TCP_ACK, 
    
}tcp_enum_t;

typedef struct{
    uint32_t payloadSize[4];
    uint8_t payload[4][1024];
    double Timestamp;
    uint16_t led_status;
    tcp_enum_t code;
    uint16_t pid;
    uint16_t sock_fd;

} payload_struct_t;

payload_struct_t c_packet;
payload_struct_t s_packet;

//struct payload_struct_t packet;




#endif