#include "stm32f10x.h"

class SHELL{
	public:
		void DictInit();
		void Init();
		void Decoder();
		void Flush();
		void Expand(char Data[]); //���ֵ�������ַ���	
	
		char* GetNumber(char *Ptr,u32 &Number); //�Ӵ�������ȡ����

		char ComBuffer[256];
		bool Received;	
		
	private:
		bool Match(char *SentenceA,char *SentenceB);
		char* GetWord(char *Ptr,char Buffer[24],char &Attr);
		
	
		bool Compare(char StrA[],char StrB[]); //�Ƚ�������
		
	
		char Upper(char Data); //ת���ɴ�д		
	
			

	private:
		struct{
			char Data[128][24];
			u32 Number;
		}Dictionary;
		
		bool IsLetter(char Data); //�����Ƿ�����ĸ
		bool IsNumber(char Data); //�����Ƿ�������
		
		u32 Chr2Num(char *Data);
};

extern class SHELL Shell;
