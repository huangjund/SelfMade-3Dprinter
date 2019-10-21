#include "stm32f10x.h"

class STATUS{
	public:
		void Init();
		
		void SetInfo(u8 Buffer[],u32 DataNumber);
	
		u32 C2Q(u8 *data);
		
		u16 TempExtruderA; //������A��ǰ�¶�
		u16 TempExtruderB; //������B��ǰ�¶�
		u16 TempBasePlane; //��׼�浱ǰ�¶�
		u16 TempAir; //�����¶�
		u16 TempExtruderASet; //������A�趨�¶�
		u16 TempExtruderBSet; //������B�趨�¶�
		u16 TempBasePlaneSet; //��׼���趨�¶�
		u8 PowerHeaterExtruderA; //������A���ȹ���
		u8 PowerHeaterExtruderB; //������B���ȹ���
		u8 PowerHeaterBasePlane; //��׼����ȹ���
		u8 PowerCoolerExtruderA; //������A��ȴ����
		u8 PowerCoolerExtruderB; //������B��ȴ����
		//H--���� C--��ȴ M--���� O--�ر�
		char StateExtruderA; //������A״̬
		char StateExtruderB; //������B״̬
		char StateBasePlane; //��׼��״̬

		u16 PosiX; //X�ᵱǰ����
		u16 PosiY; //Y�ᵱǰ����
		u16 PosiZ; //Z�ᵱǰ����
		u16 PosiXMax; //X���������
		u16 PosiYMax; //Y���������
		u16 PosiZMax; //Z���������
		u16 L1Used; //L1������ʹ������
		u16 L2Used; //L2������ʹ������
		u16 L1Max; //L1�����������
		u16 L2Max; //L2�����������

		//D--ʧ�� N--δ��λ S--�л�(Del) W--���� I--����
		char StateFA; //�Զ���״̬
		
		//E--���� D--����
		char StateIDP; //IDP״̬
		
		char StateBar[38]; //״̬��
};


extern class STATUS Status;

