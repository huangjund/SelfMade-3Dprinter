#include "AuxIO.h"
#include "ABUS.h"
#include "Status.h"
#define s64 long long

AUXIO AuxIO;

char AUXIO::GetStatusMaterialA(){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Abus.Transmit('1',"GETSW1",10); //发送并记录回复状态
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else{
			if(Abus.TestReply('D')) Status.MaterialA = 1;
			else Status.MaterialA = 0;
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

char AUXIO::GetStatusMaterialB(){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Abus.Transmit('1',"GETSW2",10); //发送并记录回复状态
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else{
			if(Abus.TestReply('D')) Status.MaterialB = 1;
			else Status.MaterialB = 0;
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}

/*char AUXIO::GetStatusSW3(){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Abus.Transmit('1',"GETSW3",10); //发送并记录回复状态
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else{
			if(Abus.TestReply('D')) //Status.MaterialB = 1;
			else //Status.MaterialB = 0;
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}*/

/*char AUXIO::GetStatusSW4(){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Abus.Transmit('1',"GETSW4",10); //发送并记录回复状态
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else{
			if(Abus.TestReply('D')) //Status.MaterialB = 1;
			else //Status.MaterialB = 0;
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}*/

/*char AUXIO::GetStatusSW5(){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Abus.Transmit('1',"GETSW5",10); //发送并记录回复状态
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else{
			if(Abus.TestReply('D')) //Status.MaterialB = 1;
			else //Status.MaterialB = 0;
			return 'D';
		}
	}
	else //未收到回复
		return 'T';
}*/

char AUXIO::SetStatusMainLight(bool Bit){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		if(Bit) ReplyReceived = Abus.Transmit('1',"ENOUT1",10); //发送并记录回复状态
		else ReplyReceived = Abus.Transmit('1',"DIOUT1",10);
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else return 'D';
	}
	else //未收到回复
		return 'T';
}

char AUXIO::SetStatusAuxiliaryLight(bool Bit){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		if(Bit) ReplyReceived = Abus.Transmit('1',"ENOUT2",10); //发送并记录回复状态
		else ReplyReceived = Abus.Transmit('1',"DIOUT2",10);
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else return 'D';
	}
	else //未收到回复
		return 'T';
}

/*char AUXIO::SetStatusSwitch3(bool Bit){	
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		if(Bit) ReplyReceived = Abus.Transmit('1',"ENOUT3",10); //发送并记录回复状态
		else ReplyReceived = Abus.Transmit('1',"DIOUT3",10);
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N') && !Abus.TestIfVerifyError()) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('R')) return 'R'; //Verifying Error
		else if(Abus.TestReply('N')) return 'N'; //Instruction Error
		else return 'D';
	}
	else //未收到回复
		return 'T';
}*/

bool AUXIO::TestLink(){
	bool ReplyReceived = 0;
	for(u32 i=0;i<5;i++){ //重复最多5次
		ReplyReceived = Abus.Transmit('1',"TSTLIK",10); //发送并记录回复状态
		if(ReplyReceived && !Abus.TestReply('R')){ //收到回复且校验成功
			if(!Abus.TestReply('N')) //指令正确
				break;
		}
	}
	
	if(ReplyReceived){ //收到回复
		if(Abus.TestReply('D')) return 1; //Done
		else if(Abus.TestReply('R')) return 0; //Verifying Error
		else if(Abus.TestReply('N')) return 0; //Instruction Error
		else return 0; //Unknown Status
	}
	else //未收到回复
		return 0;	
}


