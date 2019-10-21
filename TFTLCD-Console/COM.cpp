#include "COM.h"
#include "Status.h"
#include "LCD.h"

COM Serial;

extern"C"void USART1_IRQHandler(void){
	u8 Temp = USART1->DR;		
	if(Temp == '%'){ //起始符
		Serial.TempBuffer[0] = Temp;
		Serial.DataCounter = 1;
	}
	else if(Serial.DataCounter < 64){ //数据
		Serial.TempBuffer[Serial.DataCounter] = Temp;
		Serial.DataCounter++;
		if(Temp == '^'){ //结束符
			if(Serial.TestAddress()){ //地址相符
				for(u32 i=0;i<64;i++)
					Serial.Buffer[i] = Serial.TempBuffer[i];
				Serial.RxFinish = 1; //设置接收完成标志
				Serial.DataNumber = Serial.DataCounter; //记录数据量
			}
			Serial.DataCounter = 0; //重置接收计数器
		}
	}
}

void COM::Init(){
	RxFinish = 0;
	DataCounter = 0;
	DataNumber = 0;
  RCC->APB2ENR |= 0x01<<2; //启动PA时钟
  RCC->APB2ENR |= 0x01<<14; //启动USART1时钟
  GPIOA->CRH &= 0xFFFFF00F; //PA9:10清除配置
  GPIOA->CRH |= 0x000004F0; //PA9配置为AFOD，PA10配置为FIN
  USART1->CR2 = 0x0000; //清空CR2寄存器
  USART1->CR3 = 0x0000; //清空CR3寄存器
  USART1->GTPR = 0x0000; //清空GTPR寄存器
  USART1->BRR = 0xC0; //波特率375K Baud
  USART1->CR1 = 0x01<<13;//收发器使能
  USART1->CR1 |= 0x01<<5; //使能接收中断
//	USART1->CR1 |= 0x01<<7; //使能发送中断
  NVIC->IP[37] = 0x00; //中断优先级0
  NVIC->ISER[1] |= 0x01<<5; //开放中断线
  USART1->CR1 |= 0x01<<2; //接收使能
  USART1->CR1 |= 0x01<<3; //发送使能
}

void COM::Decoder(){
	if(DataNumber < 6) return;
	
	Status.DisableCursor();
	if(Buffer[5] == 'R'){ //重启
		LCDInterface.Init();
		Status.Init();
	}
	else if(Buffer[5] == 'C'){ //清屏
		Status.Clear();
	}
	else if(Buffer[5] == 'S'){ //改变光标状态
		Status.SetCursorStatus(Buffer[4]);
	}
	else if(Buffer[5] == 'A'){ //追加字符
		Status.AddChar(Buffer[4]);
	}
	else if(Buffer[5] == 'B'){ //修改状态栏
		if(DataNumber < 43){
			Status.EnableCursor();
			return;
		}
		Status.SetState((char*)(Buffer+6));
	}
	Status.EnableCursor();
}


void COM::Tx(u8 data){
	while(!(USART1->SR&(0x01<<7)));
	USART1->DR = data;
}

bool COM::TestAddress(){
	u32 C = 0;
	if(TempBuffer[1] == 'C') C++;
	if(TempBuffer[2] == 'C') C++;
	if(TempBuffer[3] == 'C') C++;
	
	if(C >= 2) return 1;

	return 0;
}
