#include "SPIN.h"

SPIN Spin;


extern"C"void USART2_IRQHandler(void){
	if(USART2->SR & 0x20){
		u8 Temp = USART2->DR;
		if(Spin.RxEnableFlag) //允许接收
			Spin.Load(Temp); //装入缓存
		
	}
}

void SPIN::Init(){
	for(u32 i=0;i<64;i++)
		Buffer[i] = 0;
	
	LoadPtr = 0;
	LauncherPtr = 0;
	DataNumber = 0;
	
	RxEnableFlag = 1;
	//RX-PA3 TX-PA2
  RCC->APB2ENR |= 0x01<<2;
  RCC->APB1ENR |= 0x01<<17;
  GPIOA->CRL &= 0xFFFF00FF;
  GPIOA->CRL |= 0x00004B00;
  USART2->CR2 = 0x0000;
  USART2->CR3 = 0x0000;
  USART2->GTPR = 0x0000;
  USART2->BRR = 36000000/375000; //波特率375K
  USART2->CR1 = 0x01<<13;//外设使能
  USART2->CR1 |= 0x01<<5; //接收中断
//  USART2->CR1 |= 0x01<<7; //发送中断
  NVIC->IP[38] = 0x10;
  NVIC->ISER[1] |= 0x01<<6; //开放中断线
  USART2->CR1 |= 0x01<<2; //接收使能
  USART2->CR1 |= 0x01<<3; //发送使能

}

void SPIN::EnableRx(){
	for(u32 i=0;i<64;i++)
		Buffer[i] = 0;
	
	LoadPtr = 0;
	LauncherPtr = 0;
	DataNumber = 0;
	
	RxEnableFlag = 1;
}

void SPIN::DisableRx(){
	RxEnableFlag = 0;
	
	for(u32 i=0;i<64;i++)
		Buffer[i] = 0;
	
	LoadPtr = 0;
	LauncherPtr = 0;
	DataNumber = 0;
}

void SPIN::Load(u8 Data){
	if(LoadPtr==LauncherPtr && DataNumber) //缓存满
		return;
	
	Buffer[LoadPtr] = Data; //装入缓存
	
	DataNumber++; //调整数据量
	
	LoadPtr++; //移动指针
	LoadPtr %= 64; //循环
}

u8 SPIN::Launch(){
	if(!DataNumber) //缓存空
		return 0;
	
	u8 Temp = Buffer[LauncherPtr]; //暂存输出数据
	
	Buffer[LauncherPtr] = 0; //重置缓存相应单元
	
	DataNumber--; //调整数据量
	
	LauncherPtr++; //移动指针
	LauncherPtr %= 64; //循环
	
	return Temp;
}
