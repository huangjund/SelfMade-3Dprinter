#include "stm32f10x.h"

class SHELL{
	public:
		void DictInit();
		void Init();
		void Decoder();
		void Flush();
		void Expand(char Data[]); //向字典中添加字符串	
	
		char* GetNumber(char *Ptr,u32 &Number); //从词组中提取数字

		char ComBuffer[256];
		bool Received;	
		
	private:
		bool Match(char *SentenceA,char *SentenceB);
		char* GetWord(char *Ptr,char Buffer[24],char &Attr);
		
	
		bool Compare(char StrA[],char StrB[]); //比较两个串
		
	
		char Upper(char Data); //转换成大写		
	
			

	private:
		struct{
			char Data[128][24];
			u32 Number;
		}Dictionary;
		
		bool IsLetter(char Data); //测试是否是字母
		bool IsNumber(char Data); //测试是否是数字
		
		u32 Chr2Num(char *Data);
};

extern class SHELL Shell;
