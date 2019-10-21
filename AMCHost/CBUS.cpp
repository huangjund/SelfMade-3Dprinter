#include "CBUS.h"

CBUS Cbus;

extern"C"void USART3_IRQHandler(){
	if(USART3->SR & 0x01) //У�����
		Cbus.VerifyError = 1;
	if(USART3->SR & 0x01<<5){ //��ʼ���ܵ�����
		u8 Temp = USART3->DR;//8λ����ֵ
		if(Temp == '%'){ //��ʼ
			Cbus.TempBuffer[0] = Temp;
			Cbus.DataCounter = 1;
		}
		else if(Temp != '^'){ //����
			if(Cbus.DataCounter < 128) //����δ���
				Cbus.TempBuffer[Cbus.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������
			Cbus.RxFinish = 1; //���ý�����־
			for(u32 i=0;i<128;i++)
				Cbus.Buffer[i] = Cbus.TempBuffer[i];
			Cbus.DataNumber = Cbus.DataCounter;
		}
	}
}

void CBUS::Init(){
	//RX-PB11 TX-PB10
	RCC->APB2ENR |= 0x01<<3;//IO�˿�Bʱ��ʹ��
	RCC->APB1ENR |= 0x01<<18;//USART3ʱ��ʹ��
	GPIOB->CRH &= 0xFFFF00FF;//�˿����ø߼Ĵ�����10 11��������
	GPIOB->CRH |= 0x00004F00;//MODE11������ģʽ CNF11���������� �� MODE10�� ���ģʽ������ٶ�50MHz CNF10�����ù��ܿ�©���
	
  USART3->CR1 = 0x00;
  USART3->CR2 = 0x00;
  USART3->CR3 = 0x00;//USART3���ƼĴ�������
  USART3->GTPR = 0x00;//����ʱ���Ԥ��Ƶ�Ĵ�������
	
  USART3->BRR = 36000000/375000; //������375K��û����
  USART3->CR1 |= 0x01<<13;//USARTģ��ʹ�ܣ��������0 1 ��
  USART3->CR1 |= 0x01<<5; //���ܻ������ǿ��ж�ʹ�ܣ���SR�е�ORE����RXNEΪ��1��ʱ����USART�ж�
//  USART3->CR1 |= 0x01<<7; //���ͻ��������ж�ʹ��
	USART3->CR1 |= 0x01<<12; //1����ʼλ��9������λ��n��ֹͣλ
	USART3->CR1 |= 0x01<<9; //��żУ��ѡ����У��
	USART3->CR1 |= 0x01<<8; //����У���ж�
	USART3->CR1 |= 0x01<<10; //У�����ʹ�ܣ�����У��
  NVIC->IP[39] = 0x00; //���ȼ�0
  NVIC->ISER[1] |= 0x01<<7; //�����ж���
  USART3->CR1 |= 0x01<<2; //����ʹ�ܣ�������û����
  USART3->CR1 |= 0x01<<3; //����ʹ�ܣ�������û����
}


bool CBUS::Transmit(char Address,char Data[],u32 TimeOut){
	RxFinish = 0;
	Tx('%'); //��ʼ��
	Tx((u8)Address); //��ַ��������
	Tx((u8)Address); 
	Tx((u8)Address); 
	Print(Data);
	return WaitForReceive(TimeOut); //�ȴ��ظ�
}

bool CBUS::TestReply(char Sym){ //����ִ
	RxFinish = 0;
	VerifyError = 0;
	
	u32 C = 0;
	if(Buffer[1] == Sym) C++;
	if(Buffer[2] == Sym) C++;
	if(Buffer[3] == Sym) C++;
	
	if(C >= 2) return 1;
	return 0;
}

bool CBUS::TestIfVerifyError(){
	bool TempStatus = VerifyError;
	VerifyError = 0;
	return TempStatus;
}

bool CBUS::WaitForReceive(u32 Ms){//û��
	while(Ms--){
		SysTick->LOAD=72000;
		SysTick->CTRL=0x00000005;//ʱ��Դѡ�񣺴�����ʱ��AHB Systickʹ��
		while(!(SysTick->CTRL&0x00010000))//���ϴζ�ȡ���Ĵ�����Systick�Ѿ�������0
			if(RxFinish) break;
		SysTick->CTRL=0x00000004;	//ȡ��Systick��ʹ��
		if(RxFinish) break;
	}
	if(RxFinish) return 1;
	else return 0;
}


void CBUS::Print(char *data){
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

void CBUS::Tx(u8 data){
	while(!(USART3->SR&(0x01<<7)));//������û�д�TDR�Ĵ�����ת�Ƶ���λ�Ĵ�������ѭ���ȴ�
	USART3->DR = data;//������ת������λ�Ĵ���
}
