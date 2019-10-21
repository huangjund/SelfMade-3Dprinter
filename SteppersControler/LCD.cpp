#include "LCD.h"
#include "StepperConfiguration.h"
#include "Stepper.h"
#include "Planner.h"
#include "Status.h"
LCD Lcd;




void LCD::Init(){
	
	//RX-PA3 TX-PA2
	RCC->APB2ENR |= 0x01<<2;
	RCC->APB1ENR |= 0x01<<17;
	GPIOA->CRL &= 0xFFFF00FF;
	GPIOA->CRL |= 0x00004B00;
	USART2->CR2 = 0x0000;
	USART2->CR3 = 0x0000;
	USART2->GTPR = 0x0000;
	USART2->BRR = 36000000/375000; //波特率375K
	USART2->CR1 = 0x01<<13; //外设使能
//	USART2->CR1 |= 0x01<<5; //接收中断
//	USART2->CR1 |= 0x01<<7; //发送中断
	NVIC->IP[38] = 0x10; //优先级1
	NVIC->ISER[1] |= 0x01<<6; //开放中断线
//	USART2->CR1 |= 0x01<<2; //接收使能
//	USART2->CR1 |= 0x01<<3; //发送使能
	

  
	
	
	RestartFlag = 0;
}

void LCD::SentStatus(){
	static u32 PXMax = (u32)AXIS_X_MAX_POSITION;
	static u32 PYMax = (u32)AXIS_Y_MAX_POSITION;
	static u32 PZMax = (u32)AXIS_Z_MAX_POSITION;
	
	static u32 L1Max = L1_SIZE;
	static u32 L2Max = L2_SIZE;
	Tx('%'); //起始符
	Tx_num((u32)Status.GetPosition(0)%1000); //X轴当前坐标
	Tx_num((u32)Status.GetPosition(1)%1000); //Y轴当前坐标
	Tx_num((u32)Status.GetPosition(2)%1000); //Z轴当前坐标
	Tx_num((u32)PXMax%1000); //X轴最大坐标
	Tx_num((u32)PYMax%1000); //Y轴最大坐标
	Tx_num((u32)PZMax%1000); //Z轴最大坐标
	Tx_num(L1Max-Stepper.L1RestAmount); //L1已使用容量
	Tx_num(L2Max-Planner.L2RestAmount); //L2已使用容量
	Tx_num(L1Max); //L1最大容量
	Tx_num(L2Max); //L2最大容量
	
	if(!Status.TestIfEnable()) Tx('D'); //失能状态
	else if(!Status.TestIfHomed()) Tx('N'); //尚未归位
	else Tx((u8)Stepper.FAStatus); //自动机状态
	
	if(Status.TestIfIDP()) Tx('E'); //IDP启用
	else Tx('D'); //IDP禁用
	
	if(RestartFlag){
		RestartFlag = 0;
		Tx('R');
	}
	else
		Tx('N');

}

void LCD::Print(u8 *data){
	for(u8 i=0;i<15;i++){ //最多不超过16字符
		if(data[i]=='\n' || !data[i]){ //是结束符
			return; //停止发送
		}
		else //非结束符
			Tx(data[i]); //发送一个字符
	}
}

void LCD::Tx(u8 data){
	while(!(USART2->SR&(0x01<<7)));
	USART2->DR = data;
}

void LCD::Tx_num(u32 data){
	static u32 weight[] = {1,1,10,100};
	u8 buf[4] = {};	
	for(u8 i=0;i<3;i++)
		buf[i] = data/weight[3-i] % 10 + '0'; //转码
	buf[3] = 0; //结束符
	Print(buf); //发送
}

