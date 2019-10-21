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
	if(USART1->SR & 0x01) //У�����
		Serial.VerifyError = 1;
	if(USART1->SR & 0x01<<5){ //��ʼ
		u8 Temp = USART1->DR;
		if(Temp == '%'){ //��ʼ
			Serial.Buffer[0] = Temp;
			Serial.DataCounter = 1;
		}
		else if(Temp != '^'){ //����
			if(Serial.DataCounter < 256) //����δ���
				Serial.Buffer[Serial.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������
			if(Serial.TestAddress()){ //ȷ�ϵ�ַ
				Serial.RxFinish = 1; //���ý�����־
				Serial.DataNumber = Serial.DataCounter;
				for(u32 i=0;i<256;i++)
					Serial.RxData[i] = Serial.Buffer[i];
			}
			else{ //��ַ����
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
  USART1->BRR = 72000000/375000; //������375K
  USART1->CR1 |= 0x01<<13;//����ʹ��
  USART1->CR1 |= 0x01<<5; //�����ж�
	USART1->CR1 |= 0x01<<12; //9������λ
	USART1->CR1 |= 0x01<<9; //��У��
	USART1->CR1 |= 0x01<<8; //����У���ж�
	USART1->CR1 |= 0x01<<10; //����У��
  NVIC->IP[37] = 0x00; //���ȼ�0
  NVIC->ISER[1] |= 0x01<<5; //�����ж���
  USART1->CR1 |= 0x01<<2; //����ʹ��
  USART1->CR1 |= 0x01<<3; //����ʹ��
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
	
	if(Compare((char*)(RxData+4),"GETCOM")){ //��ȡ����ָ��
		if(Spio.HangUpFlag)
			Print(Spio.ComBuffer);
		else
			Print("");
	}
	else if(Compare((char*)(RxData+4),"SETSTA")){ //����״̬��ָ��
		for(u32 i=0;i<37 && RxData[10+i]!='^';i++)
			Status.StateBar[i] = RxData[10+i];
		Status.StateBar[37] = 0;
		Spout.ChangeStatusBar();
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"SETINF")){ //����״̬��ʾ��ָ��
		Status.SetInfo(RxData+10,DataNumber-10);
		Spout.InfoRefresh();
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"SETDAT")){ //����̨��ʾָ��������
		RxData[DataNumber] = 0;
//		Spout.Display("\n");
		Spout.Display((char*)(RxData+10));
		Spio.ProcessedFlag = 1;
		Spio.HangUpFlag = 0;
		Print(STRING_DONE);		
	}
	else if(Compare((char*)(RxData+4),"HLDDAT")){ //����̨��ʾָ����ֹ���
		RxData[DataNumber] = 0;
//		Spout.Display("\n");
		Spout.Display((char*)(RxData+10));
		Spio.HangUpFlag = 1;
		TIM4->CNT = 50000;
		Print(STRING_DONE);		
	}
	else if(Compare((char*)(RxData+4),"WAITIN")){ //����̨����ָ��
		Spio.HangUpFlag = 1;
		TIM4->CNT = 50000;
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"ADDDIC")){ //�ֵ�����ָ��
		RxData[DataNumber] = 0;
		if(DataNumber > 10)
			Spio.AddWord((char*)(RxData+10));
		Print(STRING_DONE);
	}
	else if(Compare((char*)(RxData+4),"GETSWA")){ //����A״̬���ָ��
		if(GPIOA->IDR & (0x01<<6))
			Print(STRING_OFF);
		else
			Print(STRING_ON);
	}
	else if(Compare((char*)(RxData+4),"GETSWB")){ //����B״̬���ָ��
		if(GPIOA->IDR & (0x01<<7))
			Print(STRING_OFF);
		else
			Print(STRING_ON);
	}
	else if(Compare((char*)(RxData+4),"TSTLIK")){ //ͨѶ���ָ��
		Print(STRING_DONE);
	}
	else{ //�޷�ʶ��
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	Tx('%'); //��ʼ��
	for(u8 i=0;i<256;i++){ //��಻����256�ַ�
		if(data[i]=='^' || !data[i]){ //�ǽ�����
			Tx('^'); //���ͽ�����
			return; //ֹͣ����
		}
		else //�ǽ�����
			Tx((u8)data[i]); //����һ���ַ�
	}
}

void COM::Tx(u8 data){
	while(!(USART1->SR&(0x01<<7)));
	USART1->DR = data;
}

u32 COM::Chr2Int(u8 *Data){
	static u32 weight[] = {1,1,10,100};
	u32 Value = 0;
	for(u8 i=0;i<3;i++) //ת��Ϊ����
		Value += (Data[i]-'0')*weight[3-i];
	return Value;
}

bool COM::TestAddress(){
	u32 C = 0;
	if(Buffer[1] == 'C') C++;
	if(Buffer[2] == 'C') C++;
	if(Buffer[3] == 'C') C++;
	
	if(C >= 2) return 1; //����ɹ�
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
