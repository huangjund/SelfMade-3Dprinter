#include "Interpreter.h"
#include "SCONI.h"
#include "TCONI.h"
#include "FCONI.h"
#include "AuxIO.h"
#include "Status.h"

#define BUFFER_SIZE 32
#define FACTOR_AXIS_A 1.15
#define FACTOR_AXIS_B 1.15

#define RESOLUTION_X 0.00625 //[mm]
#define RESOLUTION_Y 0.00625 //[mm]
#define RESOLUTION_Z 0.00625 //[mm]
#define RESOLUTION_A 0.00625 //[mm]
#define RESOLUTION_B 0.00625 //[mm]

INTERPRETER Interpreter;

void INTERPRETER::Init(){
	RoundStatus = 'I'; //Idel
	Signal = 0;
	ErrorStep = 0xFF;
	ErrorCode = 0;
	LoadIndex = 0;
	LaunchIndex = 0;
	RestBuffer = BUFFER_SIZE;
	for(u8 i=0;i<BUFFER_SIZE;i++){
		Buffer[i].Type = 0;
		Buffer[i].LineCounter = 0;
		Buffer[i].BaseAddress = 0;
		for(u8 k=0;k<5;k++){
			Buffer[i].Position[k] = 0;
			Buffer[i].AxisExis[k] = 0;
		}
		for(u8 k=0;k<2;k++){
			Buffer[i].ExtruderExis[i] = 0;
			Buffer[i].CoolerExis[i] = 0;
			Buffer[i].TemperatureE[i] = 0;
			Buffer[i].CoolerPower[i] = 0;
		}
		Buffer[i].BasePlaneExis = 0;
		Buffer[i].TemperatureB = 0;
	}
	for(u8 i=0;i<10;i++){
		Info.Warning[i][0] = 0xFF;
		Info.Warning[i][1] = 0;
	}
	Info.WarningLoad = 0;
	Info.Error[0] = 0xFF;
	Info.Error[1] = 0;
	for(u8 i=0;i<3;i++)
		Info.HMS[i] = 0;
	Info.CountEnable = 0;
	
	StreamStatus.InsCounter = 0;
	StreamStatus.VelocityPres = 20;
	for(u8 i=0;i<5;i++)
		StreamStatus.PositionPres[i] = 0;
	StreamStatus.ExtruderPres = 0;
	StreamStatus.AbsoluteCrod = 1;
}

char INTERPRETER::Start(){
	if(RoundStatus != 'I')
		return 'I';
	Reset();
	char Recv = PreCheck(); //���
	if(Recv == 'D'){ //��ȷ
		RoundStatus = 'G'; //����ȡָ��״̬
		Report.Used = 0;
	}
	return Recv;
}

char INTERPRETER::Stop(){
	if(RoundStatus == 'I') //����״̬
		return 'I';
	else //����״̬
		Signal = 'H'; //����ֹͣ�ź�
	return 'D';
}

char INTERPRETER::Pause(){
	if(RoundStatus == 'I') //����״̬
		return 'I';
	else //����״̬
		Signal = 'B'; //������ͣ�ź�
	return 'D';
}

char INTERPRETER::Resume(){
	if(RoundStatus == 'I')
		return 'I';
	if(RoundStatus != 'R' && RoundStatus != 'B')
		return 'K';
	else
		Signal = 'S'; //���ͼ����ź�
	return 'D';
}

char INTERPRETER::ReLoad(){
	if(RoundStatus == 'I') //����״̬
		return 'I';
	else //����״̬
		Signal = 'R'; //���ͻ����ź�
	return 'D';
}

void INTERPRETER::Round(){
	bool RoundError = 1;
	Info.CountEnable = 0;
	if(RoundStatus == 'I'){ //����״̬
		RoundError = 0;
		RoundStatus = 'I';
	}
	if(RoundStatus == 'G'){ //ȡָ��״̬
		RoundError = 0;
		if(!Status.CommandNumber)
			Info.Percent = 0;
		else if(StreamStatus.InsCounter*100/Status.CommandNumber > 99)
			Info.Percent = 99;
		else
			Info.Percent = StreamStatus.InsCounter*100/Status.CommandNumber;

		Info.CountEnable = 1;
		char ReValue = 'D';
		if(RestBuffer > 2){ //�����㹻��
			ReValue = GetInstruction(); //ȡָ��
			if(ReValue != 'D'){ //����
				ErrorStep = 0;
				ErrorCode = ReValue;
				RoundStatus = 'E';
			}
			else{
				RoundStatus = 'A';
			}
		}
		else{
			RoundStatus = 'P';
		}
	}
	if(RoundStatus == 'A'){ //ָ�����״̬
		RoundError = 0;
		Info.CountEnable = 1;
		LocalAnalyse(); //ָ�����
		RoundStatus = 'C'; 
	}
	if(RoundStatus == 'C'){ //״̬���״̬
		RoundError = 0;
		Info.CountEnable = 1;
		char ReValue = 'D';
		ReValue = StatusCheck(); //״̬���
		if(ReValue != 'D'){ //����
			ErrorStep = 1;
			ErrorCode = ReValue;
			RoundStatus = 'E';
		}
		else{
			RoundStatus = 'P';
		}
	}
	if(RoundStatus == 'P'){ //ִ��״̬
		RoundError = 0;
		Info.CountEnable = 1;
		char ReValue = 'D';
		ReValue = Process(); //ִ��
		if(ReValue != 'D'){ //����
			ErrorStep = 2;
			ErrorCode = ReValue;
			RoundStatus = 'E';
		}
		else{
			RoundStatus = 'S';
		}
	}
	if(RoundStatus == 'E'){ //������״̬
		RoundError = 0;
		Info.CountEnable = 1;
		if(ErrorStep == 0){ //ȡָ��ʱ����
			switch(ErrorCode){
				case 'E': //��ָ�� W@S
					if(Status.FilePointer != Status.FileSize){ //δ���ļ���β
						Info.Warning[Info.WarningLoad][0] = 0; //����λ��
						Info.Warning[Info.WarningLoad][1] = 'E'; //�������
						Info.WarningLoad = (Info.WarningLoad+1)%10; //��һ��
					}
					RoundStatus = 'A'; //����ִ��
					break; 
				case 'R': //У��� E@B
					Info.Error[0] = 0; //����λ��
					Info.Error[1] = 'R'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				default: //û�д���
					RoundStatus = 'A'; //����ִ��
					break;
			}
		}
		else if(ErrorStep == 1){ //״̬���ʱ����
			switch(ErrorCode){
				case 'F': //�ļ������� E@B
					Info.Error[0] = 1; //����λ��
					Info.Error[1] = 'F'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case 'E': //����� W@S
					if(Status.FilePointer != Status.FileSize){ //δ���ļ���β
						Info.Warning[Info.WarningLoad][0] = 1; //����λ��
						Info.Warning[Info.WarningLoad][1] = 'E'; //�������
						Info.WarningLoad = (Info.WarningLoad+1)%10; //��һ��
					}
					RoundStatus = 'P'; //����ִ��
					break;
				case 'H': //δ��λ E@B
					Info.Error[0] = 1; //����λ��
					Info.Error[1] = 'H'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case 'A': //Aȱ�� E@R
					Info.Error[0] = 1; //����λ��
					Info.Error[1] = 'A'; //�������
					Signal = 'R';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case 'B': //Bȱ�� E@R
					Info.Error[0] = 1; //����λ��
					Info.Error[1] = 'B'; //�������
					Signal = 'R';
					RoundStatus = 'S'; //��ִͣ��
					break;
				default: //û�д���
					RoundStatus = 'P';
					break;
			}
		}
		else if(ErrorStep == 2){ //ִ��ʱ����
			switch(ErrorCode){
				case 'E': //����� W@S
					if(Status.FilePointer != Status.FileSize){ //δ���ļ���β
						Info.Warning[Info.WarningLoad][0] = 2; //����λ��
						Info.Warning[Info.WarningLoad][1] = 'E'; //�������
						Info.WarningLoad = (Info.WarningLoad+1)%10; //��һ��
					}
					RoundStatus = 'S'; //����ִ��
					break;
				case 'H': //δ��λ E@B
					Info.Error[0] = 2; //����λ��
					Info.Error[1] = 'H'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case 'R': //ͨ��У����� E@B
					Info.Error[0] = 2; //����λ��
					Info.Error[1] = 'R'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case 'N': //ͨ�Ŵ��� E@B
					Info.Error[0] = 2; //����λ��
					Info.Error[1] = 'N'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case 'T': //ͨ�ų�ʱ E@B
					Info.Error[0] = 2; //����λ��
					Info.Error[1] = 'T'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case 'U': //����ԭ��ͨ�Ŵ� E@B
					Info.Error[0] = 2; //����λ��
					Info.Error[1] = 'U'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				case '2': //�¶����ù��� W@S
					Info.Warning[Info.WarningLoad][0] = 2; //����λ��
					Info.Warning[Info.WarningLoad][1] = '2'; //�������
					Info.WarningLoad = (Info.WarningLoad+1)%10; //��һ��
					RoundStatus = 'S'; //����ִ��
					break;
				case '1': //�¶����ù��� W@S
					Info.Warning[Info.WarningLoad][0] = 2; //����λ��
					Info.Warning[Info.WarningLoad][1] = '1'; //�������
					Info.WarningLoad = (Info.WarningLoad+1)%10; //��һ��
					RoundStatus = 'S'; //����ִ��
					break;
				case 'X': //�¶ȼƹ��� E@B
					Info.Error[0] = 2; //����λ��
					Info.Error[1] = 'X'; //�������
					Signal = 'B';
					RoundStatus = 'S'; //��ִͣ��
					break;
				default: //û�д���
					RoundStatus = 'S';
					break;
			}
		}
		else{ //û�д�
			RoundStatus = 'S';
		}
	}
	if(RoundStatus == 'S'){ //�źŴ���״̬
		RoundError = 0;
		Info.CountEnable = 1;
		switch(Signal){
			case 'B': //��ͣ�ź�
				Sconi.GetStatusFA();
				if(Status.StateFA == 'W' || Status.StateFA == 'S')
					RoundStatus = 'S';
				else{
					bool Success = 1;
					if(Sconi.GetPositionX() != 'D') Success = 0;
					if(Sconi.GetPositionY() != 'D') Success = 0;
					if(Sconi.GetPositionZ() != 'D') Success = 0;
					if(Sconi.GetPositionA() != 'D') Success = 0;
					if(Sconi.GetPositionB() != 'D') Success = 0;
					if(Success){
						PrePosition[0] = Status.PosiX;
						PrePosition[1] = Status.PosiY;
						PrePosition[2] = Status.PosiZ;
						PrePosition[3] = Status.PosiA;
						PrePosition[4] = Status.PosiB;
						double Posi[5] = {0,0,0,0,0};
						for(u8 i=0;i<5;i++)
							Posi[i] = PrePosition[i];
						if(PrePosition[2]+10 > 420)
							Posi[2] = 420;
						else
							Posi[2] = PrePosition[2]+10;
						Sconi.Motion(Posi,20);
						Signal = 0; //����ź�
						RoundStatus = 'B'; //������ͣ״̬
					}
					else{
						RoundStatus = 'S';						
					}
				}
				break;
			case 'H': //ֹͣ�ź�
				Signal = 0; //����ź�
				RoundStatus = 'H'; //����ֹͣ״̬
				break;
			case 'R': //�����ź�
				Sconi.GetStatusFA();
				if(Status.StateFA == 'W' || Status.StateFA == 'S')
					RoundStatus = 'S';
				else{
					bool Success = 1;
					if(Sconi.GetPositionX() != 'D') Success = 0;
					if(Sconi.GetPositionY() != 'D') Success = 0;
					if(Sconi.GetPositionZ() != 'D') Success = 0;
					if(Sconi.GetPositionA() != 'D') Success = 0;
					if(Sconi.GetPositionB() != 'D') Success = 0;
					if(Success){
						PrePosition[0] = Status.PosiX;
						PrePosition[1] = Status.PosiY;
						PrePosition[2] = Status.PosiZ;
						PrePosition[3] = Status.PosiA;
						PrePosition[4] = Status.PosiB;
						double Posi[5] = {0,0,0,0,0};
						for(u8 i=0;i<5;i++)
							Posi[i] = PrePosition[i];
						if(PrePosition[2]+10 > 420)
							Posi[2] = 420;
						else
							Posi[2] = PrePosition[2]+10;
						Sconi.Motion(Posi,20);
						Signal = 0; //����ź�
						RoundStatus = 'R'; //���뻻��״̬
					}
					else{
						RoundStatus = 'S';						
					}
				}
				break;
			default: //û���ź�
				Signal = 0; //����ź�
				if(RestBuffer != BUFFER_SIZE) //��������ʣ��
					RoundStatus = 'G'; //����ȡָ��״̬
				else if(Status.FilePointer == Status.FileSize) //��������ļ���ȡ��
					RoundStatus = 'H'; //����ֹͣ״̬
				else //����յ��ļ�δ��ȡ��
					RoundStatus = 'G'; //����ȡָ��״̬
				break;
		}
	}
	if(RoundStatus == 'R'){ //����״̬
		RoundError = 0;
		Info.CountEnable = 0;
		switch(Signal){
			case 'S': //�˳�����״̬
				if(Sconi.Motion(PrePosition,20) == 'D'){
					Signal = 0; //����ź�
					RoundStatus = 'S'; //�����źŴ���״̬		
				}
				else{
					RoundStatus = 'R';
				}
				break;
			case 'B': //��ͣ�ź�
				Signal  = 0; //����ź�
				RoundStatus = 'B'; //������ͣ״̬
				break;
			case 'H': //ֹͣ�ź�
				Signal = 0; //����ź�
				RoundStatus = 'H'; //����ֹͣ״̬
				break;
			default:
				Signal = 0; //����ź�
				RoundStatus = 'R'; //���ֻ���״̬
		}
	}
	if(RoundStatus == 'B'){ //��ͣ״̬
		RoundError = 0;
		Info.CountEnable = 0;
		switch(Signal){
			case 'S': //�˳���ͣ״̬
				if(Sconi.Motion(PrePosition,20) == 'D'){
					Signal = 0; //����ź�
					RoundStatus = 'S'; //�����źŴ���״̬		
				}
				else{
					RoundStatus = 'R';
				}
				break;
			case 'R': //�����ź�
				Signal  = 0; //����ź�
				RoundStatus = 'R'; //���뻻��״̬
				break;
			case 'H': //ֹͣ�ź�
				Signal = 0; //����ź�
				RoundStatus = 'H'; //����ֹͣ״̬
				break;
			default:
				Signal = 0; //����ź�
				RoundStatus = 'B'; //������ͣ״̬
		}
	}
	if(RoundStatus == 'H'){ //ֹͣ״̬
		RoundError = 0;
		Info.CountEnable = 0;
		for(u8 i=0;i<3;i++)
			Report.HMS[i] = Info.HMS[i];
		Report.InsCounter = StreamStatus.InsCounter;
		Report.Used = 1;
		Reset(); //��λ
		RoundStatus = 'I'; //�������״̬
	}
	if(RoundError){ //����
		RoundStatus = 'I'; //�������״̬
	}
}

void INTERPRETER::Tick(){ //��ʱ
	if(Info.CountEnable){
		Info.HMS[2]++;
		if(Info.HMS[2] >= 60){
			Info.HMS[2] = 0;
			Info.HMS[1]++;
		}
		if(Info.HMS[1] >= 60){
			Info.HMS[1] = 0;
			Info.HMS[0]++;
		}
	}
}

void INTERPRETER::Reset(){
	RoundStatus = 'I'; //Idel
	Signal = 0;
	ErrorStep = 0xFF;
	ErrorCode = 0;
	LoadIndex = 0;
	LaunchIndex = 0;
	RestBuffer = BUFFER_SIZE;
	for(u8 i=0;i<BUFFER_SIZE;i++){
		Buffer[i].Type = 0;
		Buffer[i].LineCounter = 0;
		Buffer[i].BaseAddress = 0;
		for(u8 k=0;k<5;k++){
			Buffer[i].Position[k] = 0;
			Buffer[i].AxisExis[k] = 0;
		}
		for(u8 k=0;k<2;k++){
			Buffer[i].ExtruderExis[i] = 0;
			Buffer[i].CoolerExis[i] = 0;
			Buffer[i].TemperatureE[i] = 0;
			Buffer[i].CoolerPower[i] = 0;
		}
		Buffer[i].BasePlaneExis = 0;
		Buffer[i].TemperatureB = 0;
	}
	for(u8 i=0;i<10;i++){
		Info.Warning[i][0] = 0xFF;
		Info.Warning[i][1] = 0;
	}
	Info.WarningLoad = 0;
	Info.Error[0] = 0xFF;
	Info.Error[1] = 0;
	for(u8 i=0;i<3;i++)
		Info.HMS[i] = 0;
	Info.Percent = 0;
	Info.CountEnable = 0;

	StreamStatus.InsCounter = 0;
	StreamStatus.VelocityPres = 20;
	for(u8 i=0;i<5;i++)
		StreamStatus.PositionPres[i] = 0;
	StreamStatus.ExtruderPres = 0;
	StreamStatus.AbsoluteCrod = 1;
	
}

char INTERPRETER::PreCheck(){
	if(RoundStatus != 'I') return 'K'; //��æ	
	if(!Sconi.TestLink()) return '1'; //����ϵͳ����
	if(!Tconi.TestLink()) return '2'; //����ϵͳ����
	if(!Fconi.TestLink()) return '3'; //�ļ�ϵͳ����
	if(!AuxIO.TestLink()) return '4'; //����ϵͳ����
	Sconi.GetStatusHomed();
	if(!Status.StateHomed) return 'H'; //δ��λ
	Fconi.FileOpen();
	if(!Status.FileExist) return 'F'; //�ļ�������
	
	if(Sconi.GetPositionX() != 'D') return '1';
	if(Sconi.GetPositionY() != 'D') return '1';
	if(Sconi.GetPositionZ() != 'D') return '1';
	if(Sconi.GetPositionA() != 'D') return '1';
	if(Sconi.GetPositionB() != 'D') return '1';	
	
	StreamStatus.PositionPres[0] = Status.PosiX;
	StreamStatus.PositionPres[1] = Status.PosiY;
	StreamStatus.PositionPres[2] = Status.PosiZ;
	StreamStatus.PositionPres[3] = Status.PosiA;
	StreamStatus.PositionPres[4] = Status.PosiB;
	
	return 'D';
}

char INTERPRETER::GetInstruction(){
	//G0 G1 G28 G90 G91 G92
	//M104 M106 M107 M109 M140 M190
	TempIns.BaseAddress = 0;
	TempIns.GM_Exis = 0; TempIns.GM_Para = 0;
	TempIns.T_Exis = 0; TempIns.T_Para = 0;
	TempIns.S_Exis = 0; TempIns.S_Para = 0;
	TempIns.P_Exis = 0; TempIns.P_Para = 0;
	TempIns.X_Exis = 0; TempIns.X_Para = 0;
	TempIns.Y_Exis = 0; TempIns.Y_Para = 0;
	TempIns.Z_Exis = 0; TempIns.Z_Para = 0;
	TempIns.A_Exis = 0; TempIns.A_Para = 0;
	TempIns.B_Exis = 0; TempIns.B_Para = 0;
	TempIns.F_Exis = 0; TempIns.F_Para = 0;
	TempIns.V_Exis = 0; TempIns.V_Para = 0;
	for(u8 i=0;i<128;i++)
		TempData[i] = 0;
	
//	if(RestBuffer < 2)
//		return 'K';
	
	Fconi.GetFilePointer(); //��ȡ�ļ�ָ��
	TempIns.BaseAddress = Status.FilePointer; //��¼�ļ�ָ��
	Fconi.FileReadLine(128,TempData); //��ȡһ��ָ��
	
	if(!TempData[0]) return 'E'; //��ָ��

	for(u8 k=0;k<128&&TempData[k];k++){ //��ȡָ�����
		if(TempData[k] == 'G'){
			u8 Temp = AbsDecimal(CharToDecimal(TempData+k+1));
			if(Temp==0 || Temp==1) Buffer[LoadIndex].Type = 1; //�ƶ�
			if(Temp == 28) TempIns.GM_Para = 28; //��λ
			if(Temp == 90) TempIns.GM_Para = 90; //��������
			if(Temp == 91) TempIns.GM_Para = 91; //�������
			if(Temp == 92) TempIns.GM_Para = 92; //����
			TempIns.GM_Exis = 1;
		}
		if(TempData[k] == 'M'){
			u8 Temp = AbsDecimal(CharToDecimal(TempData+k+1));
			if(Temp == 104) TempIns.GM_Para = 104; //���ü������¶�
			if(Temp == 106) TempIns.GM_Para = 106; //���÷���״̬
			if(Temp == 107) TempIns.GM_Para = 107; //�رշ���
			if(Temp == 109) TempIns.GM_Para = 109; //���ü������¶�/����
			if(Temp == 140) TempIns.GM_Para = 140; //���û�׼���¶�
			if(Temp == 190) TempIns.GM_Para = 190; //���û�׼���¶�/����
			TempIns.GM_Exis = 1;
		}
		if(TempData[k] == 'T'){
			TempIns.T_Para = AbsDecimal(CharToDecimal(TempData+k+1));
			TempIns.T_Exis = 1;
		}	
		if(TempData[k] == 'S'){
			TempIns.S_Para = AbsDecimal(CharToDecimal(TempData+k+1));
			TempIns.S_Exis = 1;
		}	
		if(TempData[k] == 'P'){
			TempIns.P_Para = AbsDecimal(CharToDecimal(TempData+k+1));
			TempIns.P_Exis = 1;
		}
		if(TempData[k] == 'X'){
			TempIns.X_Para = CharToDouble(TempData+k+1);
			AxisRound(TempIns.X_Para,RESOLUTION_X);
			TempIns.X_Exis = 1;
		}
		if(TempData[k] == 'Y'){
			TempIns.Y_Para = CharToDouble(TempData+k+1);
			AxisRound(TempIns.Y_Para,RESOLUTION_Y);
			TempIns.Y_Exis = 1;
		}	
		if(TempData[k] == 'Z'){
			TempIns.Z_Para = CharToDouble(TempData+k+1);
			AxisRound(TempIns.Z_Para,RESOLUTION_Z);
			TempIns.Z_Exis = 1;
		}
		if(TempData[k] == 'A'){
			TempIns.A_Para = CharToDouble(TempData+k+1);
			TempIns.A_Para *= FACTOR_AXIS_A;
			AxisRound(TempIns.A_Para,RESOLUTION_A);
			TempIns.A_Exis = 1;
		}
		if(TempData[k] == 'B'){
			TempIns.B_Para = CharToDouble(TempData+k+1);
			TempIns.B_Para *= FACTOR_AXIS_B;
			AxisRound(TempIns.B_Para,RESOLUTION_B);
			TempIns.B_Exis = 1;
		}
		if(TempData[k] == 'E'){
			TempIns.E_Para = CharToDouble(TempData+k+1);
			if(!StreamStatus.ExtruderPres){
				TempIns.E_Para *= FACTOR_AXIS_A;
				AxisRound(TempIns.E_Para,RESOLUTION_A);
			}
			else{
				TempIns.E_Para *= FACTOR_AXIS_B;
				AxisRound(TempIns.E_Para,RESOLUTION_B);
			}
			TempIns.E_Exis = 1;
		}
		if(TempData[k] == 'F'){
			TempIns.F_Para = CharToDouble(TempData+k+1);
			TempIns.F_Exis = 1;
		}
		if(TempData[k] == '*'){
			TempIns.V_Para = AbsDecimal(CharToDecimal(TempData+k+1));
			TempIns.V_Exis = 1;
		}
	}
	
	u32 Verify = 0; //У���
	for(u8 i=0;i<128&&TempData[i];i++){
		if(TempData[i] == '*') break;
		Verify += TempData[i];
	}
	
	if(Verify%100==TempIns.V_Para || !TempIns.V_Exis)
		return 'D'; //У��ɹ�
	else
		return 'R'; //У��ʧ��
}

void INTERPRETER::LocalAnalyse(){
	if(TempIns.GM_Exis){ //Gx Mx
		if(TempIns.GM_Para==0 || TempIns.GM_Para==1){ //G0 G1�ƶ�ָ��
			Buffer[LoadIndex].Type = 1; //ָ����
			Buffer[LoadIndex].Segment = 0; //�ֶα��
			for(u8 i=0;i<5;i++) //���ô����������
				Buffer[LoadIndex].AxisExis[i] = 1;
			
			if(TempIns.X_Exis){ //��ָ��X�������
				if(StreamStatus.AbsoluteCrod){ //�ļ�ʹ�þ�������
					StreamStatus.PositionPres[0] = TempIns.X_Para; //��¼X����					
					Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0]; //д��X����
				}
				else{ //�ļ�ʹ���������
					StreamStatus.PositionPres[0] += TempIns.X_Para; //����X����
					Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0]; //д��X����
				}
			}
			else{ //δָ��X�������
				Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0]; //д��X����
			}
			if(TempIns.Y_Exis){ //��ָ��Y�������
				if(StreamStatus.AbsoluteCrod){ //�ļ�ʹ�þ�������
					StreamStatus.PositionPres[1] = TempIns.Y_Para; //��¼Y����					
					Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1]; //д��Y����
				}
				else{ //�ļ�ʹ���������
					StreamStatus.PositionPres[1] += TempIns.Y_Para; //����Y����
					Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1]; //д��Y����
				}
			}
			else{ //δָ��Y�������
				Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1]; //д��Y����
			}
			if(TempIns.Z_Exis){ //��ָ��Z�������
				if(StreamStatus.AbsoluteCrod){ //�ļ�ʹ�þ�������
					StreamStatus.PositionPres[2] = TempIns.Z_Para; //��¼Z����					
					Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2]; //д��Z����
				}
				else{ //�ļ�ʹ���������
					StreamStatus.PositionPres[2] += TempIns.Z_Para; //����Z����
					Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2]; //д��Z����
				}
			}
			else{ //δָ��Z�������
				Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2]; //д��Z����
			}
			if(TempIns.A_Exis){ //��ָ��A�������
				if(StreamStatus.AbsoluteCrod){ //�ļ�ʹ�þ�������
					StreamStatus.PositionPres[3] = TempIns.A_Para; //��¼A����					
					Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //д��A����
				}
				else{ //�ļ�ʹ���������
					StreamStatus.PositionPres[3] += TempIns.A_Para; //����A����
					Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //д��A����
				}
			}
			else{ //δָ��A�������
				Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //д��A����
			}
			if(TempIns.B_Exis){ //��ָ��B�������
				if(StreamStatus.AbsoluteCrod){ //�ļ�ʹ�þ�������
					StreamStatus.PositionPres[4] = TempIns.B_Para; //��¼B����					
					Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //д��B����
				}
				else{ //�ļ�ʹ���������
					StreamStatus.PositionPres[4] += TempIns.B_Para; //����B����
					Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //д��B����
				}
			}
			else{ //δָ��B�������
				Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //д��B����
			}
			if(!TempIns.A_Exis && !TempIns.B_Exis && TempIns.E_Exis){ //��ָ��A/B�������
				if(!StreamStatus.ExtruderPres){ //��ǰʹ�ü�����A
					if(StreamStatus.AbsoluteCrod){ //�ļ�ʹ�þ�������
						StreamStatus.PositionPres[3] = TempIns.E_Para; //��¼A����					
						Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //д��A����
					}
					else{ //�ļ�ʹ���������
						StreamStatus.PositionPres[3] += TempIns.E_Para; //����A����
						Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //д��A����
					}
				}
				else{ //��ǰʹ�ü�����B
					if(StreamStatus.AbsoluteCrod){ //�ļ�ʹ�þ�������
						StreamStatus.PositionPres[4] = TempIns.E_Para; //��¼B����					
						Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //д��B����
					}
					else{ //�ļ�ʹ���������
						StreamStatus.PositionPres[4] += TempIns.E_Para; //����B����
						Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //д��B����
					}
				}
			}
			if(TempIns.F_Exis){ //��ָ��F����
				StreamStatus.VelocityPres = TempIns.F_Para/60; //��¼�ٶ�
				if(StreamStatus.VelocityPres < 10) //�ٶȲ�����10mm/s
					StreamStatus.VelocityPres = 10;
				Buffer[LoadIndex].Velocity = StreamStatus.VelocityPres; //д���ٶ�
			}
			else{ //δָ��F����
				if(StreamStatus.VelocityPres < 10) //�ٶȲ�����10mm/s
					StreamStatus.VelocityPres = 10;
				Buffer[LoadIndex].Velocity = StreamStatus.VelocityPres; //д���ٶ�
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
		}
		else if(TempIns.GM_Para == 28){ //G28��λָ��
			Buffer[LoadIndex].Type = 28; //ָ����
			Buffer[LoadIndex].Segment = 0; //�ֶα��
			for(u8 i=0;i<5;i++) //��������������
				Buffer[LoadIndex].AxisExis[i] = 0;
			
			bool AllAxis = 1;
			bool ABExis = 0;
			if(TempIns.X_Exis){ //��ָ����λX��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[0] = 1; //���ô����������
				Buffer[LoadIndex].Position[0] = 0; //X����
				StreamStatus.PositionPres[0] = 0; //���X����
			}else{ //δָ����λX��
				Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0];
			}
			if(TempIns.Y_Exis){ //��ָ����λY��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[1] = 1; //���ô����������
				Buffer[LoadIndex].Position[1] = 0;
				StreamStatus.PositionPres[1] = 0; //���Y����
			}else{ //δָ����λY��
				Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1];
			}
			if(TempIns.Z_Exis){ //��ָ����λZ��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[2] = 1; //���ô����������
				Buffer[LoadIndex].Position[2] = 0;
				StreamStatus.PositionPres[2] = 0; //���Z����
			}else{ //δָ����λZ��
				Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2];
			}
			if(TempIns.A_Exis){ //��ָ����λA��
				AllAxis = 0;
				ABExis = 1;
				Buffer[LoadIndex].AxisExis[3] = 1; //���ô����������
				Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3];
				StreamStatus.PositionPres[3] = 0; //���A����
			}
			if(TempIns.B_Exis){ //��ָ����λB��
				AllAxis = 0;
				ABExis = 1;
				Buffer[LoadIndex].AxisExis[4] = 1; //���ô����������
				Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4];
				StreamStatus.PositionPres[4] = 0; //���B����
			}
			if(TempIns.E_Exis){ //��ָ����λA/B��
				AllAxis = 0;
				ABExis = 1;
				if(!StreamStatus.ExtruderPres){ //������A
					Buffer[LoadIndex].AxisExis[3] = 1; //���ô����������
					Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3];
					StreamStatus.PositionPres[3] = 0; //���A����
				}
				else{ //������B
					Buffer[LoadIndex].AxisExis[4] = 1; //���ô����������
					Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4];
					StreamStatus.PositionPres[4] = 0; //���B����
				}
			}
			if(AllAxis){ //��λ������
				ABExis = 1;
				for(u8 i=0;i<3;i++){
					Buffer[LoadIndex].AxisExis[i] = 1; //���ô���������� 
					Buffer[LoadIndex].Position[i] = 0;
					StreamStatus.PositionPres[i] = 0; //�������
				}
				for(u8 i=3;i<5;i++){
					Buffer[LoadIndex].AxisExis[i] = 1; //���ô����������
					Buffer[LoadIndex].Position[i] = StreamStatus.PositionPres[i];
					StreamStatus.PositionPres[i] = 0; //�������
				}	
			}
			Buffer[LoadIndex].Velocity = 30;
			
			u8 PreIndex = LoadIndex; //ǰһ������
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
			
			if(ABExis){ //��Ҫ�ֶ�ִ��
				Buffer[LoadIndex].Type = Buffer[PreIndex].Type; //���Ʊ��
				Buffer[LoadIndex].LineCounter = Buffer[PreIndex].LineCounter; //�����к�
				Buffer[LoadIndex].BaseAddress = Buffer[PreIndex].BaseAddress; //���ƻ�ַ
				Buffer[LoadIndex].Segment = 1; //�ֶα��
				for(u8 i=0;i<5;i++){ //��������
					Buffer[LoadIndex].AxisExis[i] = Buffer[PreIndex].AxisExis[i];
					Buffer[LoadIndex].Position[i] = Buffer[PreIndex].Position[i];
				}
				Buffer[LoadIndex].Velocity = Buffer[PreIndex].Velocity; //�����ٶ�
				LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
				RestBuffer--; //ʣ�໺������-1
			}
		}
		else if(TempIns.GM_Para == 90){ //G90ʹ�þ�������
			StreamStatus.AbsoluteCrod = 1; //����ʹ�þ�������
		}
		else if(TempIns.GM_Para == 91){ //G91ʹ���������
			StreamStatus.AbsoluteCrod = 0; //����ʹ���������
		}
		else if(TempIns.GM_Para == 92){ //G92��������
			Buffer[LoadIndex].Type = 92; //ָ����
			Buffer[LoadIndex].Segment = 0; //�ֶα��
			for(u8 i=0;i<5;i++){ //��������������
				Buffer[LoadIndex].AxisExis[i] = 0;
				Buffer[LoadIndex].Position[i] = 0;
			}
			bool AllAxis = 1;
			if(TempIns.X_Exis){ //��ָ������X��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[0] = 1; //���ô����������
				Buffer[LoadIndex].Position[0] = TempIns.X_Para; //����X����
				StreamStatus.PositionPres[0] = TempIns.X_Para; //��¼X����
			}
			if(TempIns.Y_Exis){ //��ָ������Y��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[1] = 1; //���ô����������
				Buffer[LoadIndex].Position[1] = TempIns.Y_Para; //����Y����
				StreamStatus.PositionPres[1] = TempIns.Y_Para; //��¼Y����
			}
			if(TempIns.Z_Exis){ //��ָ������Z��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[2] = 1; //���ô����������
				Buffer[LoadIndex].Position[2] = TempIns.Z_Para; //����Z����
				StreamStatus.PositionPres[2] = TempIns.Z_Para; //��¼Z����
			}
			if(TempIns.A_Exis){ //��ָ������A��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[3] = 1; //���ô����������
				Buffer[LoadIndex].Position[3] = TempIns.A_Para; //����A����
				StreamStatus.PositionPres[3] = TempIns.A_Para; //��¼A����
			}
			if(TempIns.B_Exis){ //��ָ������B��
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[4] = 1; //���ô����������
				Buffer[LoadIndex].Position[4] = TempIns.B_Para; //����B����
				StreamStatus.PositionPres[4] = TempIns.B_Para; //��¼B����
			}
			if(TempIns.E_Exis){ //��ָ������A/B��
				AllAxis = 0;
				if(!StreamStatus.ExtruderPres){ //������A
					Buffer[LoadIndex].AxisExis[3] = 1; //���ô����������
					Buffer[LoadIndex].Position[3] = TempIns.E_Para; //����A����
					StreamStatus.PositionPres[3] = TempIns.E_Para; //��¼A����
				}
				else{ //������B
					Buffer[LoadIndex].AxisExis[4] = 1; //���ô����������
					Buffer[LoadIndex].Position[4] = TempIns.E_Para; //����B����
					StreamStatus.PositionPres[4] = TempIns.E_Para; //��¼B����
				}
			}
			if(AllAxis){ //����������Ϊ0
				for(u8 i=0;i<5;i++){
					Buffer[LoadIndex].AxisExis[i] = 1; //���ô����������
					Buffer[LoadIndex].Position[i] = 0; //��������
					StreamStatus.PositionPres[i] = 0; //�������
				} 
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
		}
		else if(TempIns.GM_Para == 104){ //M104���ü������¶�/������
			Buffer[LoadIndex].Type = 104; //ָ����
			Buffer[LoadIndex].Segment = 0; //�ֶα��
			Buffer[LoadIndex].ExtruderExis[0] = 0;
			Buffer[LoadIndex].TemperatureE[0] = 0;
			Buffer[LoadIndex].ExtruderExis[1] = 0;
			Buffer[LoadIndex].TemperatureE[1] = 0;
			
			if(TempIns.T_Exis){ //��ָ��������
				if(!TempIns.T_Para){ //������A
					StreamStatus.ExtruderPres = 0; //��ǰʹ�ü�����A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //ѡ�񼷳���A
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[0] = 0; //���ѡ��
				}
				else{ //������B
					StreamStatus.ExtruderPres = 1; //��ǰʹ�ü�����B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //ѡ�񼷳���B
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[1] = 0; //���ѡ��	
				}
			}
			else{ //δָ��������
				if(!StreamStatus.ExtruderPres){ //������A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //ѡ�񼷳���A
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[0] = 0; //���ѡ��
				}
				else{ //������B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //ѡ�񼷳���B
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[1] = 0; //���ѡ��	
				}
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
		}
		else if(TempIns.GM_Para == 106){ //M106���÷���״̬
			Buffer[LoadIndex].Type = 106;
			Buffer[LoadIndex].Segment = 0; //�ֶα��
			Buffer[LoadIndex].CoolerExis[0] = 0;
			Buffer[LoadIndex].CoolerExis[1] = 0;
			Buffer[LoadIndex].CoolerPower[0] = 0;
			Buffer[LoadIndex].CoolerPower[1] = 0;
			
			if(TempIns.T_Exis){ //��ָ������
				if(!TempIns.T_Para){ //����A
					Buffer[LoadIndex].CoolerExis[0] = 1; //ѡ�����A
					if(TempIns.P_Exis) //ʹ��P����ָ��״̬
						Buffer[LoadIndex].CoolerPower[0] = TempIns.P_Para*20/51; //д��״̬
					else if(TempIns.S_Exis) //ʹ��S����ָ��״̬
						Buffer[LoadIndex].CoolerPower[0] = TempIns.S_Para*20/51; //д��״̬
					else //δָ��״̬
						Buffer[LoadIndex].CoolerExis[0] = 0; //���ѡ��
				}
				else{ //����B
					Buffer[LoadIndex].CoolerExis[1] = 1; //ѡ�����B
					if(TempIns.P_Exis) //ʹ��P����ָ��״̬
						Buffer[LoadIndex].CoolerPower[1] = TempIns.P_Para*20/51; //д��״̬
					else if(TempIns.S_Exis) //ʹ��S����ָ��״̬
						Buffer[LoadIndex].CoolerPower[1] = TempIns.S_Para*20/51; //д��״̬
					else //δָ��״̬
						Buffer[LoadIndex].CoolerExis[1] = 0; //���ѡ��	
				}
			}
			else{ //δָ������
				Buffer[LoadIndex].CoolerExis[0] = 1; //ѡ�����A
				Buffer[LoadIndex].CoolerExis[1] = 1; //ѡ�����B
				if(TempIns.P_Exis){ //ʹ��P����ָ��״̬
					Buffer[LoadIndex].CoolerPower[0] = TempIns.P_Para*20/51; //д��״̬
					Buffer[LoadIndex].CoolerPower[1] = TempIns.P_Para*20/51; //д��״̬
				}
				else if(TempIns.S_Exis){ //ʹ��S����ָ��״̬
					Buffer[LoadIndex].CoolerPower[0] = TempIns.S_Para*20/51; //д��״̬
					Buffer[LoadIndex].CoolerPower[1] = TempIns.P_Para*20/51; //д��״̬
				}
				else{ //δָ��״̬
					Buffer[LoadIndex].CoolerExis[0] = 0; //���ѡ��
					Buffer[LoadIndex].CoolerExis[1] = 0; //���ѡ��
				}
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
		}
		else if(TempIns.GM_Para == 107){ //M107�رշ���
			Buffer[LoadIndex].Type = 107;
			Buffer[LoadIndex].Segment = 0; //�ֶα��
			Buffer[LoadIndex].CoolerExis[0] = 0;
			Buffer[LoadIndex].CoolerExis[1] = 0;
			Buffer[LoadIndex].CoolerPower[0] = 0;
			Buffer[LoadIndex].CoolerPower[1] = 0;
			
			if(TempIns.T_Exis){ //��ָ������
				if(!TempIns.T_Para) //����A
					Buffer[LoadIndex].CoolerExis[0] = 1; //ѡ�����A
				else //����B
					Buffer[LoadIndex].CoolerExis[1] = 1; //ѡ�����B
			}
			else{ //δָ������
				Buffer[LoadIndex].CoolerExis[0] = 1; //ѡ�����A
				Buffer[LoadIndex].CoolerExis[1] = 1; //ѡ�����B
			}

			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��	
			RestBuffer--; //ʣ�໺������-1			
		}
		else if(TempIns.GM_Para == 109){ //M109���ü������¶�/����
			Buffer[LoadIndex].Type = 109; //ָ����
			Buffer[LoadIndex].Segment = 0; //�ֶα��
			Buffer[LoadIndex].ExtruderExis[0] = 0;
			Buffer[LoadIndex].TemperatureE[0] = 0;
			Buffer[LoadIndex].ExtruderExis[1] = 0;
			Buffer[LoadIndex].TemperatureE[1] = 0;
			
			if(TempIns.T_Exis){ //��ָ��������
				if(!TempIns.T_Para){ //������A
					StreamStatus.ExtruderPres = 0; //��ǰʹ�ü�����A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //ѡ�񼷳���A
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[0] = 0; //���ѡ��
				}
				else{ //������B
					StreamStatus.ExtruderPres = 1; //��ǰʹ�ü�����B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //ѡ�񼷳���B
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[1] = 0; //���ѡ��	
				}
			}
			else{ //δָ��������
				if(!StreamStatus.ExtruderPres){ //������A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //ѡ�񼷳���A
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[0] = 0; //���ѡ��
				}
				else{ //������B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //ѡ�񼷳���B
					if(TempIns.P_Exis) //ʹ��P����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //д���¶�
					else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //д���¶�
					else //δָ���¶�
						Buffer[LoadIndex].ExtruderExis[1] = 0; //���ѡ��	
				}
			}
			
			u8 PreIndex = LoadIndex;
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��		
			RestBuffer--; //ʣ�໺������-1
			
			Buffer[LoadIndex].Type = Buffer[PreIndex].Type; //���Ʊ��
			Buffer[LoadIndex].LineCounter = Buffer[PreIndex].LineCounter; //�����к�
			Buffer[LoadIndex].BaseAddress = Buffer[PreIndex].BaseAddress; //���ƻ�ַ
			Buffer[LoadIndex].Segment = 1; //�ֶα��
			for(u8 i=0;i<2;i++){ //�����¶�
				Buffer[LoadIndex].ExtruderExis[i] = Buffer[PreIndex].ExtruderExis[i]; 
				Buffer[LoadIndex].TemperatureE[i] = Buffer[PreIndex].TemperatureE[i];
			}
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��		
			RestBuffer--; //ʣ�໺������-1
			
		}
		else if(TempIns.GM_Para == 140){ //M140���û�׼���¶�/������
			Buffer[LoadIndex].Type = 140; //ָ����
			Buffer[LoadIndex].BasePlaneExis = 0;
			Buffer[LoadIndex].TemperatureB = 0;
			
			Buffer[LoadIndex].BasePlaneExis = 1; //ѡ���׼��
			if(TempIns.P_Exis) //ʹ��P����ָ���¶�
				Buffer[LoadIndex].TemperatureB = TempIns.P_Para; //д���¶�
			else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
				Buffer[LoadIndex].TemperatureB = TempIns.S_Para; //д���¶�
			else //δָ���¶�
				Buffer[LoadIndex].BasePlaneExis = 0; //���ѡ��

			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
		}
		else if(TempIns.GM_Para == 190){ //M190���û�׼���¶�/����
			Buffer[LoadIndex].Type = 190; //ָ����
			Buffer[LoadIndex].BasePlaneExis = 0;
			Buffer[LoadIndex].TemperatureB = 0;
			
			Buffer[LoadIndex].BasePlaneExis = 1; //ѡ���׼��
			if(TempIns.P_Exis) //ʹ��P����ָ���¶�
				Buffer[LoadIndex].TemperatureB = TempIns.P_Para; //д���¶�
			else if(TempIns.S_Exis) //ʹ��S����ָ���¶�
				Buffer[LoadIndex].TemperatureB = TempIns.S_Para; //д���¶�
			else //δָ���¶�
				Buffer[LoadIndex].BasePlaneExis = 0; //���ѡ��

			u8 PreIndex = LoadIndex;
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
			
			Buffer[LoadIndex].Type = Buffer[PreIndex].Type; //���Ʊ��
			Buffer[LoadIndex].LineCounter = Buffer[PreIndex].LineCounter; //�����к�
			Buffer[LoadIndex].BaseAddress = Buffer[PreIndex].BaseAddress; //���ƻ�ַ
			Buffer[LoadIndex].Segment = 1; //�ֶα��
			Buffer[LoadIndex].BasePlaneExis = Buffer[PreIndex].BasePlaneExis; 
			Buffer[LoadIndex].TemperatureB = Buffer[PreIndex].TemperatureB;
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��		
			RestBuffer--; //ʣ�໺������-1
		}
	}
	else if(TempIns.T_Exis){ //Tnnnѡ�񼷳���
		if(!TempIns.T_Para){ //ѡ�񼷳���A
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 0;
			Buffer[LoadIndex].AxisExis[3] = 1;
			Buffer[LoadIndex].AxisExis[4] = 0;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
			
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 1;
			Buffer[LoadIndex].AxisExis[3] = 1;
			Buffer[LoadIndex].AxisExis[4] = 0;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
			
			StreamStatus.ExtruderPres = 0;
		}
		else{ //ѡ�񼷳���B
			StreamStatus.ExtruderPres = 1;
			
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 0;
			Buffer[LoadIndex].AxisExis[3] = 0;
			Buffer[LoadIndex].AxisExis[4] = 1;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
			
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 1;
			Buffer[LoadIndex].AxisExis[3] = 0;
			Buffer[LoadIndex].AxisExis[4] = 1;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //��¼ָ���ַ
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //��¼ָ���к�
			StreamStatus.InsCounter++; //�к�+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer--; //ʣ�໺������-1
		}
	}
	else if(TempIns.F_Exis){ //Fnnn�����ٶ�
		StreamStatus.VelocityPres = TempIns.F_Para/60;
		if(StreamStatus.VelocityPres < 10)
			StreamStatus.VelocityPres = 10;
	}
}

char INTERPRETER::StatusCheck(){
	if(!Status.FileExist) //�ļ�������
		return 'F';
	if(Buffer[LaunchIndex].Type == 0) //�����
		return 'E'; //�����
	if(Status.StateHomed!='E' || Status.StateFA == 'N')
		return 'H'; //δ��λ	
	if(Buffer[LaunchIndex].Type == 1){ //�ƶ�ָ��
		if(Buffer[LaunchIndex].AxisExis[3]){ //��A��λ��
			AuxIO.GetStatusMaterialA();
			if(Status.MaterialA == 0) return 'A'; //Aȱ��
		}
		if(Buffer[LaunchIndex].AxisExis[4]){ //��B��λ��
			AuxIO.GetStatusMaterialB();
			if(Status.MaterialB == 0) return 'B'; //Bȱ��
		}
	}
	return 'D';
}

char INTERPRETER::Process(){
	if(Buffer[LaunchIndex].Type == 0){ //�����
		if(RestBuffer != BUFFER_SIZE){
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			RestBuffer++; //ʣ�໺������+1
		}
		return 'E';
	}
	else if(Buffer[LaunchIndex].Type == 1){ //�ƶ�ָ��
		char Recv = Sconi.Motion(Buffer[LaunchIndex].Position,Buffer[LaunchIndex].Velocity);
		if(Recv == 'D'){ //ִ�����
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
			RestBuffer++; //ʣ�໺������+1
			return 'D';
		}
		else if(Recv == 'F') //������,��Ϊִ�����
			return 'D';
		else //����
			return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 28){ //��λָ��
		if(!Buffer[LaunchIndex].Segment){ //��Ҫʵ���ƶ�
			char Recv = 'D';
			Recv = Sconi.GetStatusFA();
			if(Recv != 'D') return Recv;
			if(Status.StateFA != 'I') return 'D';
			
			Recv = Sconi.GetPositionX();
			if(Recv != 'D') return Recv;
			Recv = Sconi.GetPositionY();
			if(Recv != 'D') return Recv;
			Recv = Sconi.GetPositionZ();
			if(Recv != 'D') return Recv;
			Recv = Sconi.GetPositionA();
			if(Recv != 'D') return Recv;
			Recv = Sconi.GetPositionB();
			if(Recv != 'D') return Recv;
			
			double Posi[5] = {Status.PosiX,Status.PosiY,Status.PosiZ,Status.PosiA,Status.PosiB};
			
			for(u8 i=0;i<3;i++){
				if(Buffer[LaunchIndex].AxisExis[i])
					Buffer[LaunchIndex].Position[i] = 0;
				else
					Buffer[LaunchIndex].Position[i] = Posi[i];
			}
			Buffer[LaunchIndex].Position[3] = Posi[3];
			Buffer[LaunchIndex].Position[4] = Posi[4];
			
			Recv = Sconi.Motion(Buffer[LaunchIndex].Position,Buffer[LaunchIndex].Velocity);
			if(Recv == 'D'){ //ִ�����
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE; //�ƶ�ָ��
				RestBuffer++; //ʣ�໺������+1
				return 'D';
			}
			else if(Recv == 'F') //������,��Ϊִ�����
				return 'D';
			else //����
				return Recv;
		}
		else{ //��Ҫ��������
			char Recv = 'D';
			if(Buffer[LaunchIndex].AxisExis[3]){
				Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[3],'A');
				if(Recv == 'K') return 'D'; //��ֹ����,��Ϊִ������
				if(Recv != 'D') return Recv;
			}
			if(Buffer[LaunchIndex].AxisExis[4]){
				Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[4],'B');
				if(Recv == 'K') return 'D'; //��ֹ����,��Ϊִ������
				if(Recv != 'D') return Recv;
			}
			
			RestBuffer++; //ʣ�໺������+1
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			return Recv;
		}
	}
	else if(Buffer[LaunchIndex].Type == 92){ //��������
		char Recv = 'D';
		if(Buffer[LaunchIndex].AxisExis[0]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[0],'X');
			if(Recv == 'K') return 'D'; //��ֹ����,��Ϊִ������
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[1]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[1],'Y');
			if(Recv == 'K') return 'D'; //��ֹ����,��Ϊִ������
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[2]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[2],'Z');
			if(Recv == 'K') return 'D'; //��ֹ����,��Ϊִ������
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[3]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[3],'A');
			if(Recv == 'K') return 'D'; //��ֹ����,��Ϊִ������
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[4]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[4],'B');
			if(Recv == 'K') return 'D'; //��ֹ����,��Ϊִ������
			if(Recv != 'D') return Recv;
		}
		
		RestBuffer++; //ʣ�໺������+1
		LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 104){ //���ü������¶�/������
		char Recv = 'D';
		
		if(Buffer[LaunchIndex].ExtruderExis[0])
			Recv = Tconi.SetTemperatureEASetting(Buffer[LaunchIndex].TemperatureE[0]);
		else if(Buffer[LaunchIndex].ExtruderExis[1])
			Recv = Tconi.SetTemperatureEBSetting(Buffer[LaunchIndex].TemperatureE[1]);
		
		if(Recv=='D' || Recv=='H' || Recv=='L'){
			RestBuffer++; //ʣ�໺������+1
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			if(Recv == 'H') return '2';
			if(Recv == 'L') return '1';
		}
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 106){ //M106���÷���״̬
		char Recv = 'D';
		
		if(Buffer[LaunchIndex].CoolerExis[0]){
			Recv = Tconi.SetCoolerPowerExtruderA(Buffer[LaunchIndex].CoolerPower[0]);
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].CoolerExis[1]){
			Recv = Tconi.SetCoolerPowerExtruderB(Buffer[LaunchIndex].CoolerPower[1]);
			if(Recv != 'D') return Recv;
		}
		
		RestBuffer++; //ʣ�໺������+1
		LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 107){ //M107�رշ���
		char Recv = 'D';
		
		if(Buffer[LaunchIndex].CoolerExis[0]){
			Recv = Tconi.SetCoolerPowerExtruderA(0);
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].CoolerExis[1]){
			Recv = Tconi.SetCoolerPowerExtruderB(0);
			if(Recv != 'D') return Recv;
		}
		
		RestBuffer++; //ʣ�໺������+1
		LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 109){ //M109���ü������¶�/����
		if(!Buffer[LaunchIndex].Segment){ //�����¶�
			char Recv = 'D';
			if(Buffer[LaunchIndex].ExtruderExis[0])
				Recv = Tconi.SetTemperatureEASetting(Buffer[LaunchIndex].TemperatureE[0]);
			else if(Buffer[LaunchIndex].ExtruderExis[1])
				Recv = Tconi.SetTemperatureEBSetting(Buffer[LaunchIndex].TemperatureE[1]);

			if(Recv=='D' || Recv=='H' || Recv=='L'){
				RestBuffer++; //ʣ�໺������+1
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
				if(Recv == 'H') return '2';
				if(Recv == 'L') return '1';
			}
			return Recv;
		}
		else{ //�ȴ��¶ȵ���
			if(Buffer[LaunchIndex].ExtruderExis[0]){
				Tconi.GetStatusExtruderA();
				Tconi.GetTemperatureExtruderA();
				Tconi.GetTemperatureEASetting();
				if(!Status.TempExtruderA)
					return 'X';
				else if(Status.TempExtruderASet<150 && Status.TempExtruderA<100){ //��ȴ���
					RestBuffer++; //ʣ�໺������+1
					LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;		
				}					
				else if((Status.TempExtruderASet-2) < Status.TempExtruderA){
					if((Status.TempExtruderASet+2) > Status.TempExtruderA){
						RestBuffer++; //ʣ�໺������+1 
						LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
					}
				}
			}
			else if(Buffer[LaunchIndex].ExtruderExis[1]){
				Tconi.GetStatusExtruderB();
				Tconi.GetTemperatureExtruderB();
				Tconi.GetTemperatureEBSetting();
				if(!Status.TempExtruderB)
					return 'X';
				else if(Status.TempExtruderBSet<150 && Status.TempExtruderB<100){ //��ȴ���
					RestBuffer++; //ʣ�໺������+1
					LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;		
				}					
				else if((Status.TempExtruderBSet-2) < Status.TempExtruderB){
					if((Status.TempExtruderBSet+2) > Status.TempExtruderB){
						RestBuffer++; //ʣ�໺������+1 
						LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
					}
				}
			}
			return 'D';
		}
	}
	else if(Buffer[LaunchIndex].Type == 140){ //M140���û�׼���¶�/������
		char Recv = 'D';

		if(Buffer[LaunchIndex].BasePlaneExis)
			Recv = Tconi.SetTemperatureBPSetting(Buffer[LaunchIndex].TemperatureB);

		if(Recv=='D' || Recv=='H' || Recv=='L'){
			RestBuffer++; //ʣ�໺������+1
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			if(Recv == 'H') return '2';
			if(Recv == 'L') return '1';
		}
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 190){ //M190���û�׼���¶�/����
		if(!Buffer[LaunchIndex].Segment){ //�����¶�
			char Recv = 'D';
			if(Buffer[LaunchIndex].BasePlaneExis)
				Recv = Tconi.SetTemperatureBPSetting(Buffer[LaunchIndex].TemperatureB);
			
			if(Recv=='D' || Recv=='H' || Recv=='L'){
				RestBuffer++; //ʣ�໺������+1
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
				if(Recv == 'H') return '2';
				if(Recv == 'L') return '1';
			}
			return Recv;
		}
		else{ //�ȴ��¶ȵ���
			Tconi.GetStatusBasePlane();
			Tconi.GetTemperatureBasePlane();
			Tconi.GetTemperatureBPSetting();
			if(!Status.TempBasePlane)
				return 'X';
			else if(Status.TempBasePlaneSet<35 && Status.TempBasePlane<40){ //��ȴ���
				RestBuffer++; //ʣ�໺������+1
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;		
			}
			else if(Status.TempBasePlaneSet>=110 && Status.TempBasePlane>=110){
				RestBuffer++; //ʣ�໺������+1 
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;	
			}
			else if((Status.TempBasePlaneSet-2) < Status.TempBasePlane){
				if((Status.TempBasePlaneSet+2) > Status.TempBasePlane){
					RestBuffer++; //ʣ�໺������+1 
					LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
				}
			}
			return 'D';
		}
	}
	else if(Buffer[LaunchIndex].Type == 205){ //�л���ͷ
		if(!Buffer[LaunchIndex].Segment){ //�����л���ͷָ��
			u8 NozzleA = 0;
			u8 NozzleB = 0;
			if(Buffer[LaunchIndex].AxisExis[3]) NozzleA = 1;
			if(Buffer[LaunchIndex].AxisExis[4]) NozzleB = 1;
			char Recv = Sconi.SetNozzleState(NozzleA,NozzleB);
			if(Recv == 'M'){
				RestBuffer++; //ʣ�໺������+1 
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;	
				return 'D';
			}
			else return Recv;
		}
		else{ //�ȴ��л����
			if(!Sconi.GetStatusBusy()){
				RestBuffer++; //ʣ�໺������+1 
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			}
			return 'D';
		}
	}
	else{
		RestBuffer++; //ʣ�໺������+1 
		LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;	
		return 'D';
	}
}


double INTERPRETER::CharToDouble(u8 *Data){
	double Number = 0;
	double Sign = 1;
	u8 *Ptr = Data;
	
	static double WeightI[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};
	static double WeightD[] = {1,.1,.01,.001,.0001,.00001,.000001,.0000001,.00000001,.000000001,.0000000001};
	
	if(Ptr[0] == '-'){
		Sign = -1;
		Ptr++;
	}
	else if(Ptr[0] == '+'){
		Ptr++;
	}
	
	u8 *TempPtr = Ptr;
	u8 IntNumber = 0;
	for(IntNumber=0;IntNumber<11;IntNumber++){ //ͳ������λ��
		if(TempPtr[0]<'0' || TempPtr[0]>'9') break;
		else TempPtr++;
	}
	
	for(u8 i=0;i<IntNumber;i++) //��������
		Number += (*(Ptr++)-'0')*WeightI[IntNumber-i];
	
	if(Ptr[0] == '.'){ //��С��
		Ptr++; //����С����
		for(u8 i=0;i<11;i++){ //С������
			if(Ptr[0]<'0' || Ptr[0]>'9') break;
			else Number += (*(Ptr++)-'0')*WeightD[i+1];
		}
	}
	Number *= Sign; //��ӷ���
	
	return Number;
}

double INTERPRETER::AbsDouble(double Data){
	if(Data < 0) return -Data;
	return Data;
}

s32 INTERPRETER::CharToDecimal(u8 *Data){
	long long Number = 0;
	long long Sign = 1;
	u8 *Ptr = Data;
	
	static long long Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};

	if(Ptr[0] == '-'){
		Sign = -1;
		Ptr++;
	}
	else if(Ptr[0] == '+'){
		Ptr++;
	}
	
	u8 *TempPtr = Ptr;
	u8 IntNumber = 0;
	for(IntNumber=0;IntNumber<11;IntNumber++){ //ͳ������λ��
		if(TempPtr[0]<'0' || TempPtr[0]>'9') break;
		else TempPtr++;
	}
	
	for(u8 i=0;i<IntNumber;i++) //��������
		Number += (*(Ptr++)-'0')*Weight[IntNumber-i];
	
	Number *= Sign; //��ӷ���
	
	return Number;
}

u32 INTERPRETER::AbsDecimal(s32 Data){
	if(Data < 0) return -Data;
	return Data;
}
void INTERPRETER::AxisRound(double &Data,double Diff){
	int64_t Part = Data/Diff;
	Data = (double)Part*Diff;
}
