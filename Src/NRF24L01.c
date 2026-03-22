#include "spi.h"
#include "NRF24L01.h"

#define ce_low() (GPIOA->BSRR=(1<<24))
#define ce_high() (GPIOA->BSRR=(1<<8))

// PA8 as CE

void NRF_WriteReg(uint8_t reg,uint8_t value){
	uint8_t tx_buf[2];
	tx_buf[0]= (0x20 | reg);
	tx_buf[1]=value;
	cs_low();
	spi1_transmit(tx_buf, 2);
	cs_high();
}

void NRF_ReadReg(uint8_t reg){
	uint8_t tx_buf;
	uint8_t value;
	tx_buf= (0x0 | reg);
	cs_low();
	spi1_transmit(tx_buf, 1);
	spi1_receive(value, 1);
	cs_high();
}

void NRF_init(){
	GPIOA->MODER &= ~(1<<17); GPIOA->MODER |= (1<<16); // PA8 as CE
	ce_low();
	cs_high();
	NRF_WriteReg(0x00,0xA); // PWR_UP
	NRF_WriteReg(0x1,0x1); // auto ack data pipe 0
	NRF_WriteReg(0x2,0x1); // enable data pipe 0
	NRF_WriteReg(0x3,0x3); // 5 byte address
	NRF_WriteReg(0x4,0x2F);
	NRF_WriteReg(0x5,76); // channel
	NRF_WriteReg(0x6,0x6); // 1mbps , 0dbm
	NRF_WriteReg(0x11,32); // 32 bytes payload
	ce_high();

}

void NRF_Transmit(uint8_t *data,uint8_t len){
	uint8_t tx_buf=0xA0; // w_tx_payload
	ce_low();
	cs_low();
	spi1_transmit(tx_buf, 1);
	spi1_transmit(data, len);
	cs_high();
	ce_high();
}

void NRF_Receive(uint8_t *data,uint8_t len){
	uint8_t tx_buf=0x61; // r_rx_payload
	cs_low();
	spi1_transmit(tx_buf, 1);
	spi1_receive(data, len);
	cs_high();

}
