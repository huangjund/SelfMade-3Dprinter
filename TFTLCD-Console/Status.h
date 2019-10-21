#include "stm32f10x.h"
#include "GUI.h"


#define BUFFER_SIZE 40 //��������С(��С��2)
class STATUS{ //״̬
	public:
		void Init(); //��ʼ��
		void ShowStatus(); //��ʾ״̬������
		void ShowData(); //��ʾ����������

		void AddChar(char Data); //��������׷���ַ�
		void Clear(); //����
	
		void SetState(char Data[37]); //����״̬��������
		char State[38]; //״̬��������
		char DispBuffer[14][37]; //��ʾ������
		u16 Cursor[2]; //���		
		
		// I--���� B--��æ
		char CursorStatus; //���״̬
		//A״̬��2����̬ B״̬��4����̬/-\|
		char CursorCounter; //���״̬������
		
		void ShowChar(u16 Curs[2],char Data); //�ı䲢��ʾһ���ַ�
		void ShowLine(u16 Posi[2],char Data[],u16 Color); //��ʾһ������
		void EnableCursor(); //ʹ�ܹ��
		void DisableCursor(); //ʧ�ܹ��
		void SetCursorStatus(char Stat); //���ù��״̬
		void SetCursorPosition(u32 PosiX,u32 PosiY); //���ù��λ��
		void ShowCursor(); //��ʾ���
		void HideCursor(); //���ع��
};

extern class STATUS Status;


