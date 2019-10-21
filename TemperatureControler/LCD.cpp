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
	USART1->BRR = 72000000/375000; //波特率375K
	USART1->CR1 = 0x01<<13;//外设使能
//	USART1->CR1 |= 0x01<<5; //接收中断
//	USART1->CR1 |= 0x01<<7; //发送中断
	NVIC->IP[37] = 0x10; //优先级1
	NVIC->ISER[1] |= 0x01<<5; //开放中断线
	USART1->CR1 |= 0x01<<2; //接收使能
	USART1->CR1 |= 0x01<<3; //发送使能
}

void LCD::SentStatus(){
	Tx('%'); //起始符
	Tx_num(Analog_Input.GetTemperatureExtruderA()/100); //挤出机A当前温度
	Tx_num(Analog_Input.GetTemperatureExtruderB()/100); //挤出机B当前温度
	Tx_num(Analog_Input.GetTemperatureBasePlane()/100); //基准面当前温度
	Tx_num(HeaterControler.GetExtruderASetting()); //挤出机A设定温度
	Tx_num(HeaterControler.GetExtruderBSetting()); //挤出机B设定温度
	Tx_num(HeaterControler.GetBasePlaneSetting()); //基准面设定温度
	Tx_num(HeaterControler.GetPowerHeaterExtruderA()); //挤出机A加热器功率
	Tx_num(HeaterControler.GetPowerHeaterExtruderB()); //挤出机B加热器功率
	Tx_num(HeaterControler.GetPowerHeaterBasePlane()); //基准面加热器功率
	Tx_num(CoolerControler.GetPowerCoolerExtruderA()); //挤出机A冷却器功率
	Tx_num(CoolerControler.GetPowerCoolerExtruderB()); //挤出机B冷却器功率
	Tx_num(Analog_Input.GetTemperatureEnvironment()/100); //环境温度
	Tx((u8)HeaterControler.GetStatusExtruderA()); //挤出机A状态
	Tx((u8)HeaterControler.GetStatusExtruderB()); //挤出机B状态
	Tx((u8)HeaterControler.GetStatusBasePlane()); //基准面状态
	Tx('N');
}

void LCD::Print(u8 *data){
	for(u8 i=0;i<15;i++){ //最多不超过16字符
		if(data[i]=='\n' || !data[i]){ //是结束符
			return; //停止发送
		}
		else //非结束符
			Tx(data[i]); //发送一个字符
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
		buf[i] = data/weight[3-i] % 10 + '0'; //转码
	buf[3] = 0; //结束符
	Print(buf); //发送
}
