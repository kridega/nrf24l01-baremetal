#include "spi.h"


void spi1_gpio_init(){
	// PA5 CLOCK , PA6 MISO , PA7 MOSI , PA9 CS -- AF05
	RCC->AHB1ENR |= 1<<0;
	GPIOA->MODER &= ~(1<<10); GPIOA->MODER |= (1<<11);
	GPIOA->MODER &= ~(1<<12); GPIOA->MODER |= (1<<13);
	GPIOA->MODER &= ~(1<<14); GPIOA->MODER |= (1<<15);
	GPIOA->MODER &= ~(1<<19); GPIOA->MODER |= (1<<18);

	GPIOA->AFR[0] |= (5<<20) | (5<<24) | (5<<28) ;

}

void spi1_config(){
	RCC->APB2ENR |= 1<<12;
	SPI1->CR1 |= 1<<3; // Baud rate 16MHz/4=4Mhz
	// CPOL=0 , CPHA=0 default
	// full duplex default
	// MSB first default
	// 8 bit data mode default
	SPI1->CR1 |= 1<<2; // master configuration
	SPI1->CR1 |= (1<<8) | (1<<9); // SSM=1 SSI=1
	SPI1->CR1 |= 1<<6; // SPI enable

}

void spi1_transmit(uint8_t *data,uint32_t size){
	uint8_t temp;
	uint32_t i=0;

	while(i<size){
		while(!(SPI1->SR & (1<<1))); // wait until TXE is set
		SPI1->DR = data[i];
		i++;
	}

	while((SPI1->SR & (1<<7))); // wait for BUSY flag
	temp=SPI1->DR; temp=SPI1->SR; // Clear OVR flag

}

void spi1_receive(uint8_t *data,uint32_t size){
	while(size--){
		SPI1->DR=0xFF; // dummy data
		while(!(SPI1->SR & (1<<0))); // wait for RXNE flag
		*data++ = SPI1->DR;
	}
}

void cs_low(){
	GPIOA->ODR &= ~(1<<9); // PA9 low
}

void cs_high(){
	GPIOA->ODR |= (1<<9); // PA9 low
}
