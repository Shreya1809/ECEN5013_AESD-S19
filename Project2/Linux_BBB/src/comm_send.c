
#include "includes.h"
#include "comm_send.h"
#include "socket.h"
#include "logger.h"
#include "bbgled.h"
#include "mysignal.h"
#include "heartbeat.h"
#include "uart.h"

static sig_atomic_t stop_send_thread = 0;
static mqd_t mq_sendtask = -1;

#define SEND_QUEUE_NAME         "/com_send_q"
#define QUEUE_PERMISSIONS   0666
#define MAX_MESSAGES        128
#define MAX_MSG_SIZE        256
#define CURRENT_MSG         0
#define FLAGS               0

mqd_t sendtask_queue_init(void)
{
    struct mq_attr atb;

    atb.mq_flags = FLAGS;
    atb.mq_maxmsg = MAX_MESSAGES;
    atb.mq_msgsize = sizeof(packet_struct_t);
    atb.mq_curmsgs = CURRENT_MSG;

    mq_unlink(SEND_QUEUE_NAME);
    if ((mq_sendtask = mq_open(SEND_QUEUE_NAME, O_CREAT | O_RDWR, QUEUE_PERMISSIONS, &atb)) == -1)
    {
    perror("Client: mq_open (client)");
    exit(1);
    }

    return mq_sendtask;
}

int commSendTask_enqueue(opcode_t opcode, void *data)
{
  if (mq_sendtask > -1 )//&& level > LOG_INVALID && level < LOG_MAX)
  {
    packet_struct_t send_packet;
    send_packet.header.dst_node = EK_TM4C1294XL;
    send_packet.header.src_node = BBB;
    send_packet.header.timestamp = getCurrentTimeMsec();
    send_packet.header.node_state = 0;//srcModuleID = modId;
    send_packet.header.node = CONTROL_NODE;
    if(data) //output oppcodes
    {
        // switch(opcode)
        // case acceldelta:
        //     send_packet.data.opcode = opcode;
        //     send_packet.data.accel = *(accel_data_t*)data;
        //     send_packet.data.dataSize = sizeof(accel_data_t);
        // break;
        // case tempthreshold:
        //     send_packet.data.opcode = opcode;
        //     send_packet.data.temperature = *(temp_data_t*)data;
        //     send_packet.data.dataSize = sizeof(temp_data_t);
        // break;
        // case reversegear:
        //     send_packet.data.opcode = opcode;
        //     send_packet.data.dist = *(dist_data_t*)data;
        //     send_packet.data.dataSize = sizeof(dist_data_t);
        // break;
        // default:
        // break;
        

    }
    
    int ret = mq_send(mq_sendtask, (const char *)&send_packet, sizeof(send_packet), 0);
    if (ret)
      return ret;

    return EXIT_SUCCESS;
  }

  LOG_ERROR(LOGGER_TASK,"Log enqueue Error");
  GREENLEDOFF();
  REDLEDON();
  return EXIT_FAILURE;
}
#ifdef TCP
#define COMM_PHYSEND(socket,data,len)   send(socket,data,len,0)
#else
#define COMM_PHYSEND(fd,data,len)       UART_write(fd,data,len)
#endif
void *commSend_task(void *pnewsock)
{
    
    int valread = 0;
    unsigned int prio;
    packet_struct_t comm_packet;
    int fd = 0;
    #ifdef TCP
    if(pnewsock == NULL){
        perror("Invalid socket thread param"); 
        exit(EXIT_FAILURE); 
    }
    fd = *(int*)pnewsock;
    #endif
    char startFrame[5] = {0};
    char buffer[1024] = {0}; 
    struct timespec packet_timeout = {0};
    int lastError = 0;
    clock_gettime(CLOCK_REALTIME, &packet_timeout);
    packet_timeout.tv_sec = 1;   
    LOG_INFO(SEND_TASK,"fd: %d , pthread id: %lu", fd, pthread_self());
    memset(startFrame, 0, sizeof(startFrame));
    while(!stop_send_thread) //outer loop for just ther start frames
    {
        
        // if (mq_timedreceive(mq_sendtask, (char*)&comm_packet, sizeof(comm_packet), &prio, &packet_timeout) == -1) {
        //     if(!stop_send_thread && errno == ETIMEDOUT)
        //     {
        //     continue;
        //     }
        //     else if(stop_send_thread)
        //     {
        //     LOG_DEBUG(SEND_TASK, "Send task Timeout: Stop logger:%d, lastErr: %d",stop_send_thread, lastError);
        //     lastError++;
        //     //required sleep here to yield as the lastError check till < 5 
        //     //takes less than the quantum given to this thread by the scheduler.
        //     //Having this sleep here allows other thread to run and make 
        //     //sure to have all the logs from other threads
        //     usleep(10000);
        //     continue;
        //     }
        //     else
        //     {
        //     perror ("Client: mq_receive");
        //     } 
        // }
        // else
        // {
        //     //TaskSendToRemoteNode(comm_packet,clientsock);
        // }
         
    }
    
    return (void*)0;

}

int TaskSendToRemoteNode(opcode_t opcode,packet_struct_t *send_comm_packet, int fd)
{
    char startFrame[5] = "*#*#";
    char endFrame[5] = "#*#*";
    size_t frameSize = sizeof(packet_struct_t);

    //send start frame 
    COMM_PHYSEND(fd , startFrame , sizeof(startFrame));

    //send opcode 
    COMM_PHYSEND(fd , (char*)&opcode , sizeof(opcode));

    //send size of frame
    COMM_PHYSEND(fd , (char*)&frameSize , sizeof(size_t));

    //send frame
    COMM_PHYSEND(fd , (char*)send_comm_packet , frameSize);

    //send end frame
    COMM_PHYSEND(fd , startFrame , sizeof(startFrame));
    return 0;
}
void kill_send_thread(void)
{

} 