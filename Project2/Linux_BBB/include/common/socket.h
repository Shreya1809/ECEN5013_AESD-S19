
#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdint.h>
#include <stdio.h>

typedef enum {
    STARTUP,
    NORMAL,
    DEGRADED_OPERATION,
    OUTOFSERVICE
}node_operation_state_t;

typedef enum{
    NodeInfo,
    Heartbeat,
    temperature,
    distance,
    accelerometer
}opcode_t;

typedef enum{
    BBG = 0,
    BBB,
    EK_TM4C1294XL = 10,
}nodeID_t;

typedef nodeID_t dstnode_t;
typedef nodeID_t srcnode_t;

typedef enum{
    CONTROL_NODE,
    REMOTE_NODE,
}node_type_t;

typedef struct temp{
    union{
        struct {
            int16_t mantissa;
            uint16_t exponent;
        }fixedpoint;
        float floatingpoint;
    };
    bool connected;
    char unit;
}temp_data_t;

typedef struct accel{

    uint8_t devId;
    int16_t x;
    int16_t y;
    int16_t z;
    bool connected;
    bool INT2;

}accel_data_t;

typedef struct dist{
    int32_t distance;
    bool connected;
}dist_data_t;


enum nodeInfo{
    MCUtype = 0,
    SOCvendor,
    SOCtype,
    OStype,
    OSversion,
    SWversion,
    components,
    communicationType,
    communicationDetails,
    NODE_INFO_MAX
}nodeInfo_t;

struct packetHeader{
    node_operation_state_t node_state;
    uint32_t timestamp;
    srcnode_t src_node;
    dstnode_t dst_node;
    node_type_t node;
};
typedef struct packetHeader packetHeader_t;

struct packetData{
    opcode_t opcode;
    size_t dataSize;
    union{
        temp_data_t temperature;
        accel_data_t accel;
        dist_data_t dist;
        char *nodeInfo;
    };
};
typedef struct packetData packetData_t;

struct packet{
    packetHeader_t header;
    packetData_t data;
    uint16_t crc;
};
typedef struct packet packet_struct_t;

dist_data_t distdata;
temp_data_t tempdata;
accel_data_t acceldata;

void timelimit(int sockfd, int io);
void Process_TemperatureData(packet_struct_t packet);
void Process_UltrasonicData(packet_struct_t packet);
void Process_AccelerationData(packet_struct_t packet);

#endif