
#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdint.h>
#include <stdio.h>

#include "sensorData.h"
#include "nodeInformation.h"
#include "communicationPacket.h"

dist_data_t distdata;
temp_data_t tempdata;
accel_data_t acceldata;

void timelimit(int sockfd, int io);
void Process_TemperatureData(packet_struct_t packet);
void Process_UltrasonicData(packet_struct_t packet);
void Process_AccelerationData(packet_struct_t packet);

#endif