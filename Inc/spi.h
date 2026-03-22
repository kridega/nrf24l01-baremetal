/*
 * spi.h
 *
 *  Created on: Jan 29, 2026
 *      Author: parth
 */

#ifndef SPI_H_
#define SPI_H_
#include "stm32f446xx.h"
#include "stdint.h"


void spi1_gpio_init();
void spi1_config();
void spi1_transmit(uint8_t *data,uint32_t size);
void spi1_receive(uint8_t *data,uint32_t size);
void cs_low();
void cs_high();
#endif /* SPI_H_ */
