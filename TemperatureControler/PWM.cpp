#include "PWM.h"

PWM Digital_Output;

void PWM::Init(){
	RCC->APB2ENR |= 0x01<<3;
	GPIOB->CRL &= 0x00FFFFFF; //PB6 PB7 AFOD
	GPIOB->CRL |= 0xFF000000;
	GPIOB->CRH &= 0xFFFFFF00; //PB8 PB9 AFOD
	GPIOB->CRH |= 0x000000FF;
	
	GPIOA->CRL &= 0x00FFFFFF; //PA6 PA7 AFOD
	GPIOA->CRL |= 0xFF000000; 
	GPIOB->CRL &= 0xFFFFFF00; //PB0 PB1 AFOD
	GPIOB->CRL |= 0x000000FF;
	
	//--------PWM Heater--------//
	RCC->APB1ENR |= 0x01<<2;
  TIM4->ARR = 100;//Ԥװ��
  TIM4->PSC = 7199;//��Ƶ
  TIM4->CR1 |= 0x01<<4; //���¼���
//  TIM4->DIER |= 0x01<<0; 
//  NVIC->IP[30] = 0x10;//���ȼ�1
//  NVIC->ISER[0] |= 0x01<<30;
	TIM4->CCER = 0x00;
  TIM4->CCMR1 = 0x6C6C; //PB7 PB6 PWM
	TIM4->CCMR2 = 0x6C6C; //PB9 PB8 PWM
	TIM4->CCR1 = 0; //PB6ռ�ձ�0
	TIM4->CCR2 = 0; //PB7ռ�ձ�0
	TIM4->CCR3 = 0; //PB8ռ�ձ�0
	TIM4->CCR4 = 0; //PB9ռ�ձ�0
//	TIM4->CCER |= 0x01<<0; //ʹ��ͨ��1 PB6
	TIM4->CCER |= 0x01<<4; //ʹ��ͨ��2 PB7 ExtruderA-Heater
	TIM4->CCER |= 0x01<<8; //ʹ��ͨ��3 PB8 ExtruderB-Heater
	TIM4->CCER |= 0x01<<12; //ʹ��ͨ��4 PB9 BasePlane-Heater

	TIM4->CR1 |= 0x01<<0;//ʹ��
	
	//--------PWM Cooler--------//
	RCC->APB1ENR |= 0x01<<1;
  TIM3->ARR = 100;//Ԥװ��
  TIM3->PSC = 7199;//��Ƶ
  TIM3->CR1 |= 0x01<<4; //���¼���
//  TIM3->DIER|=0x01<<0;
//  NVIC->IP[29]=0x00;//���ȼ�0
//  NVIC->ISER[0]|=0x01<<29;
	TIM3->CCER = 0x00;
	TIM3->CCMR1 = 0x6C6C; //PA7 PA6 PWM
	TIM3->CCMR2 = 0x6C6C; //PB1 PB0 PWM
	TIM3->CCR1 = 0; //PA6ռ�ձ�0
	TIM3->CCR2 = 0; //PA7ռ�ձ�0
	TIM3->CCR3 = 0; //PB0ռ�ձ�0
	TIM3->CCR4 = 0; //PB1ռ�ձ�0
//	TIM3->CCER |= 0x01<<0; //ʹ��ͨ��1 PA6
//	TIM3->CCER |= 0x01<<4; //ʹ��ͨ��2 PA7
	TIM3->CCER |= 0x01<<8; //ʹ��ͨ��3 PB0 ExtruderA-Cooler
	TIM3->CCER |= 0x01<<12; //ʹ��ͨ��4 PB1 ExtruderB-Cooler
	
  TIM3->CR1 |= 0x01<<0;//ʹ��
  
}

void PWM::SetPWMHeaterExtruderA(u32 DC){ //���ü�����A������ռ�ձ�
	if(DC > 100) TIM4->CCR2 = 100;
	else TIM4->CCR2 = DC;
}
void PWM::SetPWMHeaterExtruderB(u32 DC){ //���ü�����B������ռ�ձ�
	if(DC > 100) TIM4->CCR3 = 100;
	else TIM4->CCR3 = DC;
}
void PWM::SetPWMHeaterBasePlane(u32 DC){ //���û�׼�������ռ�ձ�
	if(DC > 100) TIM4->CCR4 = 100;
	else TIM4->CCR4 = DC;
}
void PWM::SetPWMCoolerExtruderA(u32 DC){ //���ü�����A��ȴ��ռ�ձ�
	if(DC > 100) TIM3->CCR3 = 100;
	else TIM3->CCR3 = DC;
}
void PWM::SetPWMCoolerExtruderB(u32 DC){ //���ü�����B��ȴ��ռ�ձ�
	if(DC > 100) TIM3->CCR4 = 100;
	else TIM3->CCR4 = DC;
}

u32 PWM::GetPWMHeaterExtruderA(){ //��ȡ������A������ռ�ձ�
	if(TIM4->CCR2 > 100) return 100;
	else return TIM4->CCR2;
}
u32 PWM::GetPWMHeaterExtruderB(){ //��ȡ������B������ռ�ձ�
	if(TIM4->CCR3 > 100) return 100;
	else return TIM4->CCR3;
}
u32 PWM::GetPWMHeaterBasePlane(){ //��ȡ��׼�������ռ�ձ�
	if(TIM4->CCR4 > 100) return 100;
	else return TIM4->CCR4;
}
u32 PWM::GetPWMCoolerExtruderA(){ //��ȡ������A��ȴ��ռ�ձ�
	if(TIM3->CCR3 > 100) return 100;
	else return TIM3->CCR3;
}
u32 PWM::GetPWMCoolerExtruderB(){ //��ȡ������B��ȴ��ռ�ձ�
	if(TIM3->CCR4 > 100) return 100;
	else return TIM3->CCR4;
}

