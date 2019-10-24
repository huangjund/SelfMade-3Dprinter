#include "stm32f10x.h"


class IBUS{
	public:
		void Init();
	
		u8 TempBuffer[255]; //���ջ���
		u8 Buffer[255]; //�������ܻ���
		bool RxFinish; //������ɱ�־
		u8 DataCounter; //������
		u8 DataNumber; //��������
		bool VerifyError; //У������־
		
		bool Transmit(char Address,char Data[],u32 TimeOut);
	
		bool TestReply(char Sym);
	
		bool TestIfVerifyError();
	private:
		bool WaitForReceive(u32 Ms);
	
		void Print(char *data); //�����ַ���		
		void Tx(u8);

};

extern class IBUS Ibus;

