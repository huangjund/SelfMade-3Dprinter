#include "ABUS.h"

ABUS Abus;

extern"C"void USART1_IRQHandler(){
	if(USART1->SR & 0x01) //У�����
		Abus.VerifyError = 1;
	if(USART1->SR & 0x01<<5){ //��ʼ
		u8 Temp = USART1->DR;
		if(Temp == '%'){ //��ʼ
			Abus.TempBuffer[0] = Temp;
			Abus.DataCounter = 1;
		}
		else if(Temp != '^'){ //����
			if(Abus.DataCounter < 256) //����δ���
				Abus.TempBuffer[Abus.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������
			Abus.RxFinish = 1; //���ý�����־
//			Abus.TempBuffer[Abus.DataCounter++] = Temp; //��¼����
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
  USART1->BRR = 72000000/375000; //������375K
  USART1->CR1 |= 0x01<<13;//����ʹ��
  USART1->CR1 |= 0x01<<5; //�����ж�
//  USART1->CR1 |= 0x01<<7; //�����ж�
	USART1->CR1 |= 0x01<<12; //9������λ
	USART1->CR1 |= 0x01<<9; //��У��
	USART1->CR1 |= 0x01<<8; //����У���ж�
	USART1->CR1 |= 0x01<<10; //����У��
  NVIC->IP[37] = 0x10; //���ȼ�1
  NVIC->ISER[1] |= 0x01<<5; //�����ж���
  USART1->CR1 |= 0x01<<2; //����ʹ��
  USART1->CR1 |= 0x01<<3; //����ʹ��
}


bool ABUS::Transmit(char Address,char Data[],u32 TimeOut){
	RxFinish = 0;
	Tx('%'); //��ʼ��
	Tx((u8)Address); //��ַ
	Tx((u8)Address); 
	Tx((u8)Address); 
	Print(Data);
	return WaitForReceive(TimeOut); //�ȴ��ظ�
}

bool ABUS::TestReply(char Sym){ //����ִ
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
	for(u8 i=0;i<128;i++){ //��಻����128�ַ�
		if(data[i]=='^' || !data[i]){ //�ǽ�����
			Tx('^'); //���ͽ�����
			return; //ֹͣ����
		}
		else //�ǽ�����
			Tx((u8)data[i]); //����һ���ַ�
	}
	Tx('^'); //���䷢�ͽ�����
}

void ABUS::Tx(u8 data){
	while(!(USART1->SR&(0x01<<7)));
	USART1->DR = data;
}
