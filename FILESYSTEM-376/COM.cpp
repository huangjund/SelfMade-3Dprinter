#include "COM.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_SKIP[] = "KKK";
char STRING_FULL[] = "FFF";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";
char STRING_MOVE[] = "MMM";

extern"C"void USART2_IRQHandler(){
	if(USART2->SR & 0x01) //校验错误
		Serial.VerifyError = 1; //设置校验错误标志
	if(USART2->SR & (0x01<<5)){ //起始
		u8 Temp = USART2->DR; 
		if(Temp == '%'){ //起始符(开始接收)
			Serial.Buffer[0] = Temp; //写入L1缓存
			Serial.DataCounter = 1; //设置接收数据量
		}
		else if(Temp != '^'){ //数据(正在接收)
			if(Serial.DataCounter < 128) //缓存未溢出
				Serial.Buffer[Serial.DataCounter++] = Temp; //记录数据
		}
		else if(Temp == '^'){ //结束符(接收完毕)
			if(Serial.TestAddress()){ //确认地址相符
				Serial.RxFinish = 1; //设置结束标志
				for(u32 i=0;i<128;i++) //数据写入L2缓存
					Serial.RxData[i] = Serial.Buffer[i];
			}
			else{ //地址不符
				Serial.VerifyError = 0;
			}
		}
	}
}

void COM::Init(){	
	//RX-PA3 TX-PA2
  RCC->APB2ENR |= 0x01<<2;
  RCC->APB1ENR |= 0x01<<17;
  GPIOA->CRL &= 0xFFFF00FF;
  GPIOA->CRL |= 0x00004B00;
  USART2->CR1 = 0x00;
  USART2->CR2 = 0x00;
  USART2->CR3 = 0x00;
  USART2->GTPR = 0x00;
  USART2->BRR = 36000000/375000; //波特率375K
  USART2->CR1 |= 0x01<<13;//外设使能
  USART2->CR1 |= 0x01<<5; //接收中断
//  USART2->CR1 |= 0x01<<7; //发送中断
	USART2->CR1 |= 0x01<<12; //9个数据位
	USART2->CR1 |= 0x01<<9; //奇校验
	USART2->CR1 |= 0x01<<8; //开启校验中断
	USART2->CR1 |= 0x01<<10; //开启校验
  NVIC->IP[38] = 0x00; //优先级0
  NVIC->ISER[1] |= 0x01<<6; //开放中断线
  USART2->CR1 |= 0x01<<2; //接收使能
  USART2->CR1 |= 0x01<<3; //发送使能
}

void COM::Decoder(){
	if(!RxFinish) return; //未接收完成
	
	if(VerifyError){ //校验错误处理
		Print(STRING_ERROR);
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	Serial.RxFinish = 0; //清除接收完成标志
	if(RxData[4] == 'M'){ //移动指令
	}
	else{
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	for(u8 i=0;i<16;i++){ //最多不超过16字符
		if(data[i]=='^' || !data[i]){ //是结束符
			Tx('^'); //发送结束符
			return; //停止发送
		}
		else //非结束符
			Tx((u8)data[i]); //发送一个字符
	}
}

void COM::Tx(u8 data){
	while(!(USART2->SR&(0x01<<7)));
	USART2->DR = data;
}

bool COM::TestAddress(){
	u32 C = 0;
	if(Buffer[1] == 'F') C++;
	if(Buffer[2] == 'F') C++;
	if(Buffer[3] == 'F') C++;
	
	if(C >= 2) return 1; //检验成功
	return 0;
}

