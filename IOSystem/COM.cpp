#include "COM.h"
#include "SPIO.h"
#include "SPOUT.h"
#include "Status.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";
char STRING_ON[] = "OOO";
char STRING_OFF[] = "FFF";

extern"C"void USART1_IRQHandler(){
	if(USART1->SR & 0x01) //校验错误
		Serial.VerifyError = 1;
	if(USART1->SR & 0x01<<5){ //起始
		u8 Temp = USART1->DR;
		if(Temp == '%'){ //起始
			Serial.Buffer[0] = Temp;
			Serial.DataCounter = 1;
		}
		else if(Temp != '^'){ //数据
			if(Serial.DataCounter < 256) //缓存未溢出
				Serial.Buffer[Serial.DataCounter++] = Temp; //记录数据
		}
		else if(Temp == '^'){ //结束符
			if(Serial.TestAddress()){ //确认地址
				Serial.RxFinish = 1; //设置结束标志
				Serial.DataNumber = Serial.DataCounter;
				for(u32 i=0;i<256;i++)
					Serial.RxData[i] = Serial.Buffer[i];
			}
			else{ //地址不符
				Serial.VerifyError = 0;
			}
		}
	}
}

void COM::Init(){
  //RX-PA10 TX-PA9
  RCC->APB2ENR=0x01<<2;
  RCC->APB2ENR|=0x01<<14;
  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000004F0;
  USART1->CR1 = 0x00;
  USART1->CR2 = 0x00;
  USART1->CR3 = 0x00;
  USART1->GTPR = 0x00;
  USART1->BRR = 72000000/375000; //波特率375K
  USART1->CR1 |= 0x01<<13;//外设使能
  USART1->CR1 |= 0x01<<5; //接收中断
	USART1->CR1 |= 0x01<<12; //9个数据位
	USART1->CR1 |= 0x01<<9; //奇校验
	USART1->CR1 |= 0x01<<8; //开启校验中断
	USART1->CR1 |= 0x01<<10; //开启校验
  NVIC->IP[37] = 0x00; //优先级0
  NVIC->ISER[1] |= 0x01<<5; //开放中断线
  USART1->CR1 |= 0x01<<2; //接收使能
  USART1->CR1 |= 0x01<<3; //发送使能
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
	
	if(Compare((char*)(RxData+4),"GETCOM")){ //获取命令指令
		if(Spio.HangUpFlag)
			Print(Spio.ComBuffer);
		else
			Print("");
	}
	else if(Compare((char*)(RxData+4),"SETSTA")){ //设置状态栏指令
		for(u32 i=0;i<37 && RxData[10+i]!='^';i++)
			Status.StateBar[i] = RxData[10+i];
		Status.StateBar[37] = 0;
		Spout.ChangeStatusBar();
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"SETINF")){ //设置状态显示器指令
		Status.SetInfo(RxData+10,DataNumber-10);
		Spout.InfoRefresh();
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"SETDAT")){ //控制台显示指令并解除挂起
		RxData[DataNumber] = 0;
//		Spout.Display("\n");
		Spout.Display((char*)(RxData+10));
		Spio.ProcessedFlag = 1;
		Spio.HangUpFlag = 0;
		Print(STRING_DONE);		
	}
	else if(Compare((char*)(RxData+4),"HLDDAT")){ //控制台显示指令并保持挂起
		RxData[DataNumber] = 0;
//		Spout.Display("\n");
		Spout.Display((char*)(RxData+10));
		Spio.HangUpFlag = 1;
		TIM4->CNT = 50000;
		Print(STRING_DONE);		
	}
	else if(Compare((char*)(RxData+4),"WAITIN")){ //控制台挂起指令
		Spio.HangUpFlag = 1;
		TIM4->CNT = 50000;
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"ADDDIC")){ //字典扩充指令
		RxData[DataNumber] = 0;
		if(DataNumber > 10)
			Spio.AddWord((char*)(RxData+10));
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"GETSWA")){ //开关A状态检测指令
		if(GPIOA->IDR & (0x01<<6))
			Print(STRING_OFF);
		else
			Print(STRING_ON);
	}
	else if(Compare((char*)(RxData+4),"GETSWB")){ //开关B状态检测指令
		if(GPIOA->IDR & (0x01<<7))
			Print(STRING_OFF);
		else
			Print(STRING_ON);
	}
	else if(Compare((char*)(RxData+4),"TSTLIK")){ //通讯检测指令
		Print(STRING_DONE);
	}
	else{ //无法识别
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	Tx('%'); //起始符
	for(u8 i=0;i<256;i++){ //最多不超过256字符
		if(data[i]=='^' || !data[i]){ //是结束符
			Tx('^'); //发送结束符
			return; //停止发送
		}
		else //非结束符
			Tx((u8)data[i]); //发送一个字符
	}
}

void COM::Tx(u8 data){
	while(!(USART1->SR&(0x01<<7)));
	USART1->DR = data;
}

u32 COM::Chr2Int(u8 *Data){
	static u32 weight[] = {1,1,10,100};
	u32 Value = 0;
	for(u8 i=0;i<3;i++) //转换为数字
		Value += (Data[i]-'0')*weight[3-i];
	return Value;
}

bool COM::TestAddress(){
	u32 C = 0;
	if(Buffer[1] == 'C') C++;
	if(Buffer[2] == 'C') C++;
	if(Buffer[3] == 'C') C++;
	
	if(C >= 2) return 1; //检验成功
	return 0;
}

bool COM::Compare(char StrA[],char StrB[]){
	for(u32 i=0;i<24;i++){
		if(!StrA[i] || !StrB[i]) break;
		if(StrA[i] != StrB[i])
			return 0;
	}
	return 1;
}
