#include "stm32f10x.h"

#ifndef _CARD_H
	#define _CARD_H
	
	class CARD{
		public:
			void Init(); //�ӿڳ�ʼ��
			u8 ReadWrite(u8 Data,bool Read); //������д�ӿ�
			bool WaitForReady(u8 Select); //�ȴ���Ԫ׼����
			u8 ReadCapacity(u8 Select); //��ȡ��Ԫ����
			void WriteEnable(u8 Select); //��Ԫ����д��
			void WriteDisable(u8 Select); //��Ԫ��ֹд��
			void GetStatus(); //��ȡ�洢����Ϣ
			bool GetAddress(u32 Index,u8 &Card,u32 &Address); //�����߼����������ַ
		
			void SectorErase(u8 Select,u32 Address); //����һ������
			
			bool ReadData(u32 Address,u8 *Data,u32 Length); //��ȡһ����������
			bool WritePage(u32 Address,u8 *Data,u32 Length); //ҳд
			bool ReadSector(u32 Index,u8 *Data); //��ȡһ������
			bool WriteSector(u32 Index,u8 *Data); //д��һ������
			u8 GetVolume(); //��ȡ�洢������
			
		
			struct{
				bool Connected[8]; //��Ԫ����
				u8 Capacity[8]; //��Ԫ����
				u32 SectorNumber[8]; //��Ԫ������������
				u32 StartSector[8]; //��Ԫ��ʼ�߼�������
			}Status; //�洢��״̬
			
	};
	
	extern CARD Card;

#endif

