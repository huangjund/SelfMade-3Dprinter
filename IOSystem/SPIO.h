#include "stm32f10x.h"

class SPIO{
	public:
		void Init();
		
		void ReadIn(); //�ȴ���������
	
		void Decode(); //��������
	
		bool TestCommand(); //����
		
		void WaitForEnter(); //�ȴ�Enter
	
		void Sleep(u16 Click); //�������̨
	
		void Completion(); //��ȫ
		
		bool Search(char Data[],u32 &Index); //���ֵ��в�ѯ
	
		void DelayMs(u32 n);
		void DelayUs(u32 n);
		
		void Load(char Data); //�������������ַ�
		char ComBuffer[256]; //�����
		u32 ComBufferIndex; //���������
		
		void AddWord(char *Data); //���ֵ�����Ӵ�
		struct{
			char Data[24];
		}Dictionary[128];
		u32 WordNumber;
		
		bool Compare(char StrA[],char StrB[]);
		
		char Upper(char Data);
		
		bool HangUpFlag; //�����־
		
		bool ProcessedFlag; //������ϱ�־
};

extern class SPIO Spio;
