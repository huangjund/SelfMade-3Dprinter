#include "stm32f10x.h"
#include "Card.h"
#include "UART.h"
#include "File.h"
void DelayUs(u32 n);
void DelayMs(u32 n);
bool CardRefreashFlag = 0;

extern"C"void TIM2_IRQHandler(){
	if(TIM2->SR & 0x01){
		CardRefreashFlag = 1;
		TIM2->SR &= ~0x01;
	}
}

void CardRefreash(){
	static u8 PreVolume = 0;
	u8 Volume = Card.GetVolume();
	if(PreVolume != Volume)
		File.FileOpen();
	
	PreVolume = Volume;
	CardRefreashFlag = 0;
}

int main(){
	File.Init();	
	Serial.Init();
	Card.Init();
	DelayMs(10);
	Card.GetStatus();	
	File.FileOpen();
	
	RCC->APB2ENR |= 0x01;
	AFIO->MAPR &= 0xF0FFFFFF;
	AFIO->MAPR |= 0x02000000;
	SCB->AIRCR = 0x05FA0300; //4位抢占，0位响应
	RCC->APB1ENR |= 0x01;
  TIM2->ARR = 9999; //预装载
  TIM2->PSC = 719; //分频
//  TIM2->CR1 |= 0x01<<4; //向下计数
  TIM2->DIER |= 0x01;
  NVIC->IP[28] = 0x40; //优先级4
  NVIC->ISER[0] |= 0x01<<28;
	TIM2->CR1 |= 0x01;
	
	while(1){
		if(Serial.RxFinish)
			Serial.Decoder();		
		if(CardRefreashFlag)
			CardRefreash();
	}
	
}

void DelayUs(u32 n){
	SysTick->LOAD = 72*n;
	SysTick->CTRL = 0x00000005;
	while(!(SysTick->CTRL & 0x00010000));
	SysTick->CTRL = 0x00000004;
}

void DelayMs(u32 n){
	while(n--)
		DelayUs(1000);
}

