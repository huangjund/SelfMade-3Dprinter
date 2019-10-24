#include "stm32f10x.h"

#ifndef _UART_H
	#define _UART_H
	
	class COM{
		public:
			void Init();
		
			void Decoder(); //译码并执行
			void Print(char *data,char State); //发送字符串
		
			u8 Buffer[160]; //接收缓存
			u8 RxData[160]; //辅助接受缓存
			bool RxFinish; //接收完成标志
			u32 DataCounter; //数据量
			u32 DataNumber; //总数据量
		
			bool VerifyError; //校验错误标志
		
			bool TestAddress(); //地址检验
			
			bool Compare(char StrA[],char StrB[]);
		
			u32 CharToNumber(u8 InLength,u8 *Data);
			u8* NumberToChar(u8 OutLength,u32 Data,u8 *Str);
		
			void Tx(u8); 
			
			u8 WriteTemp[4096];

	};
	
	extern COM Serial;

#endif

	