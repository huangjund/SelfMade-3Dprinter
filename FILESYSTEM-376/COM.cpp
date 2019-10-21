#include "COM.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_SKIP[] = "KKK";
char STRING_FULL[] = "FFF";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";
char STRING_MOVE[] = "MMM";

extern"C"void USART2_IRQHandler(){
	if(USART2->SR & 0x01) //У�����
		Serial.VerifyError = 1; //����У������־
	if(USART2->SR & (0x01<<5)){ //��ʼ
		u8 Temp = USART2->DR; 
		if(Temp == '%'){ //��ʼ��(��ʼ����)
			Serial.Buffer[0] = Temp; //д��L1����
			Serial.DataCounter = 1; //���ý���������
		}
		else if(Temp != '^'){ //����(���ڽ���)
			if(Serial.DataCounter < 128) //����δ���
				Serial.Buffer[Serial.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������(�������)
			if(Serial.TestAddress()){ //ȷ�ϵ�ַ���
				Serial.RxFinish = 1; //���ý�����־
				for(u32 i=0;i<128;i++) //����д��L2����
					Serial.RxData[i] = Serial.Buffer[i];
			}
			else{ //��ַ����
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

void COM::Decoder(){
	if(!RxFinish) return; //δ�������
	
	if(VerifyError){ //У�������
		Print(STRING_ERROR);
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	Serial.RxFinish = 0; //���������ɱ�־
	if(RxData[4] == 'M'){ //�ƶ�ָ��
	}
	else{
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	for(u8 i=0;i<16;i++){ //��಻����16�ַ�
		if(data[i]=='^' || !data[i]){ //�ǽ�����
			Tx('^'); //���ͽ�����
			return; //ֹͣ����
		}
		else //�ǽ�����
			Tx((u8)data[i]); //����һ���ַ�
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
	
	if(C >= 2) return 1; //����ɹ�
	return 0;
}

