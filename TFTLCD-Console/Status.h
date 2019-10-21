#include "stm32f10x.h"
#include "GUI.h"


#define BUFFER_SIZE 40 //缓冲区大小(不小于2)
class STATUS{ //状态
	public:
		void Init(); //初始化
		void ShowStatus(); //显示状态栏内容
		void ShowData(); //显示缓冲区内容

		void AddChar(char Data); //缓冲区中追加字符
		void Clear(); //清屏
	
		void SetState(char Data[37]); //更改状态栏缓冲区
		char State[38]; //状态栏缓冲区
		char DispBuffer[14][37]; //显示缓冲区
		u16 Cursor[2]; //光标		
		
		// I--插入 B--正忙
		char CursorStatus; //光标状态
		//A状态有2个子态 B状态有4个子态/-\|
		char CursorCounter; //光标状态计数器
		
		void ShowChar(u16 Curs[2],char Data); //改变并显示一个字符
		void ShowLine(u16 Posi[2],char Data[],u16 Color); //显示一行内容
		void EnableCursor(); //使能光标
		void DisableCursor(); //失能光标
		void SetCursorStatus(char Stat); //设置光标状态
		void SetCursorPosition(u32 PosiX,u32 PosiY); //设置光标位置
		void ShowCursor(); //显示光标
		void HideCursor(); //隐藏光标
};

extern class STATUS Status;


