#include "COM.h"
#include "HCON.h"
#include "CCON.h"
#include "ADC.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";

extern"C"void USART3_IRQHandler(){
	if(USART3->SR & 0x01) //У�����
		Serial.VerifyError = 1;
	if(USART3->SR & 0x01<<5){
		u8 Temp = USART3->DR;
		if(Temp == '%'){ //��ʼ
			Serial.TempBuffer[0] = Temp;
			Serial.DataCounter = 1;
		}
		else if(Temp != '^'){ //����
			if(Serial.DataCounter < 24) //����δ���
				Serial.TempBuffer[Serial.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������
			if(Serial.TestAddress()){
				Serial.TempBuffer[Serial.DataCounter++] = Temp;
				Serial.RxFinish = 1; //���ý�����־
				for(u32 i=0;i<24;i++)
					Serial.Buffer[i] = Serial.TempBuffer[i];
				Serial.DataNumber = Serial.DataCounter;
			}
			else{
				Serial.VerifyError = 0;
			}
		}
	}
}

void COM::Init(){
	//RX-PB11 TX-PB10
  RCC->APB2ENR |= 0x01<<3;
  RCC->APB1ENR |= 0x01<<18;
  GPIOB->CRH &= 0xFFFF00FF;
  GPIOB->CRH |= 0x00004F00;
  USART3->CR1 = 0x00;
  USART3->CR2 = 0x00;
  USART3->CR3 = 0x00;
  USART3->GTPR = 0x00;
  USART3->BRR = 36000000/375000; //������375K
  USART3->CR1 |= 0x01<<13;//����ʹ��
  USART3->CR1 |= 0x01<<5; //�����ж�
//  USART3->CR1 |= 0x01<<7; //�����ж�
	USART3->CR1 |= 0x01<<12; //9������λ
	USART3->CR1 |= 0x01<<9; //��У��
	USART3->CR1 |= 0x01<<8; //����У���ж�
	USART3->CR1 |= 0x01<<10; //����У��
  NVIC->IP[39] = 0x00; //���ȼ�0
  NVIC->ISER[1] |= 0x01<<7; //�����ж���
  USART3->CR1 |= 0x01<<2; //����ʹ��
  USART3->CR1 |= 0x01<<3; //����ʹ��
}

void COM::Decoder(){
	if(!RxFinish) return;
	
	if(VerifyError){ //У�������
		Print(STRING_ERROR);
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	
	u32 Length = 0;
	while(TempBuffer[Length]!='^' && Length<24)
		Length++;
	
	Serial.RxFinish = 0; //���������ɱ�־
	if(Buffer[4] == 'G'){ //��ȡ��ǰ�¶�ָ��
		if(Buffer[5] == 'A') //������A
			Tx_num(Analog_Input.GetTemperatureExtruderA()/100);
		else if(Buffer[5] == 'B')
			Tx_num(Analog_Input.GetTemperatureExtruderB()/100);
		else if(Buffer[5] == 'G')
			Tx_num(Analog_Input.GetTemperatureBasePlane()/100);
		else if(Buffer[5] == 'E')
			Tx_num(Analog_Input.GetTemperatureEnvironment()/100);
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'S'){ //�����¶�ָ��
		if(Buffer[5] == 'A'){
			HeaterControler.SetTemperatureExtruderA(C2Q(Buffer+6));
			Print(STRING_DONE);
		}
		else if(Buffer[5] == 'B'){
			HeaterControler.SetTemperatureExtruderB(C2Q(Buffer+6));
			Print(STRING_DONE);
		}
		else if(Buffer[5] == 'G'){
			HeaterControler.SetTemperatureBasePlane(C2Q(Buffer+6));
			Print(STRING_DONE);
		}
		else 
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'T'){ //��ȡ�趨�¶�ָ��
		if(Buffer[5] == 'A') //������A
			Tx_num(HeaterControler.GetExtruderASetting());
		else if(Buffer[5] == 'B')
			Tx_num(HeaterControler.GetExtruderBSetting());
		else if(Buffer[5] == 'G')
			Tx_num(HeaterControler.GetBasePlaneSetting());
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'C'){ //������ȴ������ָ��
		if(Buffer[5] == 'A'){
			CoolerControler.SetPowerCoolerExtruderA(C2Q(Buffer+6));
			Print(STRING_DONE);
		}
		else if(Buffer[5] == 'B'){
			CoolerControler.SetPowerCoolerExtruderB(C2Q(Buffer+6));
			Print(STRING_DONE);
		}
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'R'){ //��ȡ��ȴ������ָ��
		if(Buffer[5] == 'A')
			Tx_num(CoolerControler.GetPowerCoolerExtruderA());
		else if(Buffer[5] == 'B')
			Tx_num(CoolerControler.GetPowerCoolerExtruderB());
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'H'){ //��ȡ����������ָ��
		if(Buffer[5] == 'A')
			Tx_num(HeaterControler.GetPowerHeaterExtruderA());
		else if(Buffer[5] == 'B')
			Tx_num(HeaterControler.GetPowerHeaterExtruderB());
		else if(Buffer[5] == 'G')
			Tx_num(HeaterControler.GetPowerHeaterBasePlane());
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'E'){ //��ȡ�豸״ָ̬��
		if(Buffer[5] == 'A'){
			Tx('%');
			Tx((u8)HeaterControler.GetStatusExtruderA());
			Tx('^');
		}
		else if(Buffer[5] == 'B'){
			Tx('%');
			Tx((u8)HeaterControler.GetStatusExtruderB());
			Tx('^');
		}
		else if(Buffer[5] == 'G'){
			Tx('%');
			Tx((u8)HeaterControler.GetStatusBasePlane());
			Tx('^');
		}
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'L'){ //ͨ�Ų���
		Print(STRING_DONE);
	}
	else{ //�޷�ʶ��
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	Tx('%');
	for(u8 i=0;i<15;i++){ //��಻����16�ַ�
		if(data[i]=='\n' || !data[i]){ //�ǽ�����
			Tx('^'); //���ͽ�����
			return; //ֹͣ����
		}
		else //�ǽ�����
			Tx((u8)data[i]); //����һ���ַ�
	}
	Tx('^'); //���ͽ�����
}

void COM::Tx(u8 data){
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = data;
}


u32 COM::C2Q(u8 *data){
	static u32 weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	u32 value = 0;
	for(u8 i=0;i<3;i++) //ת��Ϊ����
		value += (data[i]-'0')*weight[3-i];
	return value;
}

void COM::Tx_num(u32 data){
	static u32 weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	u8 buf[4] = {};	
	for(u8 i=0;i<3;i++)
		buf[i] = data%weight[3-i+1]/weight[3-i] + '0'; //ת��
	buf[3] = 0; //������
	Print((char*)buf); //����
}

bool COM::TestAddress(){
	u32 C = 0;
	if(TempBuffer[1] == 'T') C++;
	if(TempBuffer[2] == 'T') C++;
	if(TempBuffer[3] == 'T') C++;
	
	if(C >= 2) return 1;

	return 0;
}
