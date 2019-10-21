#include "LCD.h"
#include "ADC.h"
#include "HCON.h"
#include "CCON.h"


LCD Lcd;

void LCD::Init(){
	//RX-PA10 TX-PA9
	RCC->APB2ENR |= 0x01<<2;
	RCC->APB2ENR |= 0x01<<14;
	GPIOA->CRH &= 0xFFFFF00F;
	GPIOA->CRH |= 0x000004B0;
	USART1->CR2 = 0x0000;
	USART1->CR3 = 0x0000;
	USART1->GTPR = 0x0000;
	USART1->BRR = 72000000/375000; //������375K
	USART1->CR1 = 0x01<<13;//����ʹ��
//	USART1->CR1 |= 0x01<<5; //�����ж�
//	USART1->CR1 |= 0x01<<7; //�����ж�
	NVIC->IP[37] = 0x10; //���ȼ�1
	NVIC->ISER[1] |= 0x01<<5; //�����ж���
	USART1->CR1 |= 0x01<<2; //����ʹ��
	USART1->CR1 |= 0x01<<3; //����ʹ��
}

void LCD::SentStatus(){
	Tx('%'); //��ʼ��
	Tx_num(Analog_Input.GetTemperatureExtruderA()/100); //������A��ǰ�¶�
	Tx_num(Analog_Input.GetTemperatureExtruderB()/100); //������B��ǰ�¶�
	Tx_num(Analog_Input.GetTemperatureBasePlane()/100); //��׼�浱ǰ�¶�
	Tx_num(HeaterControler.GetExtruderASetting()); //������A�趨�¶�
	Tx_num(HeaterControler.GetExtruderBSetting()); //������B�趨�¶�
	Tx_num(HeaterControler.GetBasePlaneSetting()); //��׼���趨�¶�
	Tx_num(HeaterControler.GetPowerHeaterExtruderA()); //������A����������
	Tx_num(HeaterControler.GetPowerHeaterExtruderB()); //������B����������
	Tx_num(HeaterControler.GetPowerHeaterBasePlane()); //��׼�����������
	Tx_num(CoolerControler.GetPowerCoolerExtruderA()); //������A��ȴ������
	Tx_num(CoolerControler.GetPowerCoolerExtruderB()); //������B��ȴ������
	Tx_num(Analog_Input.GetTemperatureEnvironment()/100); //�����¶�
	Tx((u8)HeaterControler.GetStatusExtruderA()); //������A״̬
	Tx((u8)HeaterControler.GetStatusExtruderB()); //������B״̬
	Tx((u8)HeaterControler.GetStatusBasePlane()); //��׼��״̬
	Tx('N');
}

void LCD::Print(u8 *data){
	for(u8 i=0;i<15;i++){ //��಻����16�ַ�
		if(data[i]=='\n' || !data[i]){ //�ǽ�����
			return; //ֹͣ����
		}
		else //�ǽ�����
			Tx(data[i]); //����һ���ַ�
	}
}

void LCD::Tx(u8 data){
	while(!(USART1->SR&(0x01<<7)));
	USART1->DR = data;
}

void LCD::Tx_num(u32 data){
	static u32 weight[] = {1,1,10,100};
	u8 buf[4] = {};	
	for(u8 i=0;i<3;i++)
		buf[i] = data/weight[3-i] % 10 + '0'; //ת��
	buf[3] = 0; //������
	Print(buf); //����
}
