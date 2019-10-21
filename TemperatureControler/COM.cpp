#include "COM.h"
#include "HCON.h"
#include "CCON.h"
#include "ADC.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";

extern"C"void USART3_IRQHandler(){
	if(USART3->SR & 0x01) //校验错误
		Serial.VerifyError = 1;
	if(USART3->SR & 0x01<<5){
		u8 Temp = USART3->DR;
		if(Temp == '%'){ //起始
			Serial.TempBuffer[0] = Temp;
			Serial.DataCounter = 1;
		}
		else if(Temp != '^'){ //数据
			if(Serial.DataCounter < 24) //缓存未溢出
				Serial.TempBuffer[Serial.DataCounter++] = Temp; //记录数据
		}
		else if(Temp == '^'){ //结束符
			if(Serial.TestAddress()){
				Serial.TempBuffer[Serial.DataCounter++] = Temp;
				Serial.RxFinish = 1; //设置结束标志
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
  USART3->BRR = 36000000/375000; //波特率375K
  USART3->CR1 |= 0x01<<13;//外设使能
  USART3->CR1 |= 0x01<<5; //接收中断
//  USART3->CR1 |= 0x01<<7; //发送中断
	USART3->CR1 |= 0x01<<12; //9个数据位
	USART3->CR1 |= 0x01<<9; //奇校验
	USART3->CR1 |= 0x01<<8; //开启校验中断
	USART3->CR1 |= 0x01<<10; //开启校验
  NVIC->IP[39] = 0x00; //优先级0
  NVIC->ISER[1] |= 0x01<<7; //开放中断线
  USART3->CR1 |= 0x01<<2; //接收使能
  USART3->CR1 |= 0x01<<3; //发送使能
}

void COM::Decoder(){
	if(!RxFinish) return;
	
	if(VerifyError){ //校验错误处理
		Print(STRING_ERROR);
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	
	u32 Length = 0;
	while(TempBuffer[Length]!='^' && Length<24)
		Length++;
	
	Serial.RxFinish = 0; //清除接收完成标志
	if(Buffer[4] == 'G'){ //获取当前温度指令
		if(Buffer[5] == 'A') //挤出机A
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
	else if(Buffer[4] == 'S'){ //设置温度指令
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
	else if(Buffer[4] == 'T'){ //获取设定温度指令
		if(Buffer[5] == 'A') //挤出机A
			Tx_num(HeaterControler.GetExtruderASetting());
		else if(Buffer[5] == 'B')
			Tx_num(HeaterControler.GetExtruderBSetting());
		else if(Buffer[5] == 'G')
			Tx_num(HeaterControler.GetBasePlaneSetting());
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'C'){ //设置冷却器功率指令
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
	else if(Buffer[4] == 'R'){ //获取冷却器功率指令
		if(Buffer[5] == 'A')
			Tx_num(CoolerControler.GetPowerCoolerExtruderA());
		else if(Buffer[5] == 'B')
			Tx_num(CoolerControler.GetPowerCoolerExtruderB());
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'H'){ //获取加热器功率指令
		if(Buffer[5] == 'A')
			Tx_num(HeaterControler.GetPowerHeaterExtruderA());
		else if(Buffer[5] == 'B')
			Tx_num(HeaterControler.GetPowerHeaterExtruderB());
		else if(Buffer[5] == 'G')
			Tx_num(HeaterControler.GetPowerHeaterBasePlane());
		else
			Print(STRING_NONE);
	}
	else if(Buffer[4] == 'E'){ //获取设备状态指令
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
	else if(Buffer[4] == 'L'){ //通信测试
		Print(STRING_DONE);
	}
	else{ //无法识别
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	Tx('%');
	for(u8 i=0;i<15;i++){ //最多不超过16字符
		if(data[i]=='\n' || !data[i]){ //是结束符
			Tx('^'); //发送结束符
			return; //停止发送
		}
		else //非结束符
			Tx((u8)data[i]); //发送一个字符
	}
	Tx('^'); //发送结束符
}

void COM::Tx(u8 data){
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = data;
}


u32 COM::C2Q(u8 *data){
	static u32 weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	u32 value = 0;
	for(u8 i=0;i<3;i++) //转换为数字
		value += (data[i]-'0')*weight[3-i];
	return value;
}

void COM::Tx_num(u32 data){
	static u32 weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	u8 buf[4] = {};	
	for(u8 i=0;i<3;i++)
		buf[i] = data%weight[3-i+1]/weight[3-i] + '0'; //转码
	buf[3] = 0; //结束符
	Print((char*)buf); //发送
}

bool COM::TestAddress(){
	u32 C = 0;
	if(TempBuffer[1] == 'T') C++;
	if(TempBuffer[2] == 'T') C++;
	if(TempBuffer[3] == 'T') C++;
	
	if(C >= 2) return 1;

	return 0;
}
