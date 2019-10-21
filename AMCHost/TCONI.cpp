#include "TCONI.h"
#include "CBUS.h"
#include "Status.h"
#define s64 long long

TCONI Tconi;

char TCONI::GetTemperatureExtruderA(){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"GA",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderA = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'U';
}

char TCONI::GetTemperatureExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"GB",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderB = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'U';
}

char TCONI::GetTemperatureBasePlane(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"GG",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempBasePlane = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'U';
}

char TCONI::GetTemperatureEnvironment(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"GE",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempAir = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'U';
}

char TCONI::SetTemperatureEASetting(u16 T){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'S'; //ָ���ʶ
	
	*(TempPtr++) = 'A'; //ָ���ӱ�ʶ

	if(T > 320) T = 320;
	
	TempPtr = Flo2Chr(TempPtr,T,3,0,0);
	
	*TempPtr = 0; //������
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N')) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::SetTemperatureEBSetting(u16 T){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'S'; //ָ���ʶ
	
	*(TempPtr++) = 'B'; //ָ���ӱ�ʶ

	if(T > 320) T = 320;
	
	TempPtr = Flo2Chr(TempPtr,T,3,0,0);
	
	*TempPtr = 0; //������
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N')) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::SetTemperatureBPSetting(u16 T){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'S'; //ָ���ʶ
	
	*(TempPtr++) = 'G'; //ָ���ӱ�ʶ

	if(T > 150) T = 150;
	
	TempPtr = Flo2Chr(TempPtr,T,3,0,0);
	
	*TempPtr = 0; //������
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N')) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetTemperatureEASetting(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"TA",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderASet =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'U';
}

char TCONI::GetTemperatureEBSetting(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"TB",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderBSet =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'U';
}

char TCONI::GetTemperatureBPSetting(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"TG",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempBasePlaneSet =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'U';
}

char TCONI::SetCoolerPowerExtruderA(u8 P){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'C'; //ָ���ʶ
	
	*(TempPtr++) = 'A'; //ָ���ӱ�ʶ

	if(P > 100) P = 100;
	
	TempPtr = Flo2Chr(TempPtr,P,3,0,0);
	
	*TempPtr = 0; //������
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N')) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::SetCoolerPowerExtruderB(u8 P){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'C'; //ָ���ʶ
	
	*(TempPtr++) = 'B'; //ָ���ӱ�ʶ

	if(P > 100) P = 100;
	
	TempPtr = Flo2Chr(TempPtr,P,3,0,0);
	
	*TempPtr = 0; //������
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N')) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetCoolerPowerExtruderA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"RA",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerCoolerExtruderA =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetCoolerPowerExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"RB",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerCoolerExtruderB =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetHeaterPowerExtruderA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"HA",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerHeaterExtruderA =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetHeaterPowerExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"HB",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerHeaterExtruderB =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetHeaterPowerBasePlane(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"HG",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerHeaterBasePlane =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetStatusExtruderA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"EA",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateExtruderA = Cbus.Buffer[1];
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetStatusExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"EB",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateExtruderB = Cbus.Buffer[1];
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

char TCONI::GetStatusBasePlane(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"EG",100); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateBasePlane = Cbus.Buffer[1];
			return 'D';
		}
	}
	else //δ�յ��ظ�
		return 'T';
}

bool TCONI::TestLink(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //�ظ����5��
		ReplyReceived = Cbus.Transmit('T',"L",10); //���Ͳ���¼�ظ�״̬
		if(ReplyReceived && !Cbus.TestReply('R')){ //�յ��ظ���У��ɹ�
			if(!Cbus.TestReply('N')) //ָ����ȷ
				break;
		}
	}
	
	if(ReplyReceived){ //�յ��ظ�
		if(Cbus.TestReply('D')) return 1; //Done
		else if(Cbus.TestReply('R')) return 0; //Verifying Error
		else if(Cbus.TestReply('N')) return 0; //Instruction Error
		else return 0; //Unknown Status
	}
	else //δ�յ��ظ�
		return 0;	
}

char* TCONI::Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign){
	if(Int > 10) Int = 10; //��Χ����̫��
	if(Dec > 10) Dec = 10; //���Ȳ���̫��
	
	static s64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};
	
	if(Sign){ //��Ҫд�����
		if(Num >= 0) *(Buf++) = '+';
		else *(Buf++) = '-';
	}
	
	if(Num<0) Num = -Num; //ȡ����ֵ
	
	for(u32 i=0;i<Int;i++) //��������
		*(Buf++) = (s64)(Num)%Weight[Int-i+1]/Weight[Int-i] + '0';
	
	if(Dec) *(Buf++) = '.'; //С����(�����С��)

	for(u32 i=0;i<Dec;i++) //С������
		*(Buf++) = (s64)(Num*Weight[i+1])%10 + '0';
	
	return Buf;
}

double TCONI::Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign){
	if(Int > 10) Int = 10; //��Χ����̫��
	if(Dec > 10) Dec = 10; //���Ȳ���̫��
	
	double Temp = 0;
	double Symbol = 1;
	
	static double Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};
	
	if(Sign){ //�з���
		if(*(Buf++) == '-') Symbol = -1;
	}
	
	for(u32 i=0;i<Int;i++) //��������
		Temp += (*(Buf++)-'0')*Weight[Int-i];
	
	if(Dec) Buf++; //С����(�����С��)

	for(u32 i=0;i<Dec;i++) //С������
		Temp += (*(Buf++)-'0')/Weight[Int-i+1];
	
	Temp *= Symbol;
	
	return Temp;
}


