#include "stm32f10x.h"

class STATUS{ //״̬
	public:

	//Temperature Controler
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
		
	//Motion Controler
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
		
		double TestPosition[5];
		
		bool NozzleState[2];
		
		//E--��ƽ D--δ��ƽ
		char StateLeveling;
		//D--ʧ�� N--δ��λ S--�л�(Del) W--���� I--����
		char StateFA; //�Զ���״̬
		//E--���� D--����
		char StateIDP; //IDP״̬
		//E--���� D--����
		char StateStepper; //�������״̬
		//E--��� D--δ���
		char StateHomed; //��λ״̬
		
	//FileSystem Controler	
		u8 CardTotalVolume; //�洢�������� [MB]
		u32 CardRestVolume; //�洢��ʣ������ [B]
		bool FileExist; //�ļ�������
		char FileName[32]; //�ļ���
		u32 FileSize; //�ļ���С [B]
		u32 CommandNumber; //�ļ�ָ����
		u32 EstimateTime; //�ļ�Ԥ��ʱ�� [min]
		u32 TotalDistance; //�ļ����г� [cm]
		u32 FileInfo1; //�ļ�������Ϣ1
		u32 FileInfo2; //�ļ�������Ϣ2
		u32 FileInfo3; //�ļ�������Ϣ3
		u32 FileInfo4; //�ļ�������Ϣ4
		u32 FileInfo5; //�ļ�������Ϣ5
		u32 FileInfo6; //�ļ�������Ϣ6
		u32 FilePointer; //�ļ�ָ��
		
	//Console
		//E--���� D--�ر�
		char StateSwitchA;
		//E--���� D--�ر�
		char StateSwitchB;
		// *************************************
		// T A S F M
		char StatusBar[38];
	//ʱ��ϵͳ
		u8 YMD[3];
		u8 HMS[3];
	
	//����ϵͳ
		bool LinkToScon; //SC��·״̬
		bool LinkToTcon; //TC��·״̬
		bool LinkToFcon; //FC��·״̬
		bool LinkToCcon; //CC��·״̬
		bool LinkToAux1; //A1��·״̬
		bool LinkToAux2; //A2��·״̬
	//����ϵͳ
		bool MaterialA; //����A״̬
		bool MaterialB; //����B״̬		
		
};

extern class STATUS Status;
