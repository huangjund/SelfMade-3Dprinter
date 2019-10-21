#include "stm32f10x.h"

class SHELL{
	public:
		void DictInit();
		void Init();
		void Decoder();
		void Flush();
	
		u32 GetNumber(u32 Index); //�Ӵ�������ȡ����

		char ComBuffer[256];
		bool Received;	
		
	private:
	
		bool Identify(char **Dic,u32 Number);
	
		u32 Search(char *Data,u32 &Index);
	
		bool Compare(char StrA[],char StrB[]); //�Ƚ�������
		
		u32 CountNumber(); //����������������
	
		u32 CountWord(); //�������е�������
	
		char Upper(char Data); //ת���ɴ�д		
	
		void Slice();
		
		char* AddWord(char *Ptr); //����Ƭ��������ַ���
		struct{
			char Data[30][24];
			char Attribute[30];
			u32 Number;
		}Word;										//������Ƭ����
		
	public:		
		void Expand(char Data[]); //���ֵ�������ַ���
	
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
