#include "stm32f10x.h"


class SPOUT{
	public:
		void Init();
		
		void InfoRestart(); //��Ϣ������
		void InfoRefresh(); //ˢ����Ϣ������
		
		void ConsoleRestart(); //����̨����
		void ConsoleClear(); //����̨����
		void ConsoleSetCursorBusy(); //���ÿ���̨���ΪBusy
		void ConsoleSetCursorInsert(); //���ÿ���̨���ΪInsert
	
		void ChangeStatusBar(); //�޸Ŀ���̨״̬��
		void Display(char Data[]); //����̨��ʾ
	

		
		bool Transmit(char Address,char Data[]);
	
		void Tx(u8 Data);
	
		char* Num2Chr(u32 Data,char Str[]);
	
		bool WaitFlag;
		
		void DelayUs(u32 n);
};

extern class SPOUT Spout;

