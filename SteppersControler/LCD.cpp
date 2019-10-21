#include "LCD.h"
#include "StepperConfiguration.h"
#include "Stepper.h"
#include "Planner.h"
#include "Status.h"
LCD Lcd;




void LCD::Init(){
	
	//RX-PA3 TX-PA2
	RCC->APB2ENR |= 0x01<<2;
	RCC->APB1ENR |= 0x01<<17;
	GPIOA->CRL &= 0xFFFF00FF;
	GPIOA->CRL |= 0x00004B00;
	USART2->CR2 = 0x0000;
	USART2->CR3 = 0x0000;
	USART2->GTPR = 0x0000;
	USART2->BRR = 36000000/375000; //������375K
	USART2->CR1 = 0x01<<13; //����ʹ��
//	USART2->CR1 |= 0x01<<5; //�����ж�
//	USART2->CR1 |= 0x01<<7; //�����ж�
	NVIC->IP[38] = 0x10; //���ȼ�1
	NVIC->ISER[1] |= 0x01<<6; //�����ж���
//	USART2->CR1 |= 0x01<<2; //����ʹ��
//	USART2->CR1 |= 0x01<<3; //����ʹ��
	

  
	
	
	RestartFlag = 0;
}

void LCD::SentStatus(){
	static u32 PXMax = (u32)AXIS_X_MAX_POSITION;
	static u32 PYMax = (u32)AXIS_Y_MAX_POSITION;
	static u32 PZMax = (u32)AXIS_Z_MAX_POSITION;
	
	static u32 L1Max = L1_SIZE;
	static u32 L2Max = L2_SIZE;
	Tx('%'); //��ʼ��
	Tx_num((u32)Status.GetPosition(0)%1000); //X�ᵱǰ����
	Tx_num((u32)Status.GetPosition(1)%1000); //Y�ᵱǰ����
	Tx_num((u32)Status.GetPosition(2)%1000); //Z�ᵱǰ����
	Tx_num((u32)PXMax%1000); //X���������
	Tx_num((u32)PYMax%1000); //Y���������
	Tx_num((u32)PZMax%1000); //Z���������
	Tx_num(L1Max-Stepper.L1RestAmount); //L1��ʹ������
	Tx_num(L2Max-Planner.L2RestAmount); //L2��ʹ������
	Tx_num(L1Max); //L1�������
	Tx_num(L2Max); //L2�������
	
	if(!Status.TestIfEnable()) Tx('D'); //ʧ��״̬
	else if(!Status.TestIfHomed()) Tx('N'); //��δ��λ
	else Tx((u8)Stepper.FAStatus); //�Զ���״̬
	
	if(Status.TestIfIDP()) Tx('E'); //IDP����
	else Tx('D'); //IDP����
	
	if(RestartFlag){
		RestartFlag = 0;
		Tx('R');
	}
	else
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
	while(!(USART2->SR&(0x01<<7)));
	USART2->DR = data;
}

void LCD::Tx_num(u32 data){
	static u32 weight[] = {1,1,10,100};
	u8 buf[4] = {};	
	for(u8 i=0;i<3;i++)
		buf[i] = data/weight[3-i] % 10 + '0'; //ת��
	buf[3] = 0; //������
	Print(buf); //����
}

