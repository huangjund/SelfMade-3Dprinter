#include "Shell.h"
#include "Instruction.h"
#include "CCONI.h"

#define s64 long long
#define u64 unsigned long long

SHELL Shell;

void SHELL::DictInit(){
	Cconi.ExpandDictionary("Set");
	Cconi.ExpandDictionary("Shift");
	Cconi.ExpandDictionary("Enable");
	Cconi.ExpandDictionary("Disable");
	Cconi.ExpandDictionary("Start");
	Cconi.ExpandDictionary("Halt");
	Cconi.ExpandDictionary("Pause");
	Cconi.ExpandDictionary("Resume");
	Cconi.ExpandDictionary("Reload");
	Cconi.ExpandDictionary("Temperature");
	Cconi.ExpandDictionary("Warning");
	Cconi.ExpandDictionary("Error");
	Cconi.ExpandDictionary("Manufacture");
	Cconi.ExpandDictionary("Main");
	Cconi.ExpandDictionary("Auxiliary");
	Cconi.ExpandDictionary("Cooler");
	Cconi.ExpandDictionary("Power");
	Cconi.ExpandDictionary("Extruder");
	Cconi.ExpandDictionary("BasePlane");
	Cconi.ExpandDictionary("Axis");
	Cconi.ExpandDictionary("StepperMotors");
	Cconi.ExpandDictionary("Nozzle");
	Cconi.ExpandDictionary("Storage");
	Cconi.ExpandDictionary("Filament");
	Cconi.ExpandDictionary("Illuminator");
	Cconi.ExpandDictionary("IDP");
	Cconi.ExpandDictionary("Info");
	Cconi.ExpandDictionary("Report");
	
	Cconi.ExpandDictionary("AutoHoming");
	
	Cconi.ExpandDictionary("Refresh");
	Cconi.ExpandDictionary("Dictionary");
}
void SHELL::Init(){
	Flush();
	DictInit();
}

void SHELL::Decoder(){
	if(!Received) return;
	
	if(Match(ComBuffer,"Set Temperature Extruder A 0")){
		u32 Temperature = 0;
		GetNumber(ComBuffer,Temperature);
		Instruct.SetTemExtA(Temperature);
	}
	else if(Match(ComBuffer,"Set Temperature Extruder B 0")){
		u32 Temperature = 0;
		GetNumber(ComBuffer,Temperature);
		Instruct.SetTemExtB(Temperature);
	}
	else if(Match(ComBuffer,"Set Temperature BasePlane 0")){
		u32 Temperature = 0;
		GetNumber(ComBuffer,Temperature);
		Instruct.SetTemBP(Temperature);
	}
	else if(Match(ComBuffer,"Set Power Cooler A 0")){
		u32 Power = 0;
		GetNumber(ComBuffer,Power);
		Instruct.SetColExtA(Power);
	}
	else if(Match(ComBuffer,"Set Power Cooler B 0")){
		u32 Power = 0;
		GetNumber(ComBuffer,Power);
		Instruct.SetColExtB(Power);
	}
	else if(Match(ComBuffer,"AutoHoming")){
		Instruct.AutoHome();
	}
	else if(Match(ComBuffer,"Shift Axis X 0")){
		u32 Position = 0;
		GetNumber(ComBuffer,Position);
		Instruct.MoveAxisX(Position);
	}
	else if(Match(ComBuffer,"Shift Axis Y 0")){
		u32 Position = 0;
		GetNumber(ComBuffer,Position);
		Instruct.MoveAxisY(Position);
	}
	else if(Match(ComBuffer,"Shift Axis Z 0")){
		u32 Position = 0;
		GetNumber(ComBuffer,Position);
		Instruct.MoveAxisZ(Position);
	}
	else if(Match(ComBuffer,"Enable Nozzle A")){
		Instruct.EnableNozzleA();
	}
	else if(Match(ComBuffer,"Disable Nozzle A")){
		Instruct.DisableNozzleA();
	}
	else if(Match(ComBuffer,"Enable Nozzle B")){
		Instruct.EnableNozzleB();
	}
	else if(Match(ComBuffer,"Disable Nozzle B")){
		Instruct.DisableNozzleB();
	}
	
	else if(Match(ComBuffer,"Enable StepperMotots")){
		Instruct.EnableStepper();
	}
	else if(Match(ComBuffer,"Enable IDP")){
		Instruct.EnableIDP();
	}
	else if(Match(ComBuffer,"Disable IDP")){
		Instruct.DisableIDP();
	}
	else if(Match(ComBuffer,"Enable Main Illuminator")){
		Instruct.EnableMainLight();
	}
	else if(Match(ComBuffer,"Disable Main Illuminator")){
		Instruct.DisableMainLight();
	}
	else if(Match(ComBuffer,"Enable Auxiliary Illuminator")){
		Instruct.EnableAuxiliaryLight();
	}
	else if(Match(ComBuffer,"Disable Auxiliary Illuminator")){
		Instruct.DisableAuxiliaryLight();
	}
	else if(Match(ComBuffer,"Storage Info")){
		Instruct.StorageInfo();
	}
	else if(Match(ComBuffer,"Manufacture Start")){
		Instruct.StartManufacturing();
	}
	else if(Match(ComBuffer,"Manufacture Halt")){
		Instruct.StopManufacturing();
	}
	else if(Match(ComBuffer,"Manufacture Pause")){
		Instruct.PauseManufacturing();
	}
	else if(Match(ComBuffer,"Manufacture Resume")){
		Instruct.ResumeManufacturing();
	}
	else if(Match(ComBuffer,"Reload Filament")){
		Instruct.ReloadFilament();
	}
	else if(Match(ComBuffer,"Warning Info")){
		Instruct.WarningInfo();
	}
	else if(Match(ComBuffer,"Error Info")){
		Instruct.ErrorInfo();
	}
	else if(Match(ComBuffer,"Manufacture Info")){
		Instruct.ManufacturingInfo();
	}
	else if(Match(ComBuffer,"Manufacture Report")){
		Instruct.ManufacturingReport();
	}
	else if(Match(ComBuffer,"Refresh Dictionary")){
		Instruct.RefreshDict();
	}
	else{
		Instruct.NotFound();
	}
	Flush();
}

void SHELL::Flush(){
	for(u32 i=0;i<256;i++)
		ComBuffer[i] = 0;
	Received = 0;

}

char SHELL::Upper(char Data){
	if(Data>=97 && Data<=122)
		return Data-32;
	return Data;
}

bool SHELL::Compare(char StrA[],char StrB[]){
	for(u32 i=0;i<24;i++){
		if(Upper(StrA[i]) != Upper(StrB[i]))
			return 0;
		if(!StrA[i] || !StrB[i]) break;		
	}
	return 1;
}

bool SHELL::Match(char *SentenceA,char *SentenceB){
	char *PtrA = SentenceA;
	char *PtrB = SentenceB;
	char BufferA[24] = {0};
	char BufferB[24] = {0};
	char AttributeA = 'E';
	char AttributeB = 'E';
	for(u8 i=0;i<30;i++){
		PtrA = GetWord(PtrA,BufferA,AttributeA);
		PtrB = GetWord(PtrB,BufferB,AttributeB);
		if(AttributeA != AttributeB) 
			return 0;
		if(AttributeA=='L' && !Compare(BufferA,BufferB))
			return 0;
		if(AttributeA == 'E')
			return 1;
	}
	return 0;
}

char* SHELL::GetWord(char *Ptr,char Buffer[24], char &Attr){
	char AttrLetter = 0;
	char AttrNumber = 0;
	char *WordPtr = Buffer;
	
	for(u32 i=0;i<24;i++)
		Buffer[i] = 0;
	
	for(u32 i=0;i<23;i++){
		if(IsLetter(*Ptr)) //是字母
			AttrLetter = 1;
		else if(IsNumber(*Ptr)) //是数字
			AttrNumber = 1;
		else break; //是其他符号
		*(WordPtr++) = *(Ptr++);
	}
	
	Attr = 'E';
	if(WordPtr != Buffer){ //是有效单词(长度大于0)
		if(AttrLetter) //包括字母
			Attr = 'L';
		else if(!AttrLetter && AttrNumber) //仅包括数字
			Attr = 'N';
	}
		
	for(u32 i=0;i<256;i++){ //移动到下一个词或结尾处
		if(IsLetter(*Ptr)) break;
		else if(IsNumber(*Ptr)) break;
		else if(!(*Ptr)) break;
		Ptr++;
	}	
	return Ptr;
}

char* SHELL::GetNumber(char *Ptr,u32 &Number){
	Number = 0;
	char *Index = Ptr;	
	for(u32 i=0;i<255;i++){
		if(*Index == 0) return Index; 
		if(IsNumber(*Index)) break;
		else Index++;
	}
	char Buffer[24] = {0};
	char Attribute = 'E';
	for(u8 i=0;i<30;i++){
		Index = GetWord(Index,Buffer,Attribute);
		if(Attribute == 'N'){
			Number = Chr2Num(Buffer);
			return Index;
		}
		if(Attribute == 'E')
			return Index;
	}
	return Index;
}

bool SHELL::IsLetter(char Data){
	if(Data>='A' && Data<='Z') return 1;
	if(Data>='a' && Data<='z') return 1;
	return 0;
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


