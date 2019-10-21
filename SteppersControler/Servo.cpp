#include "Servo.h"
#include "Interface.h"

#define PWM_ENABLE 60
#define PWM_DISABLE 150

SERVO Servo;

extern"C"void TIM4_IRQHandler(void){
	if(TIM4->SR&0x01){
		Servo.Counter++;
		if(Servo.Counter == 1999){
			Servo.Counter = 0;
			AUX_ENABLE(1);
		}
		if(Servo.Counter == Servo.PWM){
			AUX_ENABLE(0);		
		}
		TIM4->SR&=~0x01;
	}
}

void SERVO::Init(){
	Counter = 0;
	PWM = PWM_DISABLE;
	RCC->APB1ENR|=0x01<<2;
  TIM4->ARR=9;//预装载
  TIM4->PSC=71;//分频
  TIM4->CR1|=0x01<<4;
  TIM4->DIER|=0x01<<0;
  NVIC->IP[30]=0x30;//优先级3
  NVIC->ISER[0]|=0x01<<30;
  TIM4->CR1|=0x01<<0;//使能
	
	DisableProbe();
}

void SERVO::EnableProbe(){
	PWM = PWM_ENABLE;
	DelayMs(500);
	PWM = 0;
}

void SERVO::DisableProbe(){
	PWM = PWM_DISABLE;
	DelayMs(500);
	PWM = 0;
}


void SERVO::DelayMs(u32 n){
	while(n--){
		SysTick->LOAD=72000;
		SysTick->CTRL=0x00000005;
		while(!(SysTick->CTRL&0x00010000));
		SysTick->CTRL=0x00000004;
	}
}
