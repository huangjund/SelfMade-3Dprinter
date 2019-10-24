#include "UART.h"
#include "File.h"
#include "Card.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";

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
			if(Serial.DataCounter < 160) //缓存未溢出
				Serial.Buffer[Serial.DataCounter++] = Temp; //记录数据
		}
		else if(Temp == '^'){ //结束符
			if(Serial.TestAddress()){ //确认地址
				Serial.RxFinish = 1; //设置结束标志
				Serial.DataNumber = Serial.DataCounter;
				for(u32 i=0;i<160;i++)
					Serial.RxData[i] = Serial.Buffer[i];
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
	
	if(VerifyError){ //校验错误处理
		Print("",'R');
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	
	TIM2->ARR = 0;
	
	Serial.RxFinish = 0; //清除接收完成标志
	
	if(Compare((char*)(RxData+4),"RCAVOL")){ //获取存储卡容量
		u8 Volume = Card.GetVolume();
		u8 Temp[4] = {0};
		NumberToChar(3,Volume,Temp);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"RCRVOL")){ //获取剩余容量
		u8 Temp[11] = {0};
		NumberToChar(10,File.GetRestVolume(),Temp);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"RFNAME")){ //获取文件名
		if(File.FileInfo.Used){
			u8 Temp[32] = {0};
			File.GetFileName(Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("",'D');
	}
	else if(Compare((char*)(RxData+4),"RFSIZE")){ //获取文件大小
		if(File.FileInfo.Used){
			u8 Temp[11] = {0};
			NumberToChar(10,File.GetFileSize(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("0000000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFCOMS")){ //获取指令数
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.GetFileCommandsNumber(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFESTM")){ //获取预计时间
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.GetFileEstimatedTime(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFADIS")){ //获取总行程
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.GetFileTotalDistance(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF1")){ //获取保留块#1
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_1,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF2")){ //获取保留块#2
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_2,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF3")){ //获取保留块#3
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_3,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF4")){ //获取保留块#4
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_4,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF5")){ //获取保留块#5
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_5,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF6")){ //获取保留块#6
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_6,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"FIOPEN")){ //文件打开
		if(File.FileOpen())
			Print("",'D');
		else
			Print("",'K');
	}
	else if(Compare((char*)(RxData+4),"FITEST")){ //文件检测
		if(File.FileInfo.Used)
			Print("",'D');
		else
			Print("",'K');
	}

/*	else if(Compare((char*)(RxData+4),"FRFEAH")){ //文件刷新
		File.FileRefreash();
		Print(STRING_DONE);		
	}
	else if(Compare((char*)(RxData+4),"FITEST")){ //文件检测
		if(File.FileTest())
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FCREAT")){ //文件创建
		if(File.FileCreate(RxData+10))
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FDELET")){ //文件删除
		File.FileDelete();
		Print("DDD");
	}*/
	else if(Compare((char*)(RxData+4),"BFWRIT")){ //缓存写入
		u8 Index = CharToNumber(2,RxData+10);
		u32 Offset = Index*128;
		if(Offset > 3967)
			Print("",'K');
		else{
			for(u32 i=0;i<128;i++)
				WriteTemp[i+Offset] = RxData[i+12];
			Print("",'D');
		}
	}
	else if(Compare((char*)(RxData+4),"BFREAD")){ //缓存读取
		u8 Index = CharToNumber(2,RxData+10);
		u32 Offset = Index*128;
		if(Offset > 3967)
			Print("",'D');
		else{
			u8 Temp[136] = {0};
			for(u32 i=0;i<128;i++)
				Temp[i] = WriteTemp[Offset+i];
			Print((char*)Temp,'D');
		}
	}
	else if(Compare((char*)(RxData+4),"CSWRIT")){ //扇区写入
		u32 Index = CharToNumber(8,RxData+10);
		if(File.WriteSector(Index,WriteTemp))
			Print("",'D');
		else
			Print("",'K');
	}
	else if(Compare((char*)(RxData+4),"CSRDHP")){ //半页读取
		u32 Index = CharToNumber(8,RxData+10);
		u32 Address = Index*128;
		u8 Temp[136] = {0};
		File.ReadData(Address,Temp,128);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"FIREAD")){ //文件读取
		u8 Length = CharToNumber(2,RxData+10);
		u8 Temp[128] = {0};
		if(File.FileRead(Temp,Length))
			Print((char*)Temp,'D');
		else
			Print("",'D');
	}
	else if(Compare((char*)(RxData+4),"FREADL")){ //文件读取行
		u8 Temp[128] = {0};
		if(File.FileReadLine(Temp))
			Print((char*)Temp,'D');
		else
			Print("",'D');
	}
	else if(Compare((char*)(RxData+4),"RFPITR")){ //获取指针
		u8 Temp[11] = {0};
		NumberToChar(10,File.FileInfo.Pointer,Temp);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"FPSEEK")){ //移动指针
		s32 Offset = CharToNumber(10,RxData+11);
		if(RxData[10] == 'B')
			Offset *= -1;
		if(File.FileSeek(Offset))
			Print("",'D');
		else
			Print("",'K');
	}
/*	else if(Compare((char*)(RxData+4),"FSNAME")){ //设置文件名
		if(File.SetFileName(RxData+10))
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FREADL")){ //设置指令数
		u32 Number = CharToNumber(8,RxData+10);
		if(File.SetFileCommandsNumber(Number))
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FREADL")){ //设置文件大小
		u32 Number = CharToNumber(8,RxData+10);
		if(File.SetFileSize(Number))
			Print("DDD");
		else
			Print("KKK");
	}*/
	else if(Compare((char*)(RxData+4),"LKTEST")){ //通信测试
			Print("",'D');
	}
	else{ //无法识别
		Print("",'N');
	}
	TIM2->ARR = 9999;
}

void COM::Print(char *data,char State){
	Tx('%'); //起始符
	Tx(State);
	Tx(State);
	Tx(State);
	for(u8 i=0;i<256;i++){ //最多不超过256字符
		if(data[i]=='^' || !data[i]){ //是结束符
			Tx('^'); //发送结束符
			return; //停止发送
		}
		else //非结束符
			Tx((u8)data[i]); //发送一个字符
	}
}

void COM::Tx(u8 data){
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = data;
}

u32 COM::CharToNumber(u8 InLength,u8 *Data){
	if(InLength > 10) InLength = 10;
	
	const u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	
	u64 Value = 0;
	for(u8 i=0;i<InLength;i++)
		Value += (Data[i]-'0')*Weight[InLength-i];
	
	return Value;
}

u8* COM::NumberToChar(u8 OutLength,u32 Data,u8 *Str){
	const u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	
	u8 *Ptr = Str;
	for(u8 i=0;i<OutLength;i++)
		*(Ptr++) = Data%Weight[OutLength-i+1]/Weight[OutLength-i] + '0';
	return Ptr;
}


bool COM::TestAddress(){
	u32 C = 0;
	if(Buffer[1] == 'F') C++;
	if(Buffer[2] == 'F') C++;
	if(Buffer[3] == 'F') C++;
	
	if(C >= 2) return 1; //检验成功
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

