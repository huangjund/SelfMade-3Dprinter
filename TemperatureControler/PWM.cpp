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
  TIM4->ARR = 100;//预装载
  TIM4->PSC = 7199;//分频
  TIM4->CR1 |= 0x01<<4; //向下计数
//  TIM4->DIER |= 0x01<<0; 
//  NVIC->IP[30] = 0x10;//优先级1
//  NVIC->ISER[0] |= 0x01<<30;
	TIM4->CCER = 0x00;
  TIM4->CCMR1 = 0x6C6C; //PB7 PB6 PWM
	TIM4->CCMR2 = 0x6C6C; //PB9 PB8 PWM
	TIM4->CCR1 = 0; //PB6占空比0
	TIM4->CCR2 = 0; //PB7占空比0
	TIM4->CCR3 = 0; //PB8占空比0
	TIM4->CCR4 = 0; //PB9占空比0
//	TIM4->CCER |= 0x01<<0; //使能通道1 PB6
	TIM4->CCER |= 0x01<<4; //使能通道2 PB7 ExtruderA-Heater
	TIM4->CCER |= 0x01<<8; //使能通道3 PB8 ExtruderB-Heater
	TIM4->CCER |= 0x01<<12; //使能通道4 PB9 BasePlane-Heater

	TIM4->CR1 |= 0x01<<0;//使能
	
	//--------PWM Cooler--------//
	RCC->APB1ENR |= 0x01<<1;
  TIM3->ARR = 100;//预装载
  TIM3->PSC = 7199;//分频
  TIM3->CR1 |= 0x01<<4; //向下计数
//  TIM3->DIER|=0x01<<0;
//  NVIC->IP[29]=0x00;//优先级0
//  NVIC->ISER[0]|=0x01<<29;
	TIM3->CCER = 0x00;
	TIM3->CCMR1 = 0x6C6C; //PA7 PA6 PWM
	TIM3->CCMR2 = 0x6C6C; //PB1 PB0 PWM
	TIM3->CCR1 = 0; //PA6占空比0
	TIM3->CCR2 = 0; //PA7占空比0
	TIM3->CCR3 = 0; //PB0占空比0
	TIM3->CCR4 = 0; //PB1占空比0
//	TIM3->CCER |= 0x01<<0; //使能通道1 PA6
//	TIM3->CCER |= 0x01<<4; //使能通道2 PA7
	TIM3->CCER |= 0x01<<8; //使能通道3 PB0 ExtruderA-Cooler
	TIM3->CCER |= 0x01<<12; //使能通道4 PB1 ExtruderB-Cooler
	
  TIM3->CR1 |= 0x01<<0;//使能
  
}

void PWM::SetPWMHeaterExtruderA(u32 DC){ //设置挤出机A加热器占空比
	if(DC > 100) TIM4->CCR2 = 100;
	else TIM4->CCR2 = DC;
}
void PWM::SetPWMHeaterExtruderB(u32 DC){ //设置挤出机B加热器占空比
	if(DC > 100) TIM4->CCR3 = 100;
	else TIM4->CCR3 = DC;
}
void PWM::SetPWMHeaterBasePlane(u32 DC){ //设置基准面加热器占空比
	if(DC > 100) TIM4->CCR4 = 100;
	else TIM4->CCR4 = DC;
}
void PWM::SetPWMCoolerExtruderA(u32 DC){ //设置挤出机A冷却器占空比
	if(DC > 100) TIM3->CCR3 = 100;
	else TIM3->CCR3 = DC;
}
void PWM::SetPWMCoolerExtruderB(u32 DC){ //设置挤出机B冷却器占空比
	if(DC > 100) TIM3->CCR4 = 100;
	else TIM3->CCR4 = DC;
}

u32 PWM::GetPWMHeaterExtruderA(){ //读取挤出机A加热器占空比
	if(TIM4->CCR2 > 100) return 100;
	else return TIM4->CCR2;
}
u32 PWM::GetPWMHeaterExtruderB(){ //读取挤出机B加热器占空比
	if(TIM4->CCR3 > 100) return 100;
	else return TIM4->CCR3;
}
u32 PWM::GetPWMHeaterBasePlane(){ //读取基准面加热器占空比
	if(TIM4->CCR4 > 100) return 100;
	else return TIM4->CCR4;
}
u32 PWM::GetPWMCoolerExtruderA(){ //读取挤出机A冷却器占空比
	if(TIM3->CCR3 > 100) return 100;
	else return TIM3->CCR3;
}
u32 PWM::GetPWMCoolerExtruderB(){ //读取挤出机B冷却器占空比
	if(TIM3->CCR4 > 100) return 100;
	else return TIM3->CCR4;
}

