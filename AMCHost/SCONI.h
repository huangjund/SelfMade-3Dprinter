#include "stm32f10x.h"

class SCONI{
	public:
		char Motion(double Position[5],float Vmax); //�ƶ�ָ��ӿ�
		char InitPosition(double Position[5]); //����ʼ��λ��
		char AutoHoming(); //�Զ���λ
	
		char MoveAxis(double Position,u8 Selection); //�ƶ���
	
		char SetNozzleState(u8 ExtA,u8 ExtB); //������ͷ״̬
	
		bool GetStatusBusy(); //���Ժ�̨�Ƿ�æ
	
		char GetTestPointA(); //��ȡ���Ե�A����
		char GetTestPointB(); //��ȡ���Ե�B����
		char GetTestPointC(); //��ȡ���Ե�C����
		char GetTestPointD(); //��ȡ���Ե�D����
		char GetTestPointM(); //��ȡ���Ե�M����
	
		char GetPositionX();
		char GetPositionY();
		char GetPositionZ();
		char GetPositionA();
		char GetPositionB();
	
		char GetPositionXMax();	
		char GetPositionYMax();	
		char GetPositionZMax();	
	
		char GetStatusNozzleA(); //��ȡ��ͷA״̬
		char GetStatusNozzleB(); //��ȡ��ͷB״̬
	
		char GetStatusFA();
		char GetStatusIDP();
	
		char GetStatusStepper();
		char GetStatusHomed();
	
		char GetStatusLeveling();
	
		char GetL1Rest();
		char GetL2Rest();
		char GetL1Max();
		char GetL2Max();
		
		char SetPosition(double Position,char Select);
		
		char EnableStepper();
		char DisableStepper();
		
		char EnableIDP();
		char DisableIDP();
		
		bool TestLink();
	private:
		char* Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign);
		double Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign);
	
};

extern class SCONI Sconi;

