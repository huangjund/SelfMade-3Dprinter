#include "SPIN.h"

SPIN Spin;


extern"C"void USART2_IRQHandler(void){
	if(USART2->SR & 0x20){
		u8 Temp = USART2->DR;
		if(Spin.RxEnableFlag) //�������
			Spin.Load(Temp); //װ�뻺��
		
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
  USART2->BRR = 36000000/375000; //������375K
  USART2->CR1 = 0x01<<13;//����ʹ��
  USART2->CR1 |= 0x01<<5; //�����ж�
//  USART2->CR1 |= 0x01<<7; //�����ж�
  NVIC->IP[38] = 0x10;
  NVIC->ISER[1] |= 0x01<<6; //�����ж���
  USART2->CR1 |= 0x01<<2; //����ʹ��
  USART2->CR1 |= 0x01<<3; //����ʹ��

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
	if(LoadPtr==LauncherPtr && DataNumber) //������
		return;
	
	Buffer[LoadPtr] = Data; //װ�뻺��
	
	DataNumber++; //����������
	
	LoadPtr++; //�ƶ�ָ��
	LoadPtr %= 64; //ѭ��
}

u8 SPIN::Launch(){
	if(!DataNumber) //�����
		return 0;
	
	u8 Temp = Buffer[LauncherPtr]; //�ݴ��������
	
	Buffer[LauncherPtr] = 0; //���û�����Ӧ��Ԫ
	
	DataNumber--; //����������
	
	LauncherPtr++; //�ƶ�ָ��
	LauncherPtr %= 64; //ѭ��
	
	return Temp;
}
