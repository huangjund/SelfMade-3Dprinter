#include "stm32f10x.h"
#include "GUI.h"

#define COLOR_OFF 0xC618
#define COLOR_HOLDING 0x6C7B
#define COLOR_COOLING 0xFB20
#define COLOR_HEATING 0xF000

#define COLOR_DISABLED 0xC618
#define COLOR_NOTHOME 0xFB20
#define COLOR_SWITCHED 0x6C7B
#define COLOR_WORKING 0x6C7B
#define COLOR_IDLEING 0x6C7B
#define COLOR_ENABLE 0x6C7B

class STATUS{ //״̬
	public:
		void GenerateDashRound1_H(DashRoundParameter *Data);
		void GenerateDashRound2_H(DashRoundParameter *Data);
		void GenetateDashRound3_H(DashRoundParameter *Data);

		void GenerateDashSquare1A_H(DashSquareParameter *Data);
		void GenerateDashSquare1B_H(DashSquareParameter *Data);
		void GenerateDashSquare2A_H(DashSquareParameter *Data);
		void GenerateDashSquare2B_H(DashSquareParameter *Data);
		void GenerateDashSquare3A_H(DashSquareParameter *Data);
		void GenerateDashSquare3B_H(DashSquareParameter *Data);
	
		void GeneratePicture1_H(BinaryPictureParameter *Data);
		void GeneratePicture2_H(BinaryPictureParameter *Data);
		void GeneratePicture3_H(BinaryPictureParameter *Data);
		void GenerateTextStatus1_H();
		void GenerateTextStatus2_H();
		void GenerateTextStatus3_H();
		
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
		
		
		void GenerateDashSquare1_S(DashSquareParameter *Data);
		void GenerateDashSquare2_S(DashSquareParameter *Data);
		void GenerateDashSquare3_S(DashSquareParameter *Data);
	
		void GenerateTextLevel1_S();
		void GenerateTextLevel2_S();
	
		void GeneratePicture1_S(BinaryPictureParameter *Data);
		void GeneratePicture2_S(BinaryPictureParameter *Data);
		void GenerateTextStatus1_S();
		void GenerateTextStatus2_S();


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
		
		
};

extern class STATUS Status;

