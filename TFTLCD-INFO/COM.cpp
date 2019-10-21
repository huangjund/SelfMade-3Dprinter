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
	else if(Serial.DataCounter < 128){ //数据
		Serial.TempBuffer[Serial.DataCounter] = Temp;
		Serial.DataCounter++;
		if(Temp == '^'){ //结束符
			if(Serial.TestAddress()){ //地址相符
				for(u32 i=0;i<128;i++)
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
	if(DataNumber < 7) return;
	Status.TempExtruderA = C2Q(Buffer+4);
	if(Status.TempExtruderA > 320)
		Status.TempExtruderA = 320;	
	
	if(DataNumber < 10) return;
	Status.TempExtruderB = C2Q(Buffer+7);
	if(Status.TempExtruderB > 320)
		Status.TempExtruderB = 320;	
	
	if(DataNumber < 13) return;
	Status.TempBasePlane = C2Q(Buffer+10);
	if(Status.TempBasePlane > 150)
		Status.TempBasePlane = 150;
	
	if(DataNumber < 16) return;
	Status.TempExtruderASet = C2Q(Buffer+13);
	if(Status.TempExtruderASet > 320)
		Status.TempExtruderASet = 320;	
	
	if(DataNumber < 19) return;
	Status.TempExtruderBSet = C2Q(Buffer+16);
	if(Status.TempExtruderBSet > 320)
		Status.TempExtruderBSet = 320;	
	
	if(DataNumber < 22) return;
	Status.TempBasePlaneSet = C2Q(Buffer+19);
	if(Status.TempBasePlaneSet > 150)
		Status.TempBasePlaneSet = 150;
	
	if(DataNumber < 25) return;
	Status.PowerHeaterExtruderA = C2Q(Buffer+22);
	if(Status.PowerHeaterExtruderA > 100)
		Status.PowerHeaterExtruderA = 100;	
	
	if(DataNumber < 28) return;
	Status.PowerHeaterExtruderB = C2Q(Buffer+25);
	if(Status.PowerHeaterExtruderB > 100)
		Status.PowerHeaterExtruderB = 100;	
	
	if(DataNumber < 31) return;
	Status.PowerHeaterBasePlane = C2Q(Buffer+28);
	if(Status.PowerHeaterBasePlane > 100)
		Status.PowerHeaterBasePlane = 100;	
	
	if(DataNumber < 34) return;
	Status.PowerCoolerExtruderA = C2Q(Buffer+31);
	if(Status.PowerCoolerExtruderA > 100)
		Status.PowerCoolerExtruderA = 100;	
	
	if(DataNumber < 37) return;
	Status.PowerCoolerExtruderB = C2Q(Buffer+34);
	if(Status.PowerCoolerExtruderB > 100)
		Status.PowerCoolerExtruderB = 100;
	
	if(DataNumber < 40) return;
	Status.TempAir = C2Q(Buffer+37);
	if(Status.TempAir > 100)
		Status.TempAir = 100;
	
	if(DataNumber < 41) return;
	Status.StateExtruderA = (char)Buffer[40];
	
	if(DataNumber < 42) return;
	Status.StateExtruderB = (char)Buffer[41];
	
	if(DataNumber < 43) return;
	Status.StateBasePlane = (char)Buffer[42];
	
	if(DataNumber < 46) return;
	Status.PosiX = C2Q(Buffer+43);
	
	if(DataNumber < 49) return;
	Status.PosiY = C2Q(Buffer+46);
	
	if(DataNumber < 52) return;
	Status.PosiZ = C2Q(Buffer+49);
	
	if(DataNumber < 55) return;
	Status.PosiXMax = C2Q(Buffer+52);
	
	if(DataNumber < 58) return;
	Status.PosiYMax = C2Q(Buffer+55);
	
	if(DataNumber < 61) return;
	Status.PosiZMax = C2Q(Buffer+58);
	
	if(Status.PosiX > Status.PosiXMax)
		Status.PosiX = Status.PosiXMax;
	if(Status.PosiY > Status.PosiYMax)
		Status.PosiY = Status.PosiYMax;
	if(Status.PosiZ > Status.PosiZMax)
		Status.PosiZ = Status.PosiZMax;
	
	if(DataNumber < 64) return;
	Status.L1Used = C2Q(Buffer+61);
	
	if(DataNumber < 67) return;
	Status.L2Used = C2Q(Buffer+64);
	
	if(DataNumber < 70) return;
	Status.L1Max = C2Q(Buffer+67);
	
	if(DataNumber < 73) return;
	Status.L2Max = C2Q(Buffer+70);
	
	if(Status.L1Used > Status.L1Max)
		Status.L1Used = Status.L1Max;
	if(Status.L2Used > Status.L2Max)
		Status.L2Used = Status.L2Max;
	
	if(DataNumber < 74) return;
	Status.StateFA = (char)Buffer[73];
	
	if(DataNumber < 75) return;
	Status.StateIDP = (char)Buffer[74];
	
	if(DataNumber < 76) return;
	if(Buffer[75] == 'R')
			LCDInterface.Init();
}


u32 COM::C2Q(u8 *data){
	static u32 weight[] = {1,1,10,100};
	u32 value = 0;
	for(u8 i=0;i<3;i++) //转换为数字
		value += (data[i]-'0')*weight[3-i];
	return value;
}


bool COM::TestAddress(){
	u32 C = 0;
	if(TempBuffer[1] == 'I') C++;
	if(TempBuffer[2] == 'I') C++;
	if(TempBuffer[3] == 'I') C++;
	
	if(C >= 2) return 1;

	return 0;
}

void COM::Tx(u8 data){
	while(!(USART1->SR&(0x01<<7)));
	USART1->DR = data;
}
