#include "COM.h"
#include "Status.h"
#include "LCD.h"

COM Serial;

extern"C"void USART1_IRQHandler(void){
	u8 Temp = USART1->DR;		
	if(Temp == '%'){ //��ʼ��
		Serial.TempBuffer[0] = Temp;
		Serial.DataCounter = 1;
	}
	else if(Serial.DataCounter < 64){ //����
		Serial.TempBuffer[Serial.DataCounter] = Temp;
		Serial.DataCounter++;
		if(Temp == '^'){ //������
			if(Serial.TestAddress()){ //��ַ���
				for(u32 i=0;i<64;i++)
					Serial.Buffer[i] = Serial.TempBuffer[i];
				Serial.RxFinish = 1; //���ý�����ɱ�־
				Serial.DataNumber = Serial.DataCounter; //��¼������
			}
			Serial.DataCounter = 0; //���ý��ռ�����
		}
	}
}

void COM::Init(){
	RxFinish = 0;
	DataCounter = 0;
	DataNumber = 0;
  RCC->APB2ENR |= 0x01<<2; //����PAʱ��
  RCC->APB2ENR |= 0x01<<14; //����USART1ʱ��
  GPIOA->CRH &= 0xFFFFF00F; //PA9:10�������
  GPIOA->CRH |= 0x000004F0; //PA9����ΪAFOD��PA10����ΪFIN
  USART1->CR2 = 0x0000; //���CR2�Ĵ���
  USART1->CR3 = 0x0000; //���CR3�Ĵ���
  USART1->GTPR = 0x0000; //���GTPR�Ĵ���
  USART1->BRR = 0xC0; //������375K Baud
  USART1->CR1 = 0x01<<13;//�շ���ʹ��
  USART1->CR1 |= 0x01<<5; //ʹ�ܽ����ж�
//	USART1->CR1 |= 0x01<<7; //ʹ�ܷ����ж�
  NVIC->IP[37] = 0x00; //�ж����ȼ�0
  NVIC->ISER[1] |= 0x01<<5; //�����ж���
  USART1->CR1 |= 0x01<<2; //����ʹ��
  USART1->CR1 |= 0x01<<3; //����ʹ��
}

void COM::Decoder(){
	if(DataNumber < 6) return;
	
	Status.DisableCursor();
	if(Buffer[5] == 'R'){ //����
		LCDInterface.Init();
		Status.Init();
	}
	else if(Buffer[5] == 'C'){ //����
		Status.Clear();
	}
	else if(Buffer[5] == 'S'){ //�ı���״̬
		Status.SetCursorStatus(Buffer[4]);
	}
	else if(Buffer[5] == 'A'){ //׷���ַ�
		Status.AddChar(Buffer[4]);
	}
	else if(Buffer[5] == 'B'){ //�޸�״̬��
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
