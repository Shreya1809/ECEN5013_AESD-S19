/*
 * uart3.h
 *
 *  Created on: Apr 29, 2019
 *      Author: Shreya
 */

#ifndef COMMUART_H_
#define COMMUART_H_

void UART3_config(uint32_t baudrate);
size_t UART3_putData(char *data, size_t len);
size_t UART3_getData(char *data, size_t len);
size_t UART3_putString(const char *str);

void UART2_config(uint32_t baudrate);
int UART2_putData(const char *data, size_t len);
int UART2_getData(char *data, size_t len);
//int32_t UART3CharGet(void);

#endif /* COMMUART_H_ */
