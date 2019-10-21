#include "Status.h"

STATUS Status;

void STATUS::Init(){
	TempExtruderA = 0; //������A��ǰ�¶�
	TempExtruderB = 0; //������B��ǰ�¶�
	TempBasePlane = 0; //��׼�浱ǰ�¶�
	TempAir = 0; //�����¶�
	TempExtruderASet = 0; //������A�趨�¶�
	TempExtruderBSet = 0; //������B�趨�¶�
	TempBasePlaneSet = 0; //��׼���趨�¶�
	PowerHeaterExtruderA = 0; //������A���ȹ���
	PowerHeaterExtruderB = 0; //������B���ȹ���
	PowerHeaterBasePlane = 0; //��׼����ȹ���
	PowerCoolerExtruderA = 0; //������A��ȴ����
	PowerCoolerExtruderB = 0; //������B��ȴ����
	//H--���� C--��ȴ M--���� O--�ر�
	StateExtruderA = 'O'; //������A״̬
	StateExtruderB = 'O'; //������B״̬
	StateBasePlane = 'O'; //��׼��״̬

	PosiX = 0; //X�ᵱǰ����
	PosiY = 0; //Y�ᵱǰ����
	PosiZ = 0; //Z�ᵱǰ����
	PosiXMax = 0; //X���������
	PosiYMax = 0; //Y���������
	PosiZMax = 0; //Z���������
	L1Used = 0; //L1������ʹ������
	L2Used = 0; //L2������ʹ������
	L1Max = 0; //L1�����������
	L2Max = 0; //L2�����������

	//D--ʧ�� N--δ��λ S--�л�(Del) W--���� I--����
	StateFA = 'D'; //�Զ���״̬
	
	//E--���� D--����
	StateIDP = 'D'; //IDP״̬
	
	for(u32 i=0;i<37;i++)
		StateBar[i] = ' '; //״̬��
	StateBar[37] = 0;
}

void STATUS::SetInfo(u8 Buffer[],u32 DataNumber){
	if(DataNumber < 3) return;
	Status.TempExtruderA = C2Q(Buffer);
	if(Status.TempExtruderA > 320)
		Status.TempExtruderA = 320;	
	
	if(DataNumber < 6) return;
	Status.TempExtruderB = C2Q(Buffer+3);
	if(Status.TempExtruderB > 320)
		Status.TempExtruderB = 320;	
	
	if(DataNumber < 9) return;
	Status.TempBasePlane = C2Q(Buffer+6);
	if(Status.TempBasePlane > 150)
		Status.TempBasePlane = 150;
	
	if(DataNumber < 12) return;
	Status.TempExtruderASet = C2Q(Buffer+9);
	if(Status.TempExtruderASet > 320)
		Status.TempExtruderASet = 320;	
	
	if(DataNumber < 15) return;
	Status.TempExtruderBSet = C2Q(Buffer+12);
	if(Status.TempExtruderBSet > 320)
		Status.TempExtruderBSet = 320;	
	
	if(DataNumber < 18) return;
	Status.TempBasePlaneSet = C2Q(Buffer+15);
	if(Status.TempBasePlaneSet > 150)
		Status.TempBasePlaneSet = 150;
	
	if(DataNumber < 21) return;
	Status.PowerHeaterExtruderA = C2Q(Buffer+18);
	if(Status.PowerHeaterExtruderA > 100)
		Status.PowerHeaterExtruderA = 100;	
	
	if(DataNumber < 24) return;
	Status.PowerHeaterExtruderB = C2Q(Buffer+21);
	if(Status.PowerHeaterExtruderB > 100)
		Status.PowerHeaterExtruderB = 100;	
	
	if(DataNumber < 27) return;
	Status.PowerHeaterBasePlane = C2Q(Buffer+24);
	if(Status.PowerHeaterBasePlane > 100)
		Status.PowerHeaterBasePlane = 100;	
	
	if(DataNumber < 30) return;
	Status.PowerCoolerExtruderA = C2Q(Buffer+27);
	if(Status.PowerCoolerExtruderA > 100)
		Status.PowerCoolerExtruderA = 100;	
	
	if(DataNumber < 33) return;
	Status.PowerCoolerExtruderB = C2Q(Buffer+30);
	if(Status.PowerCoolerExtruderB > 100)
		Status.PowerCoolerExtruderB = 100;
	
	if(DataNumber < 36) return;
	Status.TempAir = C2Q(Buffer+33);
	if(Status.TempAir > 100)
		Status.TempAir = 100;
	
	if(DataNumber < 37) return;
	Status.StateExtruderA = (char)Buffer[36];
	
	if(DataNumber < 38) return;
	Status.StateExtruderB = (char)Buffer[37];
	
	if(DataNumber < 19) return;
	Status.StateBasePlane = (char)Buffer[38];
	
	if(DataNumber < 42) return;
	Status.PosiX = C2Q(Buffer+39);
	
	if(DataNumber < 45) return;
	Status.PosiY = C2Q(Buffer+42);
	
	if(DataNumber < 48) return;
	Status.PosiZ = C2Q(Buffer+45);
	
	if(DataNumber < 51) return;
	Status.PosiXMax = C2Q(Buffer+48);
	
	if(DataNumber < 54) return;
	Status.PosiYMax = C2Q(Buffer+51);
	
	if(DataNumber < 57) return;
	Status.PosiZMax = C2Q(Buffer+54);
	
	if(Status.PosiX > Status.PosiXMax)
		Status.PosiX = Status.PosiXMax;
	if(Status.PosiY > Status.PosiYMax)
		Status.PosiY = Status.PosiYMax;
	if(Status.PosiZ > Status.PosiZMax)
		Status.PosiZ = Status.PosiZMax;
	
	if(DataNumber < 60) return;
	Status.L1Used = C2Q(Buffer+57);
	
	if(DataNumber < 63) return;
	Status.L2Used = C2Q(Buffer+60);
	
	if(DataNumber < 66) return;
	Status.L1Max = C2Q(Buffer+63);
	
	if(DataNumber < 69) return;
	Status.L2Max = C2Q(Buffer+66);
	
	if(Status.L1Used > Status.L1Max)
		Status.L1Used = Status.L1Max;
	if(Status.L2Used > Status.L2Max)
		Status.L2Used = Status.L2Max;
	
	if(DataNumber < 70) return;
	Status.StateFA = (char)Buffer[69];
	
	if(DataNumber < 71) return;
	Status.StateIDP = (char)Buffer[70];
	
}


u32 STATUS::C2Q(u8 *data){
	static u32 weight[] = {1,1,10,100};
	u32 value = 0;
	for(u8 i=0;i<3;i++) //ת��Ϊ����
		value += (data[i]-'0')*weight[3-i];
	return value;
}
