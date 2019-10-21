#include "stm32f10x.h"


class SPOUT{
	public:
		void Init();
		
		void InfoRestart(); //信息屏重启
		void InfoRefresh(); //刷新信息屏内容
		
		void ConsoleRestart(); //控制台重启
		void ConsoleClear(); //控制台清屏
		void ConsoleSetCursorBusy(); //设置控制台光标为Busy
		void ConsoleSetCursorInsert(); //设置控制台光标为Insert
	
		void ChangeStatusBar(); //修改控制台状态栏
		void Display(char Data[]); //控制台显示
	

		
		bool Transmit(char Address,char Data[]);
	
		void Tx(u8 Data);
	
		char* Num2Chr(u32 Data,char Str[]);
	
		bool WaitFlag;
		
		void DelayUs(u32 n);
};

extern class SPOUT Spout;

