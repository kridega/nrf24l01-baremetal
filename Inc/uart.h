/*
 * uart.h
 *
 *  Created on: Jan 10, 2026
 *      Author: parth
 */

#ifndef UART_H_
#define UART_H_
#include <stdint.h>
#include "stm32f446xx.h"

void uart2_rxtx_init(void);
char uart2_read(void);
void dma1_stream6_init(uint32_t src,uint32_t dst,uint32_t len);

#endif /* UART_H_ */
