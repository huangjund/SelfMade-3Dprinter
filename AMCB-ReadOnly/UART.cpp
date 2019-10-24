#include "UART.h"
#include "File.h"
#include "Card.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";

extern"C"void USART3_IRQHandler(){
	if(USART3->SR & 0x01) //У�����
		Serial.VerifyError = 1;
	if(USART3->SR & 0x01<<5){ //��ʼ
		u8 Temp = USART3->DR;
		if(Temp == '%'){ //��ʼ
			Serial.Buffer[0] = Temp;
			Serial.DataCounter = 1;
		}
		else if(Temp != '^'){ //����
			if(Serial.DataCounter < 160) //����δ���
				Serial.Buffer[Serial.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������
			if(Serial.TestAddress()){ //ȷ�ϵ�ַ
				Serial.RxFinish = 1; //���ý�����־
				Serial.DataNumber = Serial.DataCounter;
				for(u32 i=0;i<160;i++)
					Serial.RxData[i] = Serial.Buffer[i];
			}
			else{ //��ַ����
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
	if(!RxFinish) return; //δ�������
	
	if(VerifyError){ //У�������
		Print("",'R');
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	
	TIM2->ARR = 0;
	
	Serial.RxFinish = 0; //���������ɱ�־
	
	if(Compare((char*)(RxData+4),"RCAVOL")){ //��ȡ�洢������
		u8 Volume = Card.GetVolume();
		u8 Temp[4] = {0};
		NumberToChar(3,Volume,Temp);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"RCRVOL")){ //��ȡʣ������
		u8 Temp[11] = {0};
		NumberToChar(10,File.GetRestVolume(),Temp);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"RFNAME")){ //��ȡ�ļ���
		if(File.FileInfo.Used){
			u8 Temp[32] = {0};
			File.GetFileName(Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("",'D');
	}
	else if(Compare((char*)(RxData+4),"RFSIZE")){ //��ȡ�ļ���С
		if(File.FileInfo.Used){
			u8 Temp[11] = {0};
			NumberToChar(10,File.GetFileSize(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("0000000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFCOMS")){ //��ȡָ����
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.GetFileCommandsNumber(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFESTM")){ //��ȡԤ��ʱ��
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.GetFileEstimatedTime(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFADIS")){ //��ȡ���г�
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.GetFileTotalDistance(),Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF1")){ //��ȡ������#1
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_1,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF2")){ //��ȡ������#2
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_2,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF3")){ //��ȡ������#3
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_3,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF4")){ //��ȡ������#4
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_4,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF5")){ //��ȡ������#5
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_5,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"RFINF6")){ //��ȡ������#6
		if(File.FileInfo.Used){
			u8 Temp[9] = {0};
			NumberToChar(8,File.FileInfo.Reserved_6,Temp);
			Print((char*)Temp,'D');
		}
		else
			Print("00000000",'D');
	}
	else if(Compare((char*)(RxData+4),"FIOPEN")){ //�ļ���
		if(File.FileOpen())
			Print("",'D');
		else
			Print("",'K');
	}
	else if(Compare((char*)(RxData+4),"FITEST")){ //�ļ����
		if(File.FileInfo.Used)
			Print("",'D');
		else
			Print("",'K');
	}

/*	else if(Compare((char*)(RxData+4),"FRFEAH")){ //�ļ�ˢ��
		File.FileRefreash();
		Print(STRING_DONE);		
	}
	else if(Compare((char*)(RxData+4),"FITEST")){ //�ļ����
		if(File.FileTest())
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FCREAT")){ //�ļ�����
		if(File.FileCreate(RxData+10))
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FDELET")){ //�ļ�ɾ��
		File.FileDelete();
		Print("DDD");
	}*/
	else if(Compare((char*)(RxData+4),"BFWRIT")){ //����д��
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
	else if(Compare((char*)(RxData+4),"BFREAD")){ //�����ȡ
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
	else if(Compare((char*)(RxData+4),"CSWRIT")){ //����д��
		u32 Index = CharToNumber(8,RxData+10);
		if(File.WriteSector(Index,WriteTemp))
			Print("",'D');
		else
			Print("",'K');
	}
	else if(Compare((char*)(RxData+4),"CSRDHP")){ //��ҳ��ȡ
		u32 Index = CharToNumber(8,RxData+10);
		u32 Address = Index*128;
		u8 Temp[136] = {0};
		File.ReadData(Address,Temp,128);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"FIREAD")){ //�ļ���ȡ
		u8 Length = CharToNumber(2,RxData+10);
		u8 Temp[128] = {0};
		if(File.FileRead(Temp,Length))
			Print((char*)Temp,'D');
		else
			Print("",'D');
	}
	else if(Compare((char*)(RxData+4),"FREADL")){ //�ļ���ȡ��
		u8 Temp[128] = {0};
		if(File.FileReadLine(Temp))
			Print((char*)Temp,'D');
		else
			Print("",'D');
	}
	else if(Compare((char*)(RxData+4),"RFPITR")){ //��ȡָ��
		u8 Temp[11] = {0};
		NumberToChar(10,File.FileInfo.Pointer,Temp);
		Print((char*)Temp,'D');
	}
	else if(Compare((char*)(RxData+4),"FPSEEK")){ //�ƶ�ָ��
		s32 Offset = CharToNumber(10,RxData+11);
		if(RxData[10] == 'B')
			Offset *= -1;
		if(File.FileSeek(Offset))
			Print("",'D');
		else
			Print("",'K');
	}
/*	else if(Compare((char*)(RxData+4),"FSNAME")){ //�����ļ���
		if(File.SetFileName(RxData+10))
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FREADL")){ //����ָ����
		u32 Number = CharToNumber(8,RxData+10);
		if(File.SetFileCommandsNumber(Number))
			Print("DDD");
		else
			Print("KKK");
	}*/
/*	else if(Compare((char*)(RxData+4),"FREADL")){ //�����ļ���С
		u32 Number = CharToNumber(8,RxData+10);
		if(File.SetFileSize(Number))
			Print("DDD");
		else
			Print("KKK");
	}*/
	else if(Compare((char*)(RxData+4),"LKTEST")){ //ͨ�Ų���
			Print("",'D');
	}
	else{ //�޷�ʶ��
		Print("",'N');
	}
	TIM2->ARR = 9999;
}

void COM::Print(char *data,char State){
	Tx('%'); //��ʼ��
	Tx(State);
	Tx(State);
	Tx(State);
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

