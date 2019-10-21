#include "stm32f10x.h"
#include "SPIO.h"
#include "COM.h"
int main(){
	RCC->APB2ENR |= 0x01;
  AFIO->MAPR &= 0xF0FFFFFF;
  AFIO->MAPR |= 0x02000000;
	
	RCC->APB2ENR |= 0x01<<4;
//	GPIOC->CRH &= 0x00FFFFFF;
//	GPIOC->CRH |= 0x88000000;
//	GPIOC->BSRR |= 0x01<<14;
//	GPIOC->BSRR |= 0x01<<15;
	
	Serial.Init();
	Spio.Init();
	
	while(1){
		Spio.ReadIn();
	}
}

