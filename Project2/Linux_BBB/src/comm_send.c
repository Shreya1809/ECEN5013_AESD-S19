
#include "includes.h"
#include "comm_send.h"
#include "socket.h"
#include "logger.h"
#include "bbgled.h"
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
extern volatile int remoteNodeDisconnected;
int commSendTask_enqueue(opcode_t opcode, void *data)
{
  if (mq_sendtask > -1 )//&& level > LOG_INVALID && level < LOG_MAX)
  {
    packet_struct_t send_packet = {0};
    send_packet.data.opcode = opcode;
    send_packet.data.dataSize = 0;
    LOG_INFO(SEND_TASK, "Sending MSG: Opcode:%d to Node:%d",send_packet.data.opcode, send_packet.header.dst_node);
    if(data) //output oppcodes
    {
        switch(opcode){
          case Heartbeat:
            remoteNodeDisconnected = 0;
            LOG_DEBUG(SEND_TASK, "Heartbeat sent");
          break;
          case accelerometerDeltaThresholdControl:
              send_packet.data.accel = *(accel_data_t*)data;
              send_packet.data.dataSize = sizeof(accel_data_t);
          break;
          case temperatureThresholdControl:
              send_packet.data.temperature = *(temp_data_t*)data;
              send_packet.data.dataSize = sizeof(temp_data_t);
          break;
          case reverseGearStateControl:
              send_packet.data.inReverseGear = *(bool*)data;
              send_packet.data.dataSize = sizeof(bool);
          break;
          case fanStateControl:
              send_packet.data.isFanOn = *(bool*)data;
              send_packet.data.dataSize = sizeof(bool);
          break;
          default:
            LOG_ERROR(SEND_TASK, NULL, "Invalid Send COMM opcode");
            return 1; 
        }

    }
    FillPacketBBGHeader(&send_packet);
    int ret = mq_send(mq_sendtask, (const char *)&send_packet, sizeof(send_packet), 0);
    if (ret)
      return ret;

    return 0;
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

static inline int SendToRemoteNode(packet_struct_t *send_comm_packet, int fd)
{
    //send frame
    FillCRC(send_comm_packet);
    int ret = COMM_PHYSEND(fd , (char*)send_comm_packet , sizeof(*send_comm_packet));
    return ret;
}

void *commSend_task(void *pnewsock)
{
    
    int valread = 0;
    unsigned int prio;
    packet_struct_t comm_packet = {0};
    int fd = 0;
    if(pnewsock == NULL){
        perror("Invalid thread param"); 
        exit(EXIT_FAILURE); 
    }
    sendtask_queue_init();
    fd = *(int*)pnewsock; 
    struct timespec packet_timeout = {0};
    int lastError = 0;
    clock_gettime(CLOCK_REALTIME, &packet_timeout);
    packet_timeout.tv_sec = 1;   
    LOG_INFO(SEND_TASK,"fd: %d , pthread id: %lu", fd, pthread_self());
    while(!stop_send_thread) //outer loop for just ther start frames
    {
        memset(&comm_packet, 0, sizeof(comm_packet));
        if (mq_timedreceive(mq_sendtask, (char*)&comm_packet, sizeof(comm_packet), &prio, &packet_timeout) == -1) {
            if(!stop_send_thread && errno == ETIMEDOUT)
            {
              continue;
            }
            else if(stop_send_thread)
            {
              LOG_DEBUG(SEND_TASK, "Send task Timeout: Stop logger:%d, lastErr: %d",stop_send_thread, lastError);
              lastError++;
              //required sleep here to yield as the lastError check till < 5 
              //takes less than the quantum given to this thread by the scheduler.
              //Having this sleep here allows other thread to run and make 
              //sure to have all the logs from other threads
              usleep(10000);
              continue;
            }
            else
            {
              perror("Client Comm send: mq_timedreceive");
            } 
        }
        else
        {
            SendToRemoteNode(&comm_packet,fd);
        }
         
    }
    
    return (void*)0;

}

void kill_send_thread(void)
{
    LOG_DEBUG(SEND_TASK,"Send thread exit signal received");
    stop_send_thread = 1;
    pthread_cancel(threads[SEND_TASK]);    
} 