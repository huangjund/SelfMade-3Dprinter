#include "stm32f10x.h"

class SPIO{
	public:
		void Init();
		
		void ReadIn(); //等待接收命令
	
		void Decode(); //本地译码
	
		bool TestCommand(); //译码
		
		void WaitForEnter(); //等待Enter
	
		void Sleep(u16 Click); //挂起控制台
	
		void Completion(); //补全
		
		bool Search(char Data[],u32 &Index); //在字典中查询
	
		void DelayMs(u32 n);
		void DelayUs(u32 n);
		
		void Load(char Data); //向命令缓存中添加字符
		char ComBuffer[256]; //命令缓存
		u32 ComBufferIndex; //命令缓存索引
		
		void AddWord(char *Data); //向字典中添加词
		struct{
			char Data[24];
		}Dictionary[128];
		u32 WordNumber;
		
		bool Compare(char StrA[],char StrB[]);
		
		char Upper(char Data);
		
		bool HangUpFlag; //挂起标志
		
		bool ProcessedFlag; //处理完毕标志
};

extern class SPIO Spio;
