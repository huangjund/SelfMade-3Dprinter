#include "Interface.h"

void STEPPER_X_PLUSE(bool S){ //设置步进电机X脉冲状态
	 if(S)
		 GPIOB->BSRR |= 0x01<<15;
	 else
		 GPIOB->BRR |= 0x01<<15;
}

void STEPPER_Y_PLUSE(bool S){ //设置步进电机Y脉冲状态
	 if(S)
		 GPIOC->BSRR |= 0x01<<13;
	 else
		 GPIOC->BRR |= 0x01<<13;
}

void STEPPER_Z_PLUSE(bool S){ //设置步进电机Z脉冲状态
	 if(S)
		 GPIOA->BSRR |= 0x01<<4;
	 else
		 GPIOA->BRR |= 0x01<<4;
}

void STEPPER_A_PLUSE(bool S){ //设置步进电机A脉冲状态
	 if(S)
		 GPIOA->BSRR |= 0x01<<15;
	 else
		 GPIOA->BRR |= 0x01<<15;
}

void STEPPER_B_PLUSE(bool S){ //设置步进电机B脉冲状态
	 if(S)
		 GPIOA->BSRR |= 0x01<<10;
	 else
		 GPIOA->BRR |= 0x01<<10;
}




void STEPPER_X_DIRECTION(bool D){ //设置步进电机X方向状态
	 if(!D)
		 GPIOB->BSRR |= 0x01<<14;
	 else
		 GPIOB->BRR |= 0x01<<14;
}

void STEPPER_Y_DIRECTION(bool D){ //设置步进电机Y方向状态
	 if(D)
		 GPIOC->BSRR |= 0x01<<14;
	 else
		 GPIOC->BRR |= 0x01<<14;
}

void STEPPER_Z_DIRECTION(bool D){ //设置步进电机Z方向状态
	 if(!D)
		 GPIOA->BSRR |= 0x01<<5;
	 else
		 GPIOA->BRR |= 0x01<<5;
}

void STEPPER_A_DIRECTION(bool D){ //设置步进电机A方向状态
	 if(D)
		 GPIOA->BSRR |= 0x01<<12;
	 else
		 GPIOA->BRR |= 0x01<<12;
}

void STEPPER_B_DIRECTION(bool D){ //设置步进电机B方向状态
	 if(D)
		 GPIOA->BSRR |= 0x01<<9;
	 else
		 GPIOA->BRR |= 0x01<<9;
}



void STEPPER_ENABLE(bool E){ //设置步进电机状态
	if(E){
		GPIOA->BRR |= 0x01<<6;
		GPIOC->BRR |= 0x01<<15;
		GPIOB->BRR |= 0x01<<13;
		GPIOA->BRR |= 0x01<<11;
		GPIOA->BRR |= 0x01<<8;		
	}
	else{
		GPIOA->BSRR |= 0x01<<6;
		GPIOC->BSRR |= 0x01<<15;
		GPIOB->BSRR |= 0x01<<13;
		GPIOA->BSRR |= 0x01<<11;
		GPIOA->BSRR |= 0x01<<8;		
	}
}



void PROB_ENABLE(bool E){ //设置探针状态
	if(E)
		GPIOB->BSRR |= 0x01<<3;
	else
		GPIOB->BRR |= 0x01<<3;
}

void AUX_ENABLE(bool E){ //设置辅助端口状态
	if(E)
		GPIOB->BSRR |= 0x01<<3;
	else
		GPIOB->BRR |= 0x01<<3;
}


bool TEST_X_MAX(){ //检测X终点限位器状态
	if(GPIOB->IDR & (0x01<<9))
		return 0;
	else
		return 1;
}

bool TEST_Y_MAX(){ //检测Y终点限位器状态
	if(GPIOB->IDR & (0x01<<7))
		return 0;
	else
		return 1;
}

bool TEST_Z_MAX(){ //检测Z终点限位器状态
	if(GPIOB->IDR & (0x01<<5))
		return 0;
	else
		return 1;
}



bool TEST_X_MIN(){ //检测X起点限位器状态
	if(GPIOB->IDR & (0x01<<8))
		return 0;
	else
		return 1;
}

bool TEST_Y_MIN(){ //检测Y起点限位器状态
	if(GPIOB->IDR & (0x01<<6))
		return 0;
	else
		return 1;
}

bool TEST_Z_MIN(){ //检测Z起点限位器状态
	if(GPIOB->IDR & (0x01<<4))
		return 0;
	else
		return 1;
}


void Delay_Us(u32 n){
	SysTick->LOAD=72*n;
	SysTick->CTRL=0x00000005;
	while(!(SysTick->CTRL&0x00010000));
	SysTick->CTRL=0x00000004;
}

void Delay_Ms(u32 n){
	while(n--)
		Delay_Us(1000);
}

