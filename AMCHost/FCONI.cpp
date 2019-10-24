#include "FCONI.h"
#include "IBUS.h"
#include "Status.h"

#define u64 unsigned long long

FCONI Fconi;

char FCONI::GetTotalVolume(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'C';
	TempStr[2] = 'A';
	TempStr[3] = 'V';
	TempStr[4] = 'O';
	TempStr[5] = 'L';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.CardTotalVolume = CharToNumber(3,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetRestVolume(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'C';
	TempStr[2] = 'R';
	TempStr[3] = 'V';
	TempStr[4] = 'O';
	TempStr[5] = 'L';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.CardRestVolume = CharToNumber(10,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char FCONI::GetFileName(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'N';
	TempStr[3] = 'A';
	TempStr[4] = 'M';
	TempStr[5] = 'E';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			for(u8 i=0;i<32;i++)
				Status.FileName[i] = 0;
			for(u8 i=1;i<Ibus.DataNumber;i++)
				Status.FileName[i-1] = Ibus.Buffer[i];
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileSize(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'S';
	TempStr[3] = 'I';
	TempStr[4] = 'Z';
	TempStr[5] = 'E';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FileSize = CharToNumber(10,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileCommandNumber(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'C';
	TempStr[3] = 'O';
	TempStr[4] = 'M';
	TempStr[5] = 'S';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.CommandNumber = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileEstimateTime(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'E';
	TempStr[3] = 'S';
	TempStr[4] = 'T';
	TempStr[5] = 'M';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.EstimateTime = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileTotalDistance(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'A';
	TempStr[3] = 'D';
	TempStr[4] = 'I';
	TempStr[5] = 'S';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.TotalDistance = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileInfo1(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'I';
	TempStr[3] = 'N';
	TempStr[4] = 'F';
	TempStr[5] = '1';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FileInfo1 = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileInfo2(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'I';
	TempStr[3] = 'N';
	TempStr[4] = 'F';
	TempStr[5] = '2';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FileInfo2 = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileInfo3(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'I';
	TempStr[3] = 'N';
	TempStr[4] = 'F';
	TempStr[5] = '3';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FileInfo3 = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileInfo4(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'I';
	TempStr[3] = 'N';
	TempStr[4] = 'F';
	TempStr[5] = '4';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FileInfo4 = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileInfo5(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'I';
	TempStr[3] = 'N';
	TempStr[4] = 'F';
	TempStr[5] = '5';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FileInfo5 = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFileInfo6(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'I';
	TempStr[3] = 'N';
	TempStr[4] = 'F';
	TempStr[5] = '6';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FileInfo6 = CharToNumber(8,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::GetFilePointer(){
	char TempStr[20] = {0};
	TempStr[0] = 'R';
	TempStr[1] = 'F';
	TempStr[2] = 'P';
	TempStr[3] = 'I';
	TempStr[4] = 'T';
	TempStr[5] = 'R';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{ //Done
			Status.FilePointer = CharToNumber(10,Ibus.Buffer+1);
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char FCONI::FileOpen(){
	char TempStr[20] = {0};
	TempStr[0] = 'F';
	TempStr[1] = 'I';
	TempStr[2] = 'O';
	TempStr[3] = 'P';
	TempStr[4] = 'E';
	TempStr[5] = 'N';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')){
			Status.FileExist = 1;			
			return 'D'; //Done
		}
		else if(Ibus.TestReply('K')){
			Status.FileExist = 0;			
			return 'K'; //Failed
		}
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char FCONI::FileTest(){
	char TempStr[20] = {0};
	TempStr[0] = 'F';
	TempStr[1] = 'I';
	TempStr[2] = 'T';
	TempStr[3] = 'E';
	TempStr[4] = 'S';
	TempStr[5] = 'T';
		
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')){
			Status.FileExist = 1;			
			return 'D'; //Done
		}
		else if(Ibus.TestReply('K')){
			Status.FileExist = 0;			
			return 'K'; //Failed
		}
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

char FCONI::FileRead(u8 Length,u8 *Buffer){
	if(Length > 99) Length = 99;
	
	char TempStr[20] = {0};
	TempStr[0] = 'F';
	TempStr[1] = 'I';
	TempStr[2] = 'R';
	TempStr[3] = 'E';
	TempStr[4] = 'A';
	TempStr[5] = 'D';

	NumberToChar(2,Length,(u8*)(TempStr+6));
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,200); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{
			for(u8 i=0;i<Length;i++)
				Buffer[i] = 0;
			for(u32 i=1;i<Ibus.DataNumber;i++)
				Buffer[i-1] = Ibus.Buffer[i];
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::FileReadLine(u8 Length,u8 *Buffer){
	char TempStr[20] = {0};
	TempStr[0] = 'F';
	TempStr[1] = 'R';
	TempStr[2] = 'E';
	TempStr[3] = 'A';
	TempStr[4] = 'D';
	TempStr[5] = 'L';

	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,200); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else{
			for(u8 i=0;i<Length;i++)
				Buffer[i] = 0;
			for(u32 i=1;i<Ibus.DataNumber;i++)
				Buffer[i-1] = Ibus.Buffer[i];
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}
char FCONI::FilePointerSeek(bool Forward,u32 Offset){
	char TempStr[20] = {0};
	TempStr[0] = 'F';
	TempStr[1] = 'P';
	TempStr[2] = 'S';
	TempStr[3] = 'E';
	TempStr[4] = 'E';
	TempStr[5] = 'K';
	
	if(Forward) TempStr[6] = 'F';
	else TempStr[6] = 'B';
	
	NumberToChar(10,Offset,(u8*)(TempStr+7));
	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',TempStr,50); //发送并记录回复状态
		if(ReplyReceived && !Ibus.TestReply('R')){ //收到回复且校验成功
			if(!Ibus.TestReply('N') && !Ibus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Ibus.TestReply('D')) return 'D'; //Done
		else if(Ibus.TestReply('K')) return 'K'; //Failed
		else if(Ibus.TestReply('R')) return 'R'; //Verifying Error
		else if(Ibus.TestReply('N')) return 'N'; //Instruction Error
		else return 'U'; //Unknown Status
	}
	else //未收到回复
		return 'T';
}

bool FCONI::TestLink(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Ibus.Transmit('F',"LKTEST",5); //发送并记录回复状态
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

u32 FCONI::CharToNumber(u8 InLength,u8 *Data){
	if(InLength > 10) InLength = 10;
	
	const u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	
	u64 Value = 0;
	for(u8 i=0;i<InLength;i++)
		Value += (Data[i]-'0')*Weight[InLength-i];
	
	return Value;
}

u8* FCONI::NumberToChar(u8 OutLength,u32 Data,u8 *Str){
	const u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	
	u8 *Ptr = Str;
	for(u8 i=0;i<OutLength;i++)
		*(Ptr++) = Data%Weight[OutLength-i+1]/Weight[OutLength-i] + '0';
	return Ptr;
}



