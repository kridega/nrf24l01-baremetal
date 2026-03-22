#include "stm32f446xx.h"
#include "i2c.h"

#define CR1_START 1<<8;
#define CR1_ACK 1<<10;
#define CR1_STOP 1<<9;

// PB8---SCL , PB9--SDA
void i2c_init(void){
	// clock access to GPIOB
	RCC->AHB1ENR|=1<<1;
	// clock access I2C
	RCC->APB1ENR|=1<<21;
	// configure PB8,PB9 to alternate function mode
	GPIOB->MODER |=  (1<<17) | (1<<19);
	// pins to AF04
	GPIOB->AFR[1] |= (1<<2) | (1<<6);
	// pull up
	GPIOB->PUPDR |=(1<<16) | (1<<18);
	// open drain
	GPIOB->OTYPER |= (1<<8) | (1<<9);


	// Enter reset mode
	I2C1->CR1|=1<<15;
	// Come out of reset mode
	I2C1->CR1 &= ~(1<<15);
	// set peripheral clock freq
	I2C1->CR2|=1<<4;
	// set standard mode
	I2C1->CCR = 80; // scl=pclk/(2*CCR) for 1:1 duty , for 100khz scl freq => CCR=pclk/(2*scl)=16Mhz/(2*100Khz)=80
	// set TRISE
	I2C1->TRISE = 17; // TRISE=(max_rise_time*pclk)+1 , for standard mode max_rise_time=1000ns , pclk=16Mhx => Trise=17


	// enable i2c
	I2C1->CR1|=1<<0;

}

void I2C1_byteRead(char saddr,char maddr,char* data){
	volatile int tmp;
	while(I2C1->SR2 & 1<<1); // wait until bus not busy
	I2C1->CR1 |= CR1_START; // generate start
	while(!(I2C1->SR1 & 1<<0)); // wait until start flag is set
	I2C1->DR = saddr<<1; // transmit slave addr + write
	while(!(I2C1->SR1 & 1<<1)); // wait until addr flag is set
	tmp=I2C1->SR2; // this is done to clear addr flag
	I2C1->DR=maddr; // send memory address
	while(!(I2C1->SR1 & 1<<7)); // wait until DR register is empty . transmitter empty (TxE flag)
	I2C1->CR1 |= CR1_START; // generate restart
	while(!(I2C1->SR1 & 1<<0)); // wait until start flag is set
	I2C1->DR = saddr<<1 |  1; // transmit slave addr + read
	while(!(I2C1->SR1 & 1<<1)); // wait until addr flag is set
	I2C1->CR1 &= ~CR1_ACK; // disable ACK
	tmp=I2C1->SR2; // this is done to clear addr flag
	I2C1->CR1|=CR1_STOP; //generate stop after data received
	while(!(I2C1->SR1 & 1<<6)); // wait until RxNE flag is set

	*data++ = I2C1->DR;

}

void I2C1_burstRead(char saddr,char maddr,int n,char* data){
	volatile int tmp;
	while(I2C1->SR2 & 1<<1); // wait until bus not busy
	I2C1->CR1 |= CR1_START; // generate start
	while(!(I2C1->SR1 & 1<<0)); // wait until start flag is set
	I2C1->DR = saddr<<1; // transmit slave addr + write
	while(!(I2C1->SR1 & 1<<1)); // wait until addr flag is set
	tmp=I2C1->SR2; // this is done to clear addr flag
	while(!(I2C1->SR1 & 1<<7)); // wait until DR register is empty . transmitter empty (TxE flag)
	I2C1->DR=maddr; // send memory address
	while(!(I2C1->SR1 & 1<<7)); // wait until DR register is empty . transmitter empty (TxE flag)
	I2C1->CR1 |= CR1_START; // generate restart
	while(!(I2C1->SR1 & 1<<0)); // wait until start flag is set
	I2C1->DR = saddr<<1 |  1; // transmit slave addr + read
	while(!(I2C1->SR1 & 1<<1)); // wait until addr flag is set
	tmp=I2C1->SR2; // this is done to clear addr flag
	I2C1->CR1 |= CR1_ACK; // enable ACK

	while(n > 0U){
		if(n==1){
			I2C1->CR1 &= ~CR1_ACK; // disable ACK
			I2C1->CR1|=CR1_STOP; //generate stop
			while(!(I2C1->SR1 & 1<<6)); // wait until RxNE flag is set
			*data++ = I2C1->DR;
			break;
		}
		else{
			while(!(I2C1->SR1 & 1<<6)); // wait until RxNE flag is set
			*data++ = I2C1->DR;
			n--;

		}
	}

}


void I2C1_burstWrite(char saddr,char maddr,int n,char* data){
	volatile int tmp;
	while(I2C1->SR2 & 1<<1); // wait until bus not busy
	I2C1->CR1 |= CR1_START; // generate start
	while(!(I2C1->SR1 & 1<<0)); // wait until start flag is set
	I2C1->DR = saddr<<1; // transmit slave addr + write
	while(!(I2C1->SR1 & 1<<1)); // wait until addr flag is set
	tmp=I2C1->SR2; // this is done to clear addr flag
	while(!(I2C1->SR1 & 1<<7)); // wait until DR register is empty . transmitter empty (TxE flag)
	I2C1->DR=maddr; // send memory address

	for(int i=0;i<n;i++){
		while(!(I2C1->SR1 & 1<<7)); // wait until DR register is empty . transmitter empty (TxE flag)
		I2C1->DR = *data++ ; // transmit data
	}
	while(!(I2C1->SR1 & 1<<2)); // wait until transfer finished
}
