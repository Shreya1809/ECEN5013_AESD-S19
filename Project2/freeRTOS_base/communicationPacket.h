/*
 * communicationPacket.h
 *
 *  Created on: Apr 28, 2019
 *      Author: Shreya
 */

#ifndef COMMUNICATIONPACKET_H_
#define COMMUNICATIONPACKET_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "logger.h"
#include "nodeInformation.h"
#include "sensorData.h"

typedef enum opcode{
    NodeInfo = 0,
    Heartbeat,
    temperature,
    distance,
    accelerometer,
    reverseGearStateControl,
    fanStateControl,
    accelerometerDeltaThresholdControl,
}opcode_t;

typedef enum srcnode{
    BBG = 0,
    BBB,
    EK_TM4C1294XL= 10,
    EK_TM4C123GXL,
    EK_TM4C129EXL,
    DK_TM4C129X,
    DK_TM4C123G
}node_t;

typedef node_t srcnode_t;
typedef node_t dstnode_t;

typedef enum node_type{
    CONTROL_NODE = 0,
    REMOTE_NODE,
}node_type_t;

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
        bool inReverseGear;
        bool isFanOn;
    };
};
typedef struct packetData packetData_t;

struct packet{
    packetHeader_t header;
    packetData_t data;
    uint16_t crc;
};
typedef struct packet packet_struct_t;


//extern char *infostartframe;
//extern char *infoendframe;

uint16_t CRC_calculate(uint8_t * data_p, uint8_t length);

static inline void FillPacketHeader(packet_struct_t *commpacket)
{
    commpacket->header.timestamp = getTimeMsec();
    commpacket->header.node_state = getThisNodeCurrentOperation();
    commpacket->header.src_node = EK_TM4C1294XL;
    commpacket->header.dst_node = BBG;
    commpacket->header.node = REMOTE_NODE;
}

static inline void FillPacketBBGHeader(packet_struct_t *commpacket)
{
    commpacket->header.timestamp = getTimeMsec();
    commpacket->header.node_state = getThisNodeCurrentOperation();
    commpacket->header.src_node = BBG;
    commpacket->header.dst_node = EK_TM4C1294XL;
    commpacket->header.node = CONTROL_NODE;
}

static inline void FillCRC(packet_struct_t *commpacket)
{
    commpacket->crc = 0;
    commpacket->crc = CRC_calculate((uint8_t*)commpacket, sizeof(*commpacket));
}

static inline bool VerifyCRC(packet_struct_t *commpacket)
{
    uint16_t crc = CRC_calculate((uint8_t*)commpacket, sizeof(*commpacket));
    return crc == commpacket->crc;
}


#endif /* COMMUNICATIONPACKET_H_ */
