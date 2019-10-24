#include "ABUS.h"

ABUS Abus;

extern"C"void USART1_IRQHandler(){
	if(USART1->SR & 0x01) //校验错误
		Abus.VerifyError = 1;
	if(USART1->SR & 0x01<<5){ //起始
		u8 Temp = USART1->DR;
		if(Temp == '%'){ //起始
			Abus.TempBuffer[0] = Temp;
			Abus.DataCounter = 1;
		}
		else if(Temp != '^'){ //数据
			if(Abus.DataCounter < 256) //缓存未溢出
				Abus.TempBuffer[Abus.DataCounter++] = Temp; //记录数据
		}
		else if(Temp == '^'){ //结束符
			Abus.RxFinish = 1; //设置结束标志
//			Abus.TempBuffer[Abus.DataCounter++] = Temp; //记录数据
			for(u32 i=0;i<Abus.DataCounter;i++)
				Abus.Buffer[i] = Abus.TempBuffer[i];
			Abus.DataNumber = Abus.DataCounter;
		}
	}
}

void ABUS::Init(){
//RX-PA10 TX-PA9
  RCC->APB2ENR |= 0x01<<2;
  RCC->APB2ENR |= 0x01<<14;
  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000004F0; 
  USART1->CR1 = 0x00;
  USART1->CR2 = 0x00;
  USART1->CR3 = 0x00;
  USART1->GTPR = 0x00;
  USART1->BRR = 72000000/375000; //波特率375K
  USART1->CR1 |= 0x01<<13;//外设使能
  USART1->CR1 |= 0x01<<5; //接收中断
//  USART1->CR1 |= 0x01<<7; //发送中断
	USART1->CR1 |= 0x01<<12; //9个数据位
	USART1->CR1 |= 0x01<<9; //奇校验
	USART1->CR1 |= 0x01<<8; //开启校验中断
	USART1->CR1 |= 0x01<<10; //开启校验
  NVIC->IP[37] = 0x10; //优先级1
  NVIC->ISER[1] |= 0x01<<5; //开放中断线
  USART1->CR1 |= 0x01<<2; //接收使能
  USART1->CR1 |= 0x01<<3; //发送使能
}


bool ABUS::Transmit(char Address,char Data[],u32 TimeOut){
	RxFinish = 0;
	Tx('%'); //起始符
	Tx((u8)Address); //地址
	Tx((u8)Address); 
	Tx((u8)Address); 
	Print(Data);
	return WaitForReceive(TimeOut); //等待回复
}

bool ABUS::TestReply(char Sym){ //检测回执
	RxFinish = 0;
	VerifyError = 0;
	
	u32 C = 0;
	if(Buffer[1] == Sym) C++;
	if(Buffer[2] == Sym) C++;
	if(Buffer[3] == Sym) C++;
	
	if(C >= 2) return 1;
	return 0;
}

bool ABUS::TestIfVerifyError(){
	bool TempStatus = VerifyError;
	VerifyError = 0;
	return TempStatus;
}

bool ABUS::WaitForReceive(u32 Ms){
	while(Ms--){
		SysTick->LOAD=72000;
		SysTick->CTRL=0x00000005;
		while(!(SysTick->CTRL&0x00010000))
			if(RxFinish) break;
		SysTick->CTRL=0x00000004;		
		if(RxFinish) break;
	}
	if(RxFinish) return 1;
	else return 0;
}


void ABUS::Print(char *data){
	for(u8 i=0;i<128;i++){ //最多不超过128字符
		if(data[i]=='^' || !data[i]){ //是结束符
			Tx('^'); //发送结束符
			return; //停止发送
		}
		else //非结束符
			Tx((u8)data[i]); //发送一个字符
	}
	Tx('^'); //补充发送结束符
}

void ABUS::Tx(u8 data){
	while(!(USART1->SR&(0x01<<7)));
	USART1->DR = data;
}
