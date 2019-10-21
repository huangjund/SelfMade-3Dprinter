#include "stm32f10x.h"

//控制总线：封装用于定义校验、缓存、接收发送数据、标志
class CBUS{
	public:
		void Init();
	
		u8 TempBuffer[128]; //数据暂存接收缓存
		u8 Buffer[128]; //辅助接受缓存
		bool RxFinish; //接收完成标志
		u8 DataCounter; //数据量
		u8 DataNumber; //总数据量
		bool VerifyError; //校验错误标志
		
		bool Transmit(char Address,char Data[],u32 TimeOut);
	
		bool TestReply(char Sym);
	
		bool TestIfVerifyError();
	
	private:
		bool WaitForReceive(u32 Ms);
		
		void Print(char *data); //发送字符串
		void Tx(u8);

};

extern class CBUS Cbus;
