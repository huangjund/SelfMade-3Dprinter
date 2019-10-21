#include "SPOUT.h"
#include "Status.h"

SPOUT Spout;

extern"C"void TIM3_IRQHandler(void){
	if(TIM3->SR&0x01){
		TIM3->ARR = 0; //�رն�ʱ��
		Spout.WaitFlag = 0; //����ȴ���־
		TIM3->SR&=~0x01;
	}
}

extern"C"void USART3_IRQHandler(void){
	if(USART3->SR &0x20){
		u8 Temp = USART3->DR;
		if(Temp == 'D')
			Spout.WaitFlag = 0;
		else
			Spout.WaitFlag = 0;
	}
}

void SPOUT::Init(){
	//RX-PB11 TX-PB10
	RCC->APB2ENR |= 0x01<<3;
	RCC->APB1ENR |= 0x01<<18;
	GPIOB->CRH &= 0xFFFF00FF;
	GPIOB->CRH |= 0x00004F00;
  USART3->CR2 = 0x0000;
  USART3->CR3 = 0x0000;
  USART3->GTPR = 0x0000;
  USART3->BRR = 36000000/375000; //������375K
  USART3->CR1 = 0x01<<13;//����ʹ��
  USART3->CR1 |= 0x01<<5; //�����ж�
//  USART3->CR1 |= 0x01<<7; //�����ж�
	NVIC->IP[39] = 0x10; //���ȼ�1
	NVIC->ISER[1] |= 0x01<<7; //�����ж���
  USART3->CR1 |= 0x01<<2; //����ʹ��
  USART3->CR1 |= 0x01<<3; //����ʹ��

	RCC->APB1ENR |= 0x01<<1;
  TIM3->ARR = 0;//Ԥװ��
  TIM3->PSC = 71;//��Ƶ
  TIM3->CR1 |= 0x01<<4;
  TIM3->DIER |= 0x01<<0;
  NVIC->IP[29] = 0x30;//���ȼ�3
  NVIC->ISER[0] |= 0x01<<29;
  TIM3->CR1 |= 0x01<<0;//ʹ��

}

void SPOUT::InfoRestart(){
	static char Data[73];
	char *TempPtr = Data;
	
	TempPtr = Num2Chr(Status.TempExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.TempExtruderB,TempPtr);
	TempPtr = Num2Chr(Status.TempBasePlane,TempPtr);
	
	TempPtr = Num2Chr(Status.TempExtruderASet,TempPtr);
	TempPtr = Num2Chr(Status.TempExtruderBSet,TempPtr);
	TempPtr = Num2Chr(Status.TempBasePlaneSet,TempPtr);
	
	TempPtr = Num2Chr(Status.PowerHeaterExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.PowerHeaterExtruderB,TempPtr);
	TempPtr = Num2Chr(Status.PowerHeaterBasePlane,TempPtr);
	
	TempPtr = Num2Chr(Status.PowerCoolerExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.PowerCoolerExtruderB,TempPtr);
	
	TempPtr = Num2Chr(Status.TempAir,TempPtr);
	
	*(TempPtr++) = Status.StateExtruderA;
	*(TempPtr++) = Status.StateExtruderB;
	*(TempPtr++) = Status.StateBasePlane;
	
	TempPtr = Num2Chr(Status.PosiX,TempPtr);
	TempPtr = Num2Chr(Status.PosiY,TempPtr);
	TempPtr = Num2Chr(Status.PosiZ,TempPtr);
	
	TempPtr = Num2Chr(Status.PosiXMax,TempPtr);
	TempPtr = Num2Chr(Status.PosiYMax,TempPtr);
	TempPtr = Num2Chr(Status.PosiZMax,TempPtr);
	
	TempPtr = Num2Chr(Status.L1Used,TempPtr);
	TempPtr = Num2Chr(Status.L2Used,TempPtr);
	
	TempPtr = Num2Chr(Status.L1Max,TempPtr);
	TempPtr = Num2Chr(Status.L2Max,TempPtr);
	
	*(TempPtr++) = Status.StateFA;
	*(TempPtr++) = Status.StateIDP;
	
	*(TempPtr++) = 'R';
	
	*(TempPtr) = 0;
	
	while(WaitFlag);
	Transmit('I',Data);
	while(WaitFlag);
}

void SPOUT::InfoRefresh(){
	static char Data[73];
	char *TempPtr = Data;
	
	TempPtr = Num2Chr(Status.TempExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.TempExtruderB,TempPtr);
	TempPtr = Num2Chr(Status.TempBasePlane,TempPtr);
	
	TempPtr = Num2Chr(Status.TempExtruderASet,TempPtr);
	TempPtr = Num2Chr(Status.TempExtruderBSet,TempPtr);
	TempPtr = Num2Chr(Status.TempBasePlaneSet,TempPtr);
	
	TempPtr = Num2Chr(Status.PowerHeaterExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.PowerHeaterExtruderB,TempPtr);
	TempPtr = Num2Chr(Status.PowerHeaterBasePlane,TempPtr);
	
	TempPtr = Num2Chr(Status.PowerCoolerExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.PowerCoolerExtruderB,TempPtr);
	
	TempPtr = Num2Chr(Status.TempAir,TempPtr);
	
	*(TempPtr++) = Status.StateExtruderA;
	*(TempPtr++) = Status.StateExtruderB;
	*(TempPtr++) = Status.StateBasePlane;
	
	TempPtr = Num2Chr(Status.PosiX,TempPtr);
	TempPtr = Num2Chr(Status.PosiY,TempPtr);
	TempPtr = Num2Chr(Status.PosiZ,TempPtr);
	
	TempPtr = Num2Chr(Status.PosiXMax,TempPtr);
	TempPtr = Num2Chr(Status.PosiYMax,TempPtr);
	TempPtr = Num2Chr(Status.PosiZMax,TempPtr);
	
	TempPtr = Num2Chr(Status.L1Used,TempPtr);
	TempPtr = Num2Chr(Status.L2Used,TempPtr);
	
	TempPtr = Num2Chr(Status.L1Max,TempPtr);
	TempPtr = Num2Chr(Status.L2Max,TempPtr);
	
	*(TempPtr++) = Status.StateFA;
	*(TempPtr++) = Status.StateIDP;
	
	*(TempPtr++) = 'N';
	
	*(TempPtr) = 0;
	
	while(WaitFlag);
	Transmit('I',Data);
	while(WaitFlag);
}

void SPOUT::ConsoleRestart(){
	char *Data = " R";
	while(WaitFlag);
	Transmit('C',Data);
	while(WaitFlag);
}

void SPOUT::ConsoleClear(){
	char *Data = " C";
	
	while(WaitFlag);
	Transmit('C',Data);
	while(WaitFlag);
}

void SPOUT::ConsoleSetCursorBusy(){
	char *Data = "BS";
	
	while(WaitFlag);
	Transmit('C',Data);
	while(WaitFlag);
}

void SPOUT::ConsoleSetCursorInsert(){
	char *Data = "IS";
	
	while(WaitFlag);
	Transmit('C',Data);
	while(WaitFlag);
}

void SPOUT::ChangeStatusBar(){
	static char Data[40] = " B";
	for(u32 i=0;i<38;i++)
		Data[i+2] = Status.StateBar[i];
	
	while(WaitFlag);
	Transmit('C',Data);
	while(WaitFlag);
}

void SPOUT::Display(char Data[]){
	static char TempData[3] = {' ','A',0};
	for(u32 i=0;i<1024 && Data[i];i++){
		TempData[0] = Data[i];
		while(WaitFlag);
		DelayUs(5000);
		Transmit('C',TempData);
		while(WaitFlag);
	}
}

bool SPOUT::Transmit(char Address,char Data[]){
	if(WaitFlag) return 0;
	
	Tx('%'); //��ʼ��
	
	if(Address == 'I' || Address == 'C'){ //��ַ
		Tx((u8)Address);
		Tx((u8)Address);
		Tx((u8)Address);
	}
	
	for(u32 i=0;i<128 && Data[i];i++) //����
		Tx((u8)Data[i]);
	
	Tx('^'); //������
	
	WaitFlag = 1; //��ʼ�ȴ��ظ�
	TIM3->ARR = 50000; //���50ms
	return 1;
}

void SPOUT::Tx(u8 Data){
	while(!(USART3->SR & (0x01<<7)));
	USART3->DR = Data;
}

char* SPOUT::Num2Chr(u32 Data,char* Str){
	static const u32 Weight[] = {1,1,10,100};
	for(u8 i=0;i<3;i++)
		*(Str++) = Data/Weight[3-i] % 10 + '0'; //ת��
	return Str;
}

void SPOUT::DelayUs(u32 n){
	SysTick->LOAD=72*n;
	SysTick->CTRL=0x00000005;
	while(!(SysTick->CTRL&0x00010000));
	SysTick->CTRL=0x00000004;
}
