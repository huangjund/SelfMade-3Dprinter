#include "stm32f10x.h"
#include "CH376.h"
#include "Interface.h"

extern"C"void TIM4_IRQHandler(void){
	if(TIM4->SR&0x01){
		Udisk.TestIfConnectFlag = 1;
		TIM4->SR&=~0x01;
	}
}


int main(){
	RCC->APB2ENR |= 0x01;
	AFIO->MAPR &= 0xF0FFFFFF;
	AFIO->MAPR |= 0x02000000;
	
	RCC->APB1ENR|=0x01<<2;
  TIM4->ARR=50000;//预装载
  TIM4->PSC=3599;//分频
  TIM4->CR1|=0x01<<4;
  TIM4->DIER|=0x01<<0;
  NVIC->IP[30]=0x20;//优先级2
  NVIC->ISER[0]|=0x01<<30;
  TIM4->CR1|=0x01<<0;//使能
  
	Interface.Init();
	
	while(1){
		if(Udisk.TestIfConnectFlag){ //有挂载检测请求
			Udisk.TestIfConnect(); //检测是否有磁盘连接
		}
		
	}

}





