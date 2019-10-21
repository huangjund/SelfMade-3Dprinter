#include "stm32f10x.h"
#include "CH376.h"

class INTERFACE{
	public:
		void Init(); //��ʼ��
		bool MountNUnmount(); //����&�������

		int ReadDataWithVerify(u8 Index,u16 Length); //��ȡ�ļ����ݲ�У��
	
		bool ReadData(u8 Index,u16 Length); //��ȡ�ļ�����
	
		u8 Buffer[1024];
		u16 DataCounter;
	private:
		
		struct{
			FATINFO File[64]; //�ļ��б�
			u32 FilePointer[64]; //�ļ�ָ��
			bool Opened[64];
			u32 FileCounter; //�б����ļ���
		}Flat;
		
		
		bool OpenFile(u8 Index); //��Ŀ¼���ļ�		
		
		bool TestIfOpened(); //����Ƿ����ļ���
		
		void CloseAll(); //�ر������Ѵ��ļ�
		
		void ReFreash(); //Ӳ��ˢ��
		
		void GenerateFileName(u8 *Sour,u8 *Dist); //��8.3������n.m���ļ���
};


extern class INTERFACE Interface;


