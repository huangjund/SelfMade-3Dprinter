#include "TCONI.h"
#include "CBUS.h"
#include "Status.h"
#define s64 long long

TCONI Tconi;

char TCONI::GetTemperatureExtruderA(){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"GA",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderA = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'U';
}

char TCONI::GetTemperatureExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"GB",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderB = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'U';
}

char TCONI::GetTemperatureBasePlane(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"GG",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempBasePlane = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'U';
}

char TCONI::GetTemperatureEnvironment(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"GE",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempAir = Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'U';
}

char TCONI::SetTemperatureEASetting(u16 T){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'S'; //指令标识
	
	*(TempPtr++) = 'A'; //指令子标识

	if(T > 320) T = 320;
	
	TempPtr = Flo2Chr(TempPtr,T,3,0,0);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //发送并记录回复状态
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

char TCONI::SetTemperatureEBSetting(u16 T){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'S'; //指令标识
	
	*(TempPtr++) = 'B'; //指令子标识

	if(T > 320) T = 320;
	
	TempPtr = Flo2Chr(TempPtr,T,3,0,0);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //发送并记录回复状态
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

char TCONI::SetTemperatureBPSetting(u16 T){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'S'; //指令标识
	
	*(TempPtr++) = 'G'; //指令子标识

	if(T > 150) T = 150;
	
	TempPtr = Flo2Chr(TempPtr,T,3,0,0);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //发送并记录回复状态
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

char TCONI::GetTemperatureEASetting(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"TA",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderASet =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'U';
}

char TCONI::GetTemperatureEBSetting(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"TB",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempExtruderBSet =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'U';
}

char TCONI::GetTemperatureBPSetting(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"TG",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.TempBasePlaneSet =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'U';
}

char TCONI::SetCoolerPowerExtruderA(u8 P){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'C'; //指令标识
	
	*(TempPtr++) = 'A'; //指令子标识

	if(P > 100) P = 100;
	
	TempPtr = Flo2Chr(TempPtr,P,3,0,0);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //发送并记录回复状态
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

char TCONI::SetCoolerPowerExtruderB(u8 P){
	char TempStr[6];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'C'; //指令标识
	
	*(TempPtr++) = 'B'; //指令子标识

	if(P > 100) P = 100;
	
	TempPtr = Flo2Chr(TempPtr,P,3,0,0);
	
	*TempPtr = 0; //结束符
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',TempStr,100); //发送并记录回复状态
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

char TCONI::GetCoolerPowerExtruderA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"RA",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerCoolerExtruderA =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char TCONI::GetCoolerPowerExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"RB",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerCoolerExtruderB =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char TCONI::GetHeaterPowerExtruderA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"HA",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerHeaterExtruderA =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char TCONI::GetHeaterPowerExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"HB",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerHeaterExtruderB =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char TCONI::GetHeaterPowerBasePlane(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"HG",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.PowerHeaterBasePlane =	Chr2Flo((char*)(Cbus.Buffer+1),3,0,0);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char TCONI::GetStatusExtruderA(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"EA",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateExtruderA = Cbus.Buffer[1];
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char TCONI::GetStatusExtruderB(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"EB",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateExtruderB = Cbus.Buffer[1];
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char TCONI::GetStatusBasePlane(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"EG",100); //发送并记录回复状态
		if(ReplyReceived && !Cbus.TestReply('R')){ //收到回复且校验成功
			if(!Cbus.TestReply('N') && !Cbus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Cbus.TestReply('R')) return 'R'; //Verifying Error
		else if(Cbus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Status.StateBasePlane = Cbus.Buffer[1];
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

bool TCONI::TestLink(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Cbus.Transmit('T',"L",10); //发送并记录回复状态
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

char* TCONI::Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign){
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

double TCONI::Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign){
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


