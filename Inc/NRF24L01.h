/*
 * NRF24L01.h
 *
 *  Created on: Jan 30, 2026
 *      Author: parth
 */

#ifndef NRF24L01_H_
#define NRF24L01_H_
#include "stm32f446xx.h"
#include "stdint.h"

void NRF_Transmit(uint8_t *data,uint8_t len);
int NRF_Receive(uint8_t *data,uint8_t len);
void NRF_init_transmit();
void NRF_init_receive();
#endif /* NRF24L01_H_ */
