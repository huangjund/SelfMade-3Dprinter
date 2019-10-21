#include "stm32f10x.h"

class SHELL{
	public:
		void DictInit();
		void Init();
		void Decoder();
		void Flush();
	
		u32 GetNumber(u32 Index); //从词组中提取数字

		char ComBuffer[256];
		bool Received;	
		
	private:
	
		bool Identify(char **Dic,u32 Number);
	
		u32 Search(char *Data,u32 &Index);
	
		bool Compare(char StrA[],char StrB[]); //比较两个串
		
		u32 CountNumber(); //检测词组中数字数量
	
		u32 CountWord(); //检测词组中单词数量
	
		char Upper(char Data); //转换成大写		
	
		void Slice();
		
		char* AddWord(char *Ptr); //向切片表中添加字符串
		struct{
			char Data[30][24];
			char Attribute[30];
			u32 Number;
		}Word;										//命令切片单词
		
	public:		
		void Expand(char Data[]); //向字典中添加字符串
	
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
