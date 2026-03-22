#include "spi.h"
#include "systick.h"
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

char NRF_ReadReg(uint8_t reg){
	uint8_t tx_buf;
	uint8_t value;
	tx_buf= (0x0 | reg);
	cs_low();
	spi1_transmit(&tx_buf, 1);
	spi1_receive(&value, 1);
	cs_high();
	return value;
}

void NRF_init_transmit(){
	uint8_t tx_buf=0; // w_tx_payload

	GPIOA->MODER &= ~(1<<17); GPIOA->MODER |= (1<<16); // PA8 as CE
	ce_low();
	cs_high();

	systickDelayMs_polling(10);

	NRF_WriteReg(0x00,0x0);


	NRF_WriteReg(0x1,0x1); // auto ack data pipe 0
	NRF_WriteReg(0x2,0x1); // enable data pipe 0
	NRF_WriteReg(0x3,0x3); // 5 byte address
	NRF_WriteReg(0x4,0xFF);

	NRF_WriteReg(0x5,76); // channel
	NRF_WriteReg(0x6,0x6); // 1mbps , 0dbm
	NRF_WriteReg(0x11,32); // 32 bytes payload


	cs_low();
	tx_buf = 0xE1; // flush tx
	spi1_transmit(&tx_buf, 1);
	cs_high();

	cs_low();
	tx_buf = 0xE2; // flush rx
	spi1_transmit(&tx_buf, 1);
	cs_high();

	NRF_WriteReg(0x00,0xA); // PWR_UP 0xA for transmit and 0xB for recieving

	char status = NRF_ReadReg(0x07); //debug

	systickDelayMs_polling(10);


	NRF_WriteReg(0x07, 0x70);   // RX_DR | TX_DS | MAX_RT
	status = NRF_ReadReg(0x07); //debug
	status = NRF_ReadReg(0x07); //debug


}

void NRF_init_receive(){
	GPIOA->MODER &= ~(1<<17); GPIOA->MODER |= (1<<16); // PA8 as CE
	ce_low();
	cs_high();

	systickDelayMs_polling(10);
	NRF_WriteReg(0x1,0x1); // auto ack data pipe 0
	NRF_WriteReg(0x2,0x1); // enable data pipe 0
	NRF_WriteReg(0x3,0x3); // 5 byte address
	NRF_WriteReg(0x5,76); // channel
	NRF_WriteReg(0x6,0x6); // 1mbps , 0dbm
	NRF_WriteReg(0x11,32); // 32 bytes payload
	NRF_WriteReg(0x07, 0x30); // clear interrupt
	NRF_WriteReg(0x00,0xB); // PWR on , PRX config
	systickDelayMs_polling(10);

	ce_high();
}



/*void NRF_Transmit(uint8_t *data,uint8_t len){
	uint8_t tx_buf=0;
	ce_low();

	cs_low();
	tx_buf = 0xE1; // flush tx
	spi1_transmit(&tx_buf, 1);
	cs_high();

	cs_low();
	tx_buf=0xA0; // w_tx_payload
	spi1_transmit(&tx_buf, 1);
	spi1_transmit(data, len);
	cs_high();

	ce_high();
	systickDelayMs_polling(10);
	ce_low();

	char status = NRF_ReadReg(0x07);
	while((status)&(1<<5));

	NRF_WriteReg(0x07, 0x30); // clear interrupt
}*/

void NRF_Transmit(uint8_t *data,uint8_t len){

    ce_low();

    NRF_WriteReg(0x07,0x70);   // clear flags

    cs_low();
    uint8_t cmd = 0xA0;
    spi1_transmit(&cmd,1);
    spi1_transmit(data,len);
    cs_high();

    ce_high();
    systickDelayMs_polling(1);   // small pulse
    ce_low();

}


int NRF_Receive(uint8_t *data,uint8_t len){
	uint8_t tx_buf=0;
	uint8_t status = NRF_ReadReg(0x7);

	if(status & (1<6)){
		cs_low();
		tx_buf=0x61; // w_tx_payload
		spi1_transmit(&tx_buf, 1);
		spi1_receive(data, len);
		cs_high();

		NRF_WriteReg(0x07, 0x30); // clear interrupt

		return 1;

	}

	return 0;


}
