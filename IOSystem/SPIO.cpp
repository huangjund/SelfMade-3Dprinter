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
	TIM4->ARR = 50000;//预装载
	TIM4->PSC = 7199;//分频
	TIM4->CR1 |= 0x01<<4; //向下计数
	TIM4->DIER |= 0x01<<0;
	NVIC->IP[30] = 0x30;//优先级3
	NVIC->ISER[0] |= 0x01<<30;
	TIM4->CR1 |= 0x01<<0;//使能

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
	Spout.ConsoleClear(); //清屏
	DelayMs(1000);
	Spout.Display(">>"); //显示命令提示符
	
	for(u32 i=0;i<256;i++) //清空命令缓存
		ComBuffer[i] = 0;
	ComBufferIndex = 0;
	
	Spin.EnableRx(); //允许输入
	
	while(1){ //等待输入
		Serial.Decoder(); //插入译码进程
		
		u8 TempChar = Spin.Launch(); //获取输入
		if(!TempChar) continue; //无效字符
		else if(TempChar == '\r'){ //结束符
			Spout.Display("\r"); //换行
			break; //停止等待
		}
		else if(TempChar == '\t') Completion(); //补全
		else Load(TempChar);
		
		
	}
	Spin.DisableRx(); //禁止输入
	
	Decode();
	
	WaitForEnter();
}

void SPIO::Decode(){
	char *ANASTR = "Processing...";
	Spout.Display(ANASTR);
	Spout.ConsoleSetCursorBusy();	
	if(TestCommand()){ //本地处理完成
		ProcessedFlag = 1;
	}
	else{ //本地无法处理
		ProcessedFlag = 0;	//清除处理完毕标志
		Sleep(50000);
	}
}

void SPIO::WaitForEnter(){	
	while(HangUpFlag){ //等待回复
		Serial.Decoder(); //插入译码进程
	}
	
	if(!ProcessedFlag){ //未处理
		Spout.Display("\nHost Timeout.");
		Spout.Display("\nYou Can Retry The Operation Again.");
	}
	
	ProcessedFlag = 0;	//清除处理完毕标志
	
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
	if(!ComBufferIndex || !ComBuffer[0] || ComBuffer[0]=='^'){ //空指令
		char *EMPTYSTR = "\nCommand Not Found.";
		Spout.Display(EMPTYSTR);
		return 1;
	}
	return 0;
}

void SPIO::Completion(){
	int Start; //起始索引
	
	for(Start=ComBufferIndex;Start>=0;Start--){ //查询分隔符
		if(ComBuffer[Start] == ' ') break; //查找到分隔符
	}
	Start++;
	
	if(ComBufferIndex <= Start) return; //没有内容无需补全
	if(ComBufferIndex-Start > 23) return; //内容过多无法补全
	
	u32 DataNumber = ComBufferIndex-Start; //
	
	char TempStr[24] = {0};
	for(u32 i=0;i<DataNumber;i++){ //转录待补全内容
		TempStr[i] = ComBuffer[Start+i];
	}
	

	
	u32 DictionaryIndex = 0; //字典匹配项索引
	if(Search(TempStr,DictionaryIndex)){ //内容匹配
		
		for(u32 i=0;i<DataNumber;i++)
			Load('\b'); //清除原显示内容
		
		ComBufferIndex = Start; //修改缓存指针
		
		for(u32 i=0;Dictionary[DictionaryIndex].Data[i];i++) //补全
			Load(Dictionary[DictionaryIndex].Data[i]); //显示内容
		
		Load(' ');
	}
}


bool SPIO::Search(char Data[],u32 &Index){
	u32 Counter = 0; //计数器
	u32 TempIndex = 0; //临时索引
	for(u32 i=0;i<WordNumber;i++){ //遍历字典
		if(Compare(Data,Dictionary[i].Data)){ //找到匹配项
			TempIndex = i; //记录索引
			Counter++; 
		}
	}
	if(Counter == 1){ //仅有一项匹配
		Index = TempIndex; //传递索引
		return 1; //匹配成功
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
	if(Data == '\b'){ //退格
		if(!ComBufferIndex) return; //无可删除
		ComBuffer[ComBufferIndex] = 0;
		ComBufferIndex--;
		Spout.Display("\b");
	}
	if(ComBufferIndex > 255) return; //缓存满
	
	if(Data>31 && Data<127){
		char Temp[2] = {Data,0};		
		ComBuffer[ComBufferIndex] = Data;
		Spout.Display(Temp);
		ComBufferIndex++;
	}
}

void SPIO::AddWord(char *Data){
	if(WordNumber > 127) return;
	
	for(u32 i=0;i<WordNumber;i++) //在字典中检查有没有重复
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
