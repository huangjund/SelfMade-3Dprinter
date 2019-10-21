#include "SPIO.h"
#include "SPOUT.h"
#include "SPIN.h"
#include "Status.h"
#include "COM.h"

SPIO Spio;

extern"C"void TIM4_IRQHandler(void){
	if(TIM4->SR&0x01){
		Spio.HangUpFlag = 0;		
		TIM4->SR&=~0x01;
	}
}


void SPIO::Init(){
	
	RCC->APB1ENR |= 0x01<<2;
	TIM4->ARR = 50000;//Ԥװ��
	TIM4->PSC = 7199;//��Ƶ
	TIM4->CR1 |= 0x01<<4; //���¼���
	TIM4->DIER |= 0x01<<0;
	NVIC->IP[30] = 0x30;//���ȼ�3
	NVIC->ISER[0] |= 0x01<<30;
	TIM4->CR1 |= 0x01<<0;//ʹ��

	HangUpFlag = 0;
	
	
	Spout.Init();
	Spin.Init();	
	Status.Init();	
	
	DelayMs(1000);	
	
	Spout.ConsoleRestart();
	Spout.InfoRestart();	
	
	DelayMs(1500);	
	
	Spout.InfoRefresh();	
	Spout.ChangeStatusBar();	
	
}

void SPIO::ReadIn(){
	Spout.ConsoleClear(); //����
	DelayMs(1000);
	Spout.Display(">>"); //��ʾ������ʾ��
	
	for(u32 i=0;i<256;i++) //��������
		ComBuffer[i] = 0;
	ComBufferIndex = 0;
	
	Spin.EnableRx(); //��������
	
	while(1){ //�ȴ�����
		Serial.Decoder(); //�����������
		
		u8 TempChar = Spin.Launch(); //��ȡ����
		if(!TempChar) continue; //��Ч�ַ�
		else if(TempChar == '\r'){ //������
			Spout.Display("\r"); //����
			break; //ֹͣ�ȴ�
		}
		else if(TempChar == '\t') Completion(); //��ȫ
		else Load(TempChar);
		
		
	}
	Spin.DisableRx(); //��ֹ����
	
	Decode();
	
	WaitForEnter();
}

void SPIO::Decode(){
	char *ANASTR = "Processing...";
	Spout.Display(ANASTR);
	Spout.ConsoleSetCursorBusy();	
	if(TestCommand()){ //���ش������
		ProcessedFlag = 1;
	}
	else{ //�����޷�����
		ProcessedFlag = 0;	//���������ϱ�־
		Sleep(50000);
	}
}

void SPIO::WaitForEnter(){	
	while(HangUpFlag){ //�ȴ��ظ�
		Serial.Decoder(); //�����������
	}
	
	if(!ProcessedFlag){ //δ����
		Spout.Display("\nHost Timeout.");
		Spout.Display("\nYou Can Retry The Operation Again.");
	}
	
	ProcessedFlag = 0;	//���������ϱ�־
	
	Spout.Display("\nPress Enter To Continue.");
	Spout.ConsoleSetCursorInsert();	
	
	Spin.EnableRx();
	while(1){
		u8 Temp = Spin.Launch();
		if(Temp == '\r') break;
	}
	Spin.DisableRx();
}

void SPIO::Sleep(u16 Click){
	TIM4->CNT = Click;
	HangUpFlag = 1;	
}

bool SPIO::TestCommand(){
	if(!ComBufferIndex || !ComBuffer[0] || ComBuffer[0]=='^'){ //��ָ��
		char *EMPTYSTR = "\nCommand Not Found.";
		Spout.Display(EMPTYSTR);
		return 1;
	}
	return 0;
}

void SPIO::Completion(){
	int Start; //��ʼ����
	
	for(Start=ComBufferIndex;Start>=0;Start--){ //��ѯ�ָ���
		if(ComBuffer[Start] == ' ') break; //���ҵ��ָ���
	}
	Start++;
	
	if(ComBufferIndex <= Start) return; //û���������貹ȫ
	if(ComBufferIndex-Start > 23) return; //���ݹ����޷���ȫ
	
	u32 DataNumber = ComBufferIndex-Start; //
	
	char TempStr[24] = {0};
	for(u32 i=0;i<DataNumber;i++){ //ת¼����ȫ����
		TempStr[i] = ComBuffer[Start+i];
	}
	

	
	u32 DictionaryIndex = 0; //�ֵ�ƥ��������
	if(Search(TempStr,DictionaryIndex)){ //����ƥ��
		
		for(u32 i=0;i<DataNumber;i++)
			Load('\b'); //���ԭ��ʾ����
		
		ComBufferIndex = Start; //�޸Ļ���ָ��
		
		for(u32 i=0;Dictionary[DictionaryIndex].Data[i];i++) //��ȫ
			Load(Dictionary[DictionaryIndex].Data[i]); //��ʾ����
		
		Load(' ');
	}
}


bool SPIO::Search(char Data[],u32 &Index){
	u32 Counter = 0; //������
	u32 TempIndex = 0; //��ʱ����
	for(u32 i=0;i<WordNumber;i++){ //�����ֵ�
		if(Compare(Data,Dictionary[i].Data)){ //�ҵ�ƥ����
			TempIndex = i; //��¼����
			Counter++; 
		}
	}
	if(Counter == 1){ //����һ��ƥ��
		Index = TempIndex; //��������
		return 1; //ƥ��ɹ�
	}
	return 0;
}

void SPIO::DelayMs(u32 n){
	while(n--)
		DelayUs(1000);	
}

void SPIO::DelayUs(u32 n){
	SysTick->LOAD=72*n;
	SysTick->CTRL=0x00000005;
	while(!(SysTick->CTRL&0x00010000));
	SysTick->CTRL=0x00000004;	
}

void SPIO::Load(char Data){
	if(Data == '\b'){ //�˸�
		if(!ComBufferIndex) return; //�޿�ɾ��
		ComBuffer[ComBufferIndex] = 0;
		ComBufferIndex--;
		Spout.Display("\b");
	}
	if(ComBufferIndex > 255) return; //������
	
	if(Data>31 && Data<127){
		char Temp[2] = {Data,0};		
		ComBuffer[ComBufferIndex] = Data;
		Spout.Display(Temp);
		ComBufferIndex++;
	}
}

void SPIO::AddWord(char *Data){
	if(WordNumber > 127) return;
	
	for(u32 i=0;i<WordNumber;i++) //���ֵ��м����û���ظ�
		if(Compare(Data,Dictionary[i].Data))
			return;
		
	for(u32 i=0;i<24;i++)
		Dictionary[WordNumber].Data[i] = 0;
	
	for(u32 i=0;i<23 && Data[i];i++)
		Dictionary[WordNumber].Data[i] = Data[i];
	
	WordNumber++;
}

bool SPIO::Compare(char StrA[],char StrB[]){
	for(u32 i=0;i<24;i++){
		if(!StrA[i] || !StrB[i]) break;
		if(Upper(StrA[i]) != Upper(StrB[i]))
			return 0;
	}
	return 1;
}

char SPIO::Upper(char Data){
	if(Data>=97 && Data<=122)
		return Data-32;
	return Data;
}
