/*
 * ethernet.h
 *
 *  Created on: Apr 24, 2019
 *      Author: Shreya
 */

#ifndef ETHERNET_H_
#define ETHERNET_H_

void InitDescriptors(uint32_t ui32Base);
void Ethernet_Init(void);
void EthernetIntHandler(void);
int32_t Ethernet_PacketTransmit(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen);
int32_t Ethernet_PacketReceive(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen);
int32_t Ethernet_FrameLength(void);
#endif /* ETHERNET_H_ */
