#include "IBUS.h"

IBUS Ibus;

extern"C"void USART2_IRQHandler(){
	if(USART2->SR & 0x01) //У�����
		Ibus.VerifyError = 1;
	if(USART2->SR & 0x01<<5){ //��ʼ
		u8 Temp = USART2->DR;
		if(Temp == '%'){ //��ʼ
			Ibus.TempBuffer[0] = Temp;
			Ibus.DataCounter = 1;
		}
		else if(Temp != '^'){ //����
			if(Ibus.DataCounter < 255) //����δ���
				Ibus.TempBuffer[Ibus.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������
			Ibus.RxFinish = 1; //���ý�����־
//			Ibus.TempBuffer[Ibus.DataCounter++] = Temp; //��¼����
			for(u32 i=0;i<Ibus.DataCounter;i++)
				Ibus.Buffer[i] = Ibus.TempBuffer[i];
			Ibus.DataNumber = Ibus.DataCounter;
		}
	}
}

void IBUS::Init(){
	//RX-PA3 TX-PA2
  RCC->APB2ENR=0x01<<2;
  RCC->APB1ENR|=0x01<<17;
  GPIOA->CRL&=0xFFFF00FF;
  GPIOA->CRL|=0x00004F00;
  USART2->CR1 = 0x00;
  USART2->CR2 = 0x00;
  USART2->CR3 = 0x00;
  USART2->GTPR = 0x00;
  USART2->BRR = 36000000/375000; //������375K
  USART2->CR1 |= 0x01<<13;//����ʹ��
  USART2->CR1 |= 0x01<<5; //�����ж�
//  USART2->CR1 |= 0x01<<7; //�����ж�
	USART2->CR1 |= 0x01<<12; //9������λ
	USART2->CR1 |= 0x01<<9; //��У��
	USART2->CR1 |= 0x01<<8; //����У���ж�
	USART2->CR1 |= 0x01<<10; //����У��
  NVIC->IP[38] = 0x00; //���ȼ�0
  NVIC->ISER[1] |= 0x01<<6; //�����ж���
  USART2->CR1 |= 0x01<<2; //����ʹ��
  USART2->CR1 |= 0x01<<3; //����ʹ��
}


bool IBUS::Transmit(char Address,char Data[],u32 TimeOut){
	RxFinish = 0;
	Tx('%'); //��ʼ��
	Tx((u8)Address); //��ַ
	Tx((u8)Address); 
	Tx((u8)Address); 
	Print(Data);
	return WaitForReceive(TimeOut); //�ȴ��ظ�
}

bool IBUS::TestReply(char Sym){ //����ִ
	RxFinish = 0;
	VerifyError = 0;
	
	u32 C = 0;
	if(Buffer[1] == Sym) C++;
	if(Buffer[2] == Sym) C++;
	if(Buffer[3] == Sym) C++;
	
	if(C >= 2) return 1;
	return 0;
}

bool IBUS::TestIfVerifyError(){
	bool TempStatus = VerifyError;
	VerifyError = 0;
	return TempStatus;
}

bool IBUS::WaitForReceive(u32 Ms){
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


void IBUS::Print(char *data){
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

void IBUS::Tx(u8 data){
	while(!(USART2->SR&(0x01<<7)));
	USART2->DR = data;
}
