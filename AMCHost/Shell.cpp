#include "Shell.h"
#include "Instruction.h"
#include "CCONI.h"

#define s64 long long
#define u64 unsigned long long

SHELL Shell;

void SHELL::DictInit(){
	Cconi.ExpandDictionary("Get");
	Cconi.ExpandDictionary("Set");
	Cconi.ExpandDictionary("Temperature");
	Cconi.ExpandDictionary("Status");
	Cconi.ExpandDictionary("Aim");
	Cconi.ExpandDictionary("Power");
	Cconi.ExpandDictionary("Cooler");
	Cconi.ExpandDictionary("Extruder");
	Cconi.ExpandDictionary("BasePlane");
	Cconi.ExpandDictionary("Environment");
	
	Cconi.ExpandDictionary("AutoHoming");
	
	Cconi.ExpandDictionary("Refresh");
	Cconi.ExpandDictionary("Dictionary");
}
void SHELL::Init(){
	Flush();
	DictInit();
	Instruct.GetTemExtAInit();
	Instruct.GetTemExtBInit();
	Instruct.GetTemBPInit();
	Instruct.GetTemEnvInit();
	Instruct.RefreshDictInit();
	Instruct.GetColPowEAInit();
	Instruct.GetColPowEBInit();
	Instruct.GetTemEASInit();
	Instruct.GetTemEBSInit();
	Instruct.GetTemBPSInit();
	Instruct.SetTemExtAInit();
	Instruct.SetTemExtBInit();
	Instruct.SetTemBPInit();
	Instruct.SetColExtAInit();
	Instruct.SetColExtBInit();
	Instruct.GetStaExtAInit();
	Instruct.GetStaExtBInit();
	Instruct.GetStaBPInit();
	Instruct.AutoHomeInit();
}

void SHELL::Decoder(){
	if(!Received) return;
	Slice(); //分割成词组
	
	if(Identify(Instruct.GetTemExtA_InsSet,Instruct.GetTemExtAOperand)){
		Instruct.GetTemExtA();
	}
	else if(Identify(Instruct.GetTemExtB_InsSet,Instruct.GetTemExtBOperand)){
		Instruct.GetTemExtB();
	}
	else if(Identify(Instruct.GetTemBP_InsSet,Instruct.GetTemBPOperand)){
		Instruct.GetTemBP();
	}
	else if(Identify(Instruct.GetTemEnv_InsSet,Instruct.GetTemEnvOperand)){
		Instruct.GetTemEnv();
	}
	else if(Identify(Instruct.GetColPowEA_InsSet,Instruct.GetColPowEAOperand)){
		Instruct.GetColPowEA();
	}
	else if(Identify(Instruct.GetColPowEB_InsSet,Instruct.GetColPowEBOperand)){
		Instruct.GetColPowEB();
	}
	else if(Identify(Instruct.GetTemEAS_InsSet,Instruct.GetTemEASOperand)){
		Instruct.GetTemEAS();
	}
	else if(Identify(Instruct.GetTemEBS_InsSet,Instruct.GetTemEBSOperand)){
		Instruct.GetTemEBS();
	}
	else if(Identify(Instruct.GetTemBPS_InsSet,Instruct.GetTemBPSOperand)){
		Instruct.GetTemBPS();
	}
	else if(Identify(Instruct.SetTemExtA_InsSet,Instruct.SetTemExtAOperand)){
		Instruct.SetTemExtA();
	}
	else if(Identify(Instruct.SetTemExtB_InsSet,Instruct.SetTemExtBOperand)){
		Instruct.SetTemExtB();
	}
	else if(Identify(Instruct.SetTemBP_InsSet,Instruct.SetTemBPOperand)){
		Instruct.SetTemBP();
	}
	else if(Identify(Instruct.SetColExtA_InsSet,Instruct.SetColExtAOperand)){
		Instruct.SetColExtA();
	}
	else if(Identify(Instruct.SetColExtB_InsSet,Instruct.SetColExtBOperand)){
		Instruct.SetColExtB();
	}
	else if(Identify(Instruct.GetStaExtA_InsSet,Instruct.GetStaExtAOperand)){
		Instruct.GetStaExtA();
	}
	else if(Identify(Instruct.GetStaExtB_InsSet,Instruct.GetStaExtBOperand)){
		Instruct.GetStaExtB();
	}
	else if(Identify(Instruct.GetStaBP_InsSet,Instruct.GetStaBPOperand)){
		Instruct.GetStaBP();
	}
	else if(Identify(Instruct.AutoHome_InsSet,Instruct.AutoHomeOperand)){
		Instruct.AutoHome();
	}
	else if(Identify(Instruct.RefreshDict_InsSet,Instruct.RefreshDictOperand)){
		Instruct.RefreshDict();
	}
	else{
		Instruct.NotFound();
	}
	Flush();
}

void SHELL::Flush(){//清除命令缓存
	for(u32 i=0;i<256;i++)
		ComBuffer[i] = 0;
	Received = 0;

}

u32 SHELL::GetNumber(u32 Index){//index: 第几个操作数数字量
	u32 Counter = 0;
	for(u32 i=0;i<Word.Number;i++){
		if(Word.Attribute[i] == 'N'){
			if(Counter == Index)
				return Chr2Num(Word.Data[i]);
			Counter++;
		}
	}
	return 0;
}

void SHELL::Slice(){
	Word.Number = 0;
	char *TempPtr = ComBuffer;
	for(u32 i=0;i<30;i++){
		TempPtr = AddWord(TempPtr);
		if(!(*TempPtr)) break;
	}
}

char SHELL::Upper(char Data){
	if(Data>=97 && Data<=122)
		return Data-32;
	return Data;
}

bool SHELL::Identify(char **Dic,u32 Number){
	u32 InsDataCounter[30] = {}; //指令关键词计数器
	u32 Index; //目标索引
	for(u32 i=0;i<30&&Dic[i][0];i++){ //在字典中查找
		u32 Temp = Search(Dic[i],Index);
		if(Temp){
			InsDataCounter[Index] = Temp;
		}
	}
	
	u32 ComDataCounter[30] = {}; //键入命令关键词计数器
	for(u32 i=0;i<Word.Number;i++){ //在键入命令中查找
		u32 Temp = Search(Word.Data[i],Index);
		if(Temp){
			ComDataCounter[Index] = Temp;
		}
	}
	
	for(u32 i=0;i<30;i++){ //关键词匹配检测
		if(InsDataCounter[i] != ComDataCounter[i])
			return 0;
	}
	if(Number != CountNumber()) //操作数数量检测
		return 0;
	
	return 1;
	
}

u32 SHELL::Search(char *Data,u32 &Index){//在字典中寻找Data
	u32 Counter = 0;
	for(u32 i=0;i<Dictionary.Number;i++)
		if(Compare(Data,Dictionary.Data[i])){
			Index = i;
			Counter++;
		}
		
	return Counter;
}

bool SHELL::Compare(char StrA[],char StrB[]){
	for(u32 i=0;i<24;i++){
		if(Upper(StrA[i]) != Upper(StrB[i]))
			return 0;
		if(!StrA[i] || !StrB[i]) break;		
	}
	return 1;
}

u32 SHELL::CountNumber(){
	u32 Counter = 0;
	for(u32 i=0;i<Word.Number;i++)
		if(Word.Attribute[i] == 'N')
			Counter++;
		
	return Counter;
}

u32 SHELL::CountWord(){
	u32 Counter = 0;
	for(u32 i=0;i<Word.Number;i++)
		if(Word.Attribute[i] == 'L')
			Counter++;
		
	return Counter;
}

bool SHELL::IsLetter(char Data){
	if(Data>='A' && Data<='Z') return 1;
	if(Data>='a' && Data<='z') return 1;
	return 0;
}

char* SHELL::AddWord(char *Ptr){
	char AttrLetter = 0;
	char AttrNumber = 0;
	char *WordPtr = Word.Data[Word.Number];
	
	for(u32 i=0;i<24;i++)
		Word.Data[Word.Number][i] = 0;
	
	for(u32 i=0;i<23;i++){
		if(IsLetter(*Ptr)) //是字母
			AttrLetter = 1;
		else if(IsNumber(*Ptr)) //是数字
			AttrNumber = 1;
		else break; //是其他符号
		*(WordPtr++) = *(Ptr++);
	}
	
	if(WordPtr != Word.Data[Word.Number]){ //是非符号字符串	
		if(AttrLetter) //包括字母
			Word.Attribute[Word.Number] = 'L';
		else if(!AttrLetter && AttrNumber) //仅包括数字
			Word.Attribute[Word.Number] = 'N';
		Word.Number++;
	}
		
	for(u32 i=0;i<256;i++){ //移动到下一个词或结尾处
		if(IsLetter(*Ptr)) break;
		else if(IsNumber(*Ptr)) break;
		else if(!(*Ptr)) break;
		Ptr++;
	}	

	return Ptr;
}

bool SHELL::IsNumber(char Data){
	if(Data>='0' && Data<='9') return 1;
	return 0;
}

void SHELL::Expand(char Data[]){
	if(Dictionary.Number > 127) return; //字典已经满了
	
	for(u32 i=0;i<Dictionary.Number;i++) //在字典中检查有没有重复
		if(Compare(Data,Dictionary.Data[i]))
			return;
	
	for(u32 i=0;i<24;i++) 
		Dictionary.Data[Dictionary.Number][i] = 0;
	
	for(u32 i=0;i<23 && Data[i];i++)
		Dictionary.Data[Dictionary.Number][i] = Data[i];
	
	Dictionary.Number++;
}

u32 SHELL::Chr2Num(char *Data){
	u32 Length;
	for(Length=0;Length<23 && Data[Length];Length++)
		if(Data[Length]>'9' || Data[Length]<'0')
			break;
		
	u64 Temp = 0;
	
	static u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};
	
	for(u32 i=0;i<Length;i++) //整数部分
		Temp += (Data[i]-'0')*Weight[Length-i];
	
	return Temp;
}