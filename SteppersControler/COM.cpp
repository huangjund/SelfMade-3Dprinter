#include "COM.h"
#include "LCD.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_SKIP[] = "KKK";
char STRING_FULL[] = "FFF";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";
char STRING_MOVE[] = "MMM";

extern"C"void USART3_IRQHandler(){
	if(USART3->SR & 0x01) //校验错误
		Serial.VerifyError = 1;
	if(USART3->SR & 0x01<<5){ //起始
		u8 Temp = USART3->DR;
		if(Temp == '%'){ //起始
			Serial.Buffer[0] = Temp;
			Serial.DataCounter = 1;
		}
		else if(Temp != '^'){ //数据
			if(Serial.DataCounter < 128) //缓存未溢出
				Serial.Buffer[Serial.DataCounter++] = Temp; //记录数据
		}
		else if(Temp == '^'){ //结束符
			if(Serial.TestAddress()){ //确认地址
				Serial.RxFinish = 1; //设置结束标志
				for(u32 i=0;i<128;i++)
					Serial.RxData[i] = Serial.Buffer[i];
				Serial.DataNumber = Serial.DataCounter;
			}
			else{ //地址不符
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
	if(!RxFinish) return; //未接收完成
	
//	if(!TestAddress()){ //地址不符
//		Serial.RxFinish = 0;
//		Serial.VerifyError = 0;
//		return;
//	}
	
	if(VerifyError){ //校验错误处理
		Print(STRING_ERROR);
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	Serial.RxFinish = 0; //清除接收完成标志
	if(RxData[4] == 'M'){ //移动指令
		if(!Status.TestIfHomed()){ //尚未归位
			Print(STRING_SKIP);
		}
		else if(Planner.PtrLoader->Full){ //缓存满
			Print(STRING_FULL);
		}
		else{
			
			if(!Status.TestIfEnable())
				Stepper.EnableStepper();
			
			double Posi[5];
			Posi[0] = C2D(RxData+6);
			Posi[1] = C2D(RxData+19);
			Posi[2] = C2D(RxData+32);
			Posi[3] = LC2D(RxData+45);
			Posi[4] = LC2D(RxData+60);
			float Vmax;
			Vmax = C2F(RxData+75);
		
//			for(u32 i=0;i<5;i++)
//				Temp[i] = Posi[i];
//			V = Vmax;
			
			
			//(可以扩展为除缓存满以外的其他检测)
			if(Planner.Load(Posi,Vmax)) //装填完毕
				Print(STRING_DONE);
			else //装填失败
				Print(STRING_FULL);
		}
	}
	else if(RxData[4] == 'E'){ //分段移动指令
		if(Planner.L2RestAmount != L2_SIZE){ //L2缓存非空不可移动
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.L1RestAmount != L1_SIZE){ //L1缓存非空不可移动
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.FAStatus != 'I'){ //运行中不可移动
			Print(STRING_SKIP);
			return;
		}
		if(!Status.TestIfHomed()){
			Print("HHH");
			return;
		}
		if(!Status.TestIfEnable()){ //未使能先使能
			Stepper.EnableStepper();
		}
		
		Print(STRING_MOVE);
		
		double Dest = LC2D(RxData+6);
		if(RxData[5] == 'X')
			Stepper.SectionMove(Dest,0);
		else if(RxData[5] == 'Y')
			Stepper.SectionMove(Dest,1);
		else if(RxData[5] == 'Z')
			Stepper.SectionMove(Dest,2);
		else if(RxData[5] == 'A')
			Stepper.SectionMove(Dest,3);
		else if(RxData[5] == 'B')
			Stepper.SectionMove(Dest,4);
		
		Print(STRING_DONE);
	}
	else if(RxData[4] == 'I'){ //位置初始化指令
		double Posi[5];
		Posi[0] = C2D(RxData+6);
		Posi[1] = C2D(RxData+19);
		Posi[2] = C2D(RxData+32);
		Posi[3] = LC2D(RxData+45);
		Posi[4] = LC2D(RxData+60);
		if(Status.InitPosition(Posi))
			Print(STRING_DONE);
		else
			Print(STRING_SKIP);
	}
	else if(RxData[4] == 'H'){ //自动归位指令
		if(RxData[5] != 'H'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[6] != 'O'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[7] != 'M'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[8] != 'E'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[9] != 'X'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[10] != 'Y'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[11] != 'Z'){
			Print(STRING_NONE);
			return;
		}
		
		if(Planner.L2RestAmount != L2_SIZE){ //L2缓存非空不可归位
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.L1RestAmount != L1_SIZE){ //L1缓存非空不可归位
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.FAStatus != 'I'){ //运行中不可归位
			Print(STRING_SKIP);
			return;
		}
		if(!Status.TestIfEnable()){ //未使能不可归位
			Print(STRING_SKIP);
			return;
		}
		
		Print(STRING_MOVE);
		Stepper.AutoHoming();
		Print(STRING_DONE);
	}
	else if(RxData[4] == 'G'){ //读取指令
		switch(RxData[5]){
			case 'X':Tx_Decimal(Status.GetPosition(0));break; //X轴坐标
			case 'Y':Tx_Decimal(Status.GetPosition(1));break; //Y轴坐标
			case 'Z':Tx_Decimal(Status.GetPosition(2));break; //Z轴坐标
			case 'A':Tx_Decimal(Status.GetPosition(3));break; //A轴坐标
			case 'B':Tx_Decimal(Status.GetPosition(4));break; //B轴坐标
			case 'O':Tx_Decimal(AXIS_X_MAX_POSITION);break;
			case 'P':Tx_Decimal(AXIS_Y_MAX_POSITION);break;
			case 'Q':Tx_Decimal(AXIS_Z_MAX_POSITION);break;
			case 'S':
				if(!Status.TestIfEnable())
					Print("DDD");
				else if(!Status.TestIfHomed())
					Print("HHH");
				else{
					Tx('%'); //自动机状态
					Tx(Stepper.FAStatus);
					Tx(Stepper.FAStatus);
					Tx(Stepper.FAStatus);
					Tx('^');
				}
				break;
			case 'I': //IDP状态
				if(Status.TestIfIDP()) Print("EEE");
				else Print("DDD");
				break;
			
			case 'E': //电机状态
				if(Status.TestIfEnable()) Print("EEE");
				else Print("DDD");
				break;
			
			case 'H':Tx('%'); //复位状态
				if(Status.TestIfHomed()) Print("EEE");
				else Print("DDD");
				break;
			
			case '1':Tx_Integer(Stepper.L1RestAmount);break; //L1缓存剩余容量
			case '2':Tx_Integer(Planner.L2RestAmount);break; //L2缓存剩余容量
			case 'M':Tx_Integer(L1_SIZE);break; //L1缓存容量
			case 'N':Tx_Integer(L2_SIZE);break; //L2缓存容量
			case '3': //检测是否已经调平
				if(Status.TestIfLevel()) //已调平
					Print("EEE");
				else //未调平
					Print("DDD");
				break;
			case '4': //获取测试点A
				Tx_Decimal(Status.TestPoint[0]);
				break;
			case '5': //获取测试点B
				Tx_Decimal(Status.TestPoint[1]);
				break;
			case '6': //获取测试点C
				Tx_Decimal(Status.TestPoint[2]);
				break;
			case '7': //获取测试点D
				Tx_Decimal(Status.TestPoint[3]);
				break;			
			case '8': //获取测试点M
				Tx_Decimal(Status.TestPoint[4]);
				break;
			default:Print(STRING_NONE);break; //无法识别
		}
	}
	else if(RxData[4] == 'P'){ //坐标设置指令
		switch(RxData[5]){
			case 'X':Status.SetPosition(LC2D(RxData+6),0);Print(STRING_DONE);break; //X轴坐标
			case 'Y':Status.SetPosition(LC2D(RxData+6),1);Print(STRING_DONE);break; //Y轴坐标
			case 'Z':Status.SetPosition(LC2D(RxData+6),2);Print(STRING_DONE);break; //Z轴坐标
			case 'A':Status.SetPosition(LC2D(RxData+6),3);Print(STRING_DONE);break; //A轴坐标
			case 'B':Status.SetPosition(LC2D(RxData+6),4);Print(STRING_DONE);break; //B轴坐标
			default:Print(STRING_NONE);break; //无法识别
		}
	}
	else if(RxData[4] == 'S'){ //步进电机状态设置指令
		switch(RxData[5]){
			case 'E':Stepper.EnableStepper();Print(STRING_DONE);break; //启动步进电机并关闭制动器
			case 'D': //启动制动器并关闭步进电机
				if(Stepper.DisableStepper())
					Print(STRING_DONE);
				else 
					Print(STRING_SKIP);
				break;
			default:Print(STRING_NONE);break; //无法识别
		}
	}
	else if(RxData[4] == 'D'){ //IDP状态设置指令
		switch(RxData[5]){
			case 'E':Status.SetIDP(1);Print(STRING_DONE);break; //激活IDP
			case 'D':Status.SetIDP(0);Print(STRING_DONE);break; //禁用IDP
			default:Print(STRING_NONE);break; //无法识别
		}
	}
	else if(RxData[4] == 'T'){ //通信测试指令
//		if(RxData[5] == 'R') Lcd.RestartFlag = 1;
		Print("DDD");
	}
	else{
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	Tx('%');
	for(u8 i=0;i<24;i++){ //最多不超过16字符
		if(data[i]=='^' || !data[i]){ //是结束符
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

double COM::LC2D(u8 *Data){
	static s64 weight[] = {1,1,10,100,1000,10000,100000};
	s64 Integer = 0;
	s64 Decimal = 0;
	s64 Symbol = 1;
	
	if(*Data == '-') Symbol = -1;
	Data++;
	
	for(u8 i=0;i<6;i++) //整数部分
		Integer += (*(Data++)-'0')*weight[6-i];
	
	Data++; //小数点

	for(u8 i=0;i<6;i++) //小数部分
		Decimal += (*(Data++)-'0')*weight[6-i];
	
	double Value = (double)Symbol*((double)Integer+(double)Decimal/1000000.0);

	return Value;
}

double COM::C2D(u8 *Data){
	static s64 weight[] = {1,1,10,100,1000,10000,100000,1000000};
	s64 Integer = 0;
	s64 Decimal = 0;
	s64 Symbol = 1;
	
	if(*Data == '-') Symbol = -1;
	Data++;
	
	for(u8 i=0;i<4;i++) //整数部分
		Integer += (*(Data++)-'0')*weight[4-i];
	
	Data++; //小数点

	for(u8 i=0;i<6;i++) //小数部分
		Decimal += (*(Data++)-'0')*weight[6-i];
	
	double Value = (double)Symbol*((double)Integer+(double)Decimal/1000000.0);

	return Value;
}

float COM::C2F(u8 *Data){
	static u32 weight[] = {1,1,10,100};
	u32 Value = 0;
	for(u8 i=0;i<3;i++) //转换为数字
		Value += (Data[i]-'0')*weight[3-i];
	return Value*1.0f;
}

void COM::Tx_Decimal(double Data){
	u8 buffer[13] = {};
	u8 *Ptr = buffer;
	double TempFloat = Data;

	if(Data < 0){ //提取符号
		*(Ptr++) = '-';
		TempFloat = -TempFloat;
	}
	else *(Ptr++) = '+';
	
	u64 Integer = (u64)TempFloat; //提取整数
	u64 Decimal = (u64)(TempFloat*1000000.0); //提取小数
	
	static u64 weight[] = {1,1,10,100,1000,10000,100000,100000};
	
	for(u32 i=0;i<6;i++) //整数部分转码
		*(Ptr++) = Integer%weight[6-i+1]/weight[6-i] + '0';
	
	*(Ptr++) = '.'; //小数点
	
	for(u32 i=0;i<6;i++) //小数部分转码
		*(Ptr++) = Decimal%weight[6-i+1]/weight[6-i] + '0';
	
	*Ptr = 0; //结束符
	
Print((char*)buffer);
}

void COM::Tx_Integer(u32 Data){
	u8 buffer[4] = {};		
	static u32 weight[] = {1,1,10,100,1000};
	for(u32 i=0;i<3;i++)
		buffer[i] = Data%weight[3-i+1]/weight[3-i] + '0'; //转码
	buffer[3] = 0; //结束符
	Print((char*)buffer); //发送
}

bool COM::TestAddress(){
	u32 C = 0;
	if(Buffer[1] == 'S') C++;
	if(Buffer[2] == 'S') C++;
	if(Buffer[3] == 'S') C++;
	
	if(C >= 2) return 1; //检验成功
	return 0;
}
