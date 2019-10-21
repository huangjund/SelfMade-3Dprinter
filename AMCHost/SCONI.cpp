#include "SCONI.h"
#include "CBUS.h"
#include "Status.h"
#define s64 long long

SCONI Sconi;

char SCONI::Motion(double Position[5],float Vmax){
	char TempStr[75];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'M'; //指令标识
	
	*(TempPtr++) = 'X'; //X轴
	TempPtr = Flo2Chr(TempPtr,Position[0],4,6,1);
	
	*(TempPtr++) = 'Y'; //Y轴
	TempPtr = Flo2Chr(TempPtr,Position[1],4,6,1);
	
	*(TempPtr++) = 'Z'; //Z轴
	TempPtr = Flo2Chr(TempPtr,Position[2],4,6,1);
	
	*(TempPtr++) = 'A'; //A轴
	TempPtr = Flo2Chr(TempPtr,Position[3],6,6,1);
	
	*(TempPtr++) = 'B'; //B轴
	TempPtr = Flo2Chr(TempPtr,Position[4],6,6,1);
	
	*(TempPtr++) = 'V'; //最高速度
	TempPtr = Flo2Chr(TempPtr,Vmax,3,0,0);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',TempStr,100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('F')) return 'F'; //Cache Full
		else if(Cbus.TestReply('K')) return 'K'; //Not Home
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}


char SCONI::InitPosition(double Position[5]){
	char TempStr[71];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'I'; //指令标识
	
	*(TempPtr++) = 'X'; //X轴
	TempPtr = Flo2Chr(TempPtr,Position[0],4,6,1);
	
	*(TempPtr++) = 'Y'; //Y轴
	TempPtr = Flo2Chr(TempPtr,Position[1],4,6,1);
	
	*(TempPtr++) = 'Z'; //Z轴
	TempPtr = Flo2Chr(TempPtr,Position[2],4,6,1);
	
	*(TempPtr++) = 'A'; //A轴
	TempPtr = Flo2Chr(TempPtr,Position[3],6,6,1);
	
	*(TempPtr++) = 'B'; //B轴
	TempPtr = Flo2Chr(TempPtr,Position[4],6,6,1);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',TempStr,100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('K')) return 'K'; //Forbiden
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char SCONI::AutoHoming(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"HHOMEXYZ",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else if(Cbus.TestReply('M')) return 'M'; //Now Moving
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char SCONI::WaitForHoming(u32 Time){
	while(Time--){
		SysTick->LOAD=72000;
		SysTick->CTRL=0x00000005;
		while(!(SysTick->CTRL&0x00010000));
		SysTick->CTRL=0x00000004;
	}	
	if(Cbus.RxFinish){
		if(Cbus.TestReply('D')) return 'D';
		else return 'N';
	}
	return 'T';
}

char SCONI::MoveAxis(double Position,u8 Selection){
	if(Selection > 4) return 'N';
	
	char TempStr[71];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'E'; //指令标识
	
	if(Selection == 0) *(TempPtr++) = 'X'; //X轴
	else if(Selection == 1) *(TempPtr++) = 'Y'; //Y轴
	else if(Selection == 2) *(TempPtr++) = 'Z'; //Z轴
	else if(Selection == 3) *(TempPtr++) = 'A'; //A轴
	else if(Selection == 4) *(TempPtr++) = 'B'; //B轴
	else return 'N';
	TempPtr = Flo2Chr(TempPtr,Position,6,6,1);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',TempStr,100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('M')) return 'M'; //Now Moving
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char SCONI::WaitForMoving(u32 Time){
	while(Time--){
		SysTick->LOAD=72000;
		SysTick->CTRL=0x00000005;
		while(!(SysTick->CTRL&0x00010000));
		SysTick->CTRL=0x00000004;
	}	
	if(Cbus.RxFinish){
		if(Cbus.TestReply('D')) return 'D';
		else return 'N';
	}
	return 'T';
}

char SCONI::GetTestPointA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G4",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TestPosition[0] = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetTestPointB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G5",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TestPosition[1] = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetTestPointC(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G6",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TestPosition[2] = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetTestPointD(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G7",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TestPosition[3] = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetTestPointM(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G8",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TestPosition[4] = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionX(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GX",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiX = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionY(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GY",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiY = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionZ(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GZ",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiZ = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GA",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiA = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GB",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiB = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionXMax(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GO",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiXMax = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionYMax(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GP",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiYMax = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetPositionZMax(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GQ",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PosiZMax = Chr2Flo((char*)(Cbus.Buffer+1),6,6,1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetStatusFA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GS",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateFA = Cbus.Buffer[1];
			if(Status.StateFA == 'H') //避免与标志冲突
				Status.StateFA = 'N';
			return	'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetStatusLeveling(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G3",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateLeveling = Cbus.Buffer[1];
			return	'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetStatusIDP(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GI",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateIDP = Cbus.Buffer[1];
			return	'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetStatusStepper(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GE",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateStepper = Cbus.Buffer[1];
			return	'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetStatusHomed(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GH",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateHomed = Cbus.Buffer[1];
			return	'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetL1Rest(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G1",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.L1Reset = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetL2Rest(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"G2",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.L2Reset = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetL1Max(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GM",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.L1Max = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char SCONI::GetL2Max(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"GN",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.L2Max = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}


char SCONI::SetPosition(double Position,char Select){
	char TempStr[17];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'P'; //指令标识
	
	if(Select == 'X') *(TempPtr++) = 'X'; //X轴
	else if(Select == 'Y') *(TempPtr++) = 'Y'; //Y轴
	else if(Select == 'Z') *(TempPtr++) = 'Z'; //Z轴
	else if(Select == 'A') *(TempPtr++) = 'A'; //A轴
	else if(Select == 'B') *(TempPtr++) = 'B'; //B轴
	else return 'N';
	
	TempPtr = Flo2Chr(TempPtr,Position,6,6,1);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',TempStr,100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char SCONI::EnableStepper(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"SE",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('K')) return 'K'; //Running
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char SCONI::DisableStepper(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"SD",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('K')) return 'K'; //Running
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char SCONI::EnableIDP(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"DE",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char SCONI::DisableIDP(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"DD",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 'D'; //Done
		else if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

bool SCONI::TestLink(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('S',"T",10); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('D')) return 1; //Done
		else if(Cbus.TestReply('R')) return 0; //Verifying Error
		else if(Cbus.TestReply('N')) return 0; //Instruction Error
		else return 0; //Unknown Status
	}
	else //未收到回复
		return 0;	
}

char* SCONI::Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign){
	if(Int > 10) Int = 10; //范围不能太大
	if(Dec > 10) Dec = 10; //精度不能太高
	
	static s64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};
	
	if(Sign){ //需要写入符号
		if(Num >= 0) *(Buf++) = '+';
		else *(Buf++) = '-';
	}
	
	if(Num<0) Num = -Num; //取绝对值
	
	for(u32 i=0;i<Int;i++) //整数部分
		*(Buf++) = (s64)(Num)%Weight[Int-i+1]/Weight[Int-i] + '0';
	
	if(Dec) *(Buf++) = '.'; //小数点(如果有小数)

	for(u32 i=0;i<Dec;i++) //小数部分
		*(Buf++) = (s64)(Num*Weight[i+1])%10 + '0';
	
	return Buf;
}

double SCONI::Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign){
	if(Int > 10) Int = 10; //范围不能太大
	if(Dec > 10) Dec = 10; //精度不能太高
	
	double Temp = 0;
	double Symbol = 1;
	
	static double Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};
	
	if(Sign){ //有符号
		if(*(Buf++) == '-') Symbol = -1;
	}
	
	for(u32 i=0;i<Int;i++) //整数部分
		Temp += (*(Buf++)-'0')*Weight[Int-i];
	
	if(Dec) Buf++; //小数点(如果有小数)

	for(u32 i=0;i<Dec;i++) //小数部分
		Temp += (*(Buf++)-'0')/Weight[Int-i+1];
	
	Temp *= Symbol;
	
	return Temp;
}


