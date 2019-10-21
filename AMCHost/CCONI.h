#include "stm32f10x.h"

class CCONI{
	public:
		char GetCommand();
		char SetStatusBar();
		char SetInfoScreen();
		char ConsoleDisplay(char Data[],bool Hold);
		char HangUpConsole();
		char ExpandDictionary(char Data[]);
		char GetSwitchAStatus();
		char GetSwitchBStatus();
		
		bool TestLink();
	private:
		char* Num2Chr(u32 Data,char* Str);
		char* Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign);
		double Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign);
		
};

extern class CCONI Cconi;

