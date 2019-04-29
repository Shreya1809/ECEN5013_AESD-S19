/*
 * spi.h
 *
 *  Created on: Apr 13, 2019
 *      Author: Shreya
 */

#ifndef SPI_H_
#define SPI_H_


void spi_Init(void);

uint8_t spi_ReadByte(void);

void spi_WriteByte(uint8_t data);

void spi_WriteBytes( uint8_t* data, size_t size);

size_t spi_ReadBytes(uint8_t* data, size_t size);

void spi_close(void);


#endif /* SPI_H_ */
