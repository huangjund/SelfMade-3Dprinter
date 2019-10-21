#include "stm32f10x.h"

class STATUS{ //״̬
	public:

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
		

		double PosiX; //X�ᵱǰ����
		double PosiY; //Y�ᵱǰ����
		double PosiZ; //Z�ᵱǰ����
		double PosiA;
		double PosiB;
		
		double PosiXMax; //X���������
		double PosiYMax; //Y���������
		double PosiZMax; //Z���������
		u16 L1Reset; //L1����ʣ������
		u16 L2Reset; //L2����ʣ������
		u16 L1Max; //L1�����������
		u16 L2Max; //L2�����������

		//E--��ƽ D--δ��ƽ
		char StateLeveling;
		
		double TestPosition[5];

		//D--ʧ�� N--δ��λ S--�л�(Del) W--���� I--����
		char StateFA; //�Զ���״̬
		
		//E--���� D--����
		char StateIDP; //IDP״̬
		
		//E--���� D--����
		char StateStepper; //IDP״̬
		
		//E--��� D--δ���
		char StateHomed; 
		
		//E--���� D--�ر�
		char StateSwitchA;
		
		//E--���� D--�ر�
		char StateSwitchB;
		
		char StatusBar[38];
		
		bool LinkToScon;
		
		bool LinkToTcon;
		
		bool LinkToFcon;
		
		bool LinkToCcon;
		
		bool Manufacturing;
		
		bool Running;
};

extern class STATUS Status;
