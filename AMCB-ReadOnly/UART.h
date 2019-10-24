#include "stm32f10x.h"

#ifndef _UART_H
	#define _UART_H
	
	class COM{
		public:
			void Init();
		
			void Decoder(); //���벢ִ��
			void Print(char *data,char State); //�����ַ���
		
			u8 Buffer[160]; //���ջ���
			u8 RxData[160]; //�������ܻ���
			bool RxFinish; //������ɱ�־
			u32 DataCounter; //������
			u32 DataNumber; //��������
		
			bool VerifyError; //У������־
		
			bool TestAddress(); //��ַ����
			
			bool Compare(char StrA[],char StrB[]);
		
			u32 CharToNumber(u8 InLength,u8 *Data);
			u8* NumberToChar(u8 OutLength,u32 Data,u8 *Str);
		
			void Tx(u8); 
			
			u8 WriteTemp[4096];

	};
	
	extern COM Serial;

#endif

	