#include "CCONI.h"
#include "IBUS.h"
#include "Status.h"
#include "Shell.h"
#define s64 long long

CCONI Cconi;

char CCONI::GetCommand(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',"GETCOM",100); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{
			Shell.Flush(); //清空命令缓存
			if(Ibus.DataNumber>1){ //有命令
				for(u32 i=1;i<Ibus.DataNumber;i++)
					Shell.ComBuffer[i-1] = Ibus.Buffer[i];
				Shell.Received = 1; //设置接收完成标志
			}
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char CCONI::SetStatusBar(){
	char TempStr[44];
	char *TempPtr = TempStr;
	*(TempPtr++) = 'S';
	*(TempPtr++) = 'E';
	*(TempPtr++) = 'T';
	*(TempPtr++) = 'S';
	*(TempPtr++) = 'T';
	*(TempPtr++) = 'A';
	
	for(u32 i=0;i<38;i++)
		*(TempPtr++) = Status.StatusBar[i];
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',TempStr,100); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')) return 'D'; //Done
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char CCONI::SetInfoScreen(){
	char TempStr[90];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'S';
	*(TempPtr++) = 'E';
	*(TempPtr++) = 'T';
	*(TempPtr++) = 'I';
	*(TempPtr++) = 'N';
	*(TempPtr++) = 'F';
	
	TempPtr = Num2Chr(Status.TempExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.TempExtruderB,TempPtr);
	TempPtr = Num2Chr(Status.TempBasePlane,TempPtr);
	
	TempPtr = Num2Chr(Status.TempExtruderASet,TempPtr);
	TempPtr = Num2Chr(Status.TempExtruderBSet,TempPtr);
	TempPtr = Num2Chr(Status.TempBasePlaneSet,TempPtr);
	
	TempPtr = Num2Chr(Status.PowerHeaterExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.PowerHeaterExtruderB,TempPtr);
	TempPtr = Num2Chr(Status.PowerHeaterBasePlane,TempPtr);
	
	TempPtr = Num2Chr(Status.PowerCoolerExtruderA,TempPtr);
	TempPtr = Num2Chr(Status.PowerCoolerExtruderB,TempPtr);
	
	TempPtr = Num2Chr(Status.TempAir,TempPtr);
	
	*(TempPtr++) = Status.StateExtruderA;
	*(TempPtr++) = Status.StateExtruderB;
	*(TempPtr++) = Status.StateBasePlane;
	
	TempPtr = Num2Chr(Status.PosiX,TempPtr);
	TempPtr = Num2Chr(Status.PosiY,TempPtr);
	TempPtr = Num2Chr(Status.PosiZ,TempPtr);
	
	TempPtr = Num2Chr(Status.PosiXMax,TempPtr);
	TempPtr = Num2Chr(Status.PosiYMax,TempPtr);
	TempPtr = Num2Chr(Status.PosiZMax,TempPtr);
	
	TempPtr = Num2Chr(Status.L1Max-Status.L1Reset,TempPtr);
	TempPtr = Num2Chr(Status.L2Max-Status.L2Reset,TempPtr);
	
	TempPtr = Num2Chr(Status.L1Max,TempPtr);
	TempPtr = Num2Chr(Status.L2Max,TempPtr);
	
	*(TempPtr++) = Status.StateFA;
	*(TempPtr++) = Status.StateIDP;
	
	*(TempPtr) = 0;
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',TempStr,100); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')) return 'D'; //Done
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';

}

char CCONI::ConsoleDisplay(char Data[],bool Hold){
	char TempStr[256];
	char *TempPtr = TempStr;
	
	if(Hold){
		*(TempPtr++) = 'H';
		*(TempPtr++) = 'L';
		*(TempPtr++) = 'D';
		*(TempPtr++) = 'D';
		*(TempPtr++) = 'A';
		*(TempPtr++) = 'T';
	}
	else{
		*(TempPtr++) = 'S';
		*(TempPtr++) = 'E';
		*(TempPtr++) = 'T';
		*(TempPtr++) = 'D';
		*(TempPtr++) = 'A';
		*(TempPtr++) = 'T';
	}
	
	for(u32 i=0;i<256&&Data[i];i++)
		*(TempPtr++) = Data[i];
	
	*(TempPtr) = 0;
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',TempStr,500); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')) return 'D'; //Done
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char CCONI::HangUpConsole(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',"WAITIN",100); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')) return 'D'; //Done
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char CCONI::ExpandDictionary(char Data[]){
	char TempStr[256];
	char *TempPtr = TempStr;
	
	*(TempPtr++) = 'A';
	*(TempPtr++) = 'D';
	*(TempPtr++) = 'D';
	*(TempPtr++) = 'D';
	*(TempPtr++) = 'I';
	*(TempPtr++) = 'C';
	
	if(!Data[0]) return 'D';
	
	for(u32 i=0;i<23&&Data[i];i++)
		*(TempPtr++) = Data[i];
	
	*(TempPtr) = 0;
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',TempStr,100); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')) return 'D'; //Done
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char CCONI::GetSwitchAStatus(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',"GETSWA",100); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else if(Ibus.TestReply('O')){
			Status.StateSwitchA = 'E';
			return 'D';
		}
		else if(Ibus.TestReply('F')){
			Status.StateSwitchA = 'D';
			return 'D';
		}
		else
			return 'U'; //Unknown Status			
	}
	else //未收到回复
		return 'T';
}

char CCONI::GetSwitchBStatus(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',"GETSWB",100); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else if(Ibus.TestReply('O')){
			Status.StateSwitchB = 'E';
			return 'D';
		}
		else if(Ibus.TestReply('F')){
			Status.StateSwitchB = 'D';
			return 'D';
		}
		else
			return 'U'; //Unknown Status			
	}
	else //未收到回复
		return 'T';
}

bool CCONI::TestLink(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('C',"TSTLIK",10); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')) return 1; //Done
		else if(Ibus.TestReply('R')) return 0; //Verifying Error
		else if(Ibus.TestReply('N')) return 0; //Instruction Error
		else return 0; //Unknown Status
	}
	else //未收到回复
		return 0;	
}

char* CCONI::Num2Chr(u32 Data,char* Str){
	static const u32 Weight[] = {1,1,10,100};
	for(u8 i=0;i<3;i++)
		*(Str++) = Data/Weight[3-i] % 10 + '0'; //转码
	return Str;
}

char* CCONI::Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign){
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

double CCONI::Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign){
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

