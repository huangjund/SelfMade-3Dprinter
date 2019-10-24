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
	char Recv = PreCheck(); //检测
	if(Recv == 'D'){ //正确
		RoundStatus = 'G'; //进入取指令状态
		Report.Used = 0;
	}
	return Recv;
}

char INTERPRETER::Stop(){
	if(RoundStatus == 'I') //空闲状态
		return 'I';
	else //其他状态
		Signal = 'H'; //发送停止信号
	return 'D';
}

char INTERPRETER::Pause(){
	if(RoundStatus == 'I') //空闲状态
		return 'I';
	else //其他状态
		Signal = 'B'; //发送暂停信号
	return 'D';
}

char INTERPRETER::Resume(){
	if(RoundStatus == 'I')
		return 'I';
	if(RoundStatus != 'R' && RoundStatus != 'B')
		return 'K';
	else
		Signal = 'S'; //发送继续信号
	return 'D';
}

char INTERPRETER::ReLoad(){
	if(RoundStatus == 'I') //空闲状态
		return 'I';
	else //其他状态
		Signal = 'R'; //发送换料信号
	return 'D';
}

void INTERPRETER::Round(){
	bool RoundError = 1;
	Info.CountEnable = 0;
	if(RoundStatus == 'I'){ //空闲状态
		RoundError = 0;
		RoundStatus = 'I';
	}
	if(RoundStatus == 'G'){ //取指令状态
		RoundError = 0;
		if(!Status.CommandNumber)
			Info.Percent = 0;
		else if(StreamStatus.InsCounter*100/Status.CommandNumber > 99)
			Info.Percent = 99;
		else
			Info.Percent = StreamStatus.InsCounter*100/Status.CommandNumber;

		Info.CountEnable = 1;
		char ReValue = 'D';
		if(RestBuffer > 2){ //缓存足够大
			ReValue = GetInstruction(); //取指令
			if(ReValue != 'D'){ //出错
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
	if(RoundStatus == 'A'){ //指令解析状态
		RoundError = 0;
		Info.CountEnable = 1;
		LocalAnalyse(); //指令解析
		RoundStatus = 'C'; 
	}
	if(RoundStatus == 'C'){ //状态检测状态
		RoundError = 0;
		Info.CountEnable = 1;
		char ReValue = 'D';
		ReValue = StatusCheck(); //状态检测
		if(ReValue != 'D'){ //出错
			ErrorStep = 1;
			ErrorCode = ReValue;
			RoundStatus = 'E';
		}
		else{
			RoundStatus = 'P';
		}
	}
	if(RoundStatus == 'P'){ //执行状态
		RoundError = 0;
		Info.CountEnable = 1;
		char ReValue = 'D';
		ReValue = Process(); //执行
		if(ReValue != 'D'){ //出错
			ErrorStep = 2;
			ErrorCode = ReValue;
			RoundStatus = 'E';
		}
		else{
			RoundStatus = 'S';
		}
	}
	if(RoundStatus == 'E'){ //错误处理状态
		RoundError = 0;
		Info.CountEnable = 1;
		if(ErrorStep == 0){ //取指令时出错
			switch(ErrorCode){
				case 'E': //空指令 W@S
					if(Status.FilePointer != Status.FileSize){ //未达文件结尾
						Info.Warning[Info.WarningLoad][0] = 0; //警告位置
						Info.Warning[Info.WarningLoad][1] = 'E'; //警告代号
						Info.WarningLoad = (Info.WarningLoad+1)%10; //下一个
					}
					RoundStatus = 'A'; //继续执行
					break; 
				case 'R': //校验错 E@B
					Info.Error[0] = 0; //错误位置
					Info.Error[1] = 'R'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				default: //没有错误
					RoundStatus = 'A'; //继续执行
					break;
			}
		}
		else if(ErrorStep == 1){ //状态检测时出错
			switch(ErrorCode){
				case 'F': //文件不存在 E@B
					Info.Error[0] = 1; //错误位置
					Info.Error[1] = 'F'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				case 'E': //缓存空 W@S
					if(Status.FilePointer != Status.FileSize){ //未达文件结尾
						Info.Warning[Info.WarningLoad][0] = 1; //警告位置
						Info.Warning[Info.WarningLoad][1] = 'E'; //警告代号
						Info.WarningLoad = (Info.WarningLoad+1)%10; //下一个
					}
					RoundStatus = 'P'; //继续执行
					break;
				case 'H': //未复位 E@B
					Info.Error[0] = 1; //错误位置
					Info.Error[1] = 'H'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				case 'A': //A缺料 E@R
					Info.Error[0] = 1; //错误位置
					Info.Error[1] = 'A'; //错误代号
					Signal = 'R';
					RoundStatus = 'S'; //暂停执行
					break;
				case 'B': //B缺料 E@R
					Info.Error[0] = 1; //错误位置
					Info.Error[1] = 'B'; //错误代号
					Signal = 'R';
					RoundStatus = 'S'; //暂停执行
					break;
				default: //没有错误
					RoundStatus = 'P';
					break;
			}
		}
		else if(ErrorStep == 2){ //执行时出错
			switch(ErrorCode){
				case 'E': //缓存空 W@S
					if(Status.FilePointer != Status.FileSize){ //未达文件结尾
						Info.Warning[Info.WarningLoad][0] = 2; //警告位置
						Info.Warning[Info.WarningLoad][1] = 'E'; //警告代号
						Info.WarningLoad = (Info.WarningLoad+1)%10; //下一个
					}
					RoundStatus = 'S'; //继续执行
					break;
				case 'H': //未复位 E@B
					Info.Error[0] = 2; //错误位置
					Info.Error[1] = 'H'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				case 'R': //通信校验错误 E@B
					Info.Error[0] = 2; //错误位置
					Info.Error[1] = 'R'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				case 'N': //通信错误 E@B
					Info.Error[0] = 2; //错误位置
					Info.Error[1] = 'N'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				case 'T': //通信超时 E@B
					Info.Error[0] = 2; //错误位置
					Info.Error[1] = 'T'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				case 'U': //其他原因通信错 E@B
					Info.Error[0] = 2; //错误位置
					Info.Error[1] = 'U'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				case '2': //温度设置过高 W@S
					Info.Warning[Info.WarningLoad][0] = 2; //警告位置
					Info.Warning[Info.WarningLoad][1] = '2'; //警告代号
					Info.WarningLoad = (Info.WarningLoad+1)%10; //下一个
					RoundStatus = 'S'; //继续执行
					break;
				case '1': //温度设置过低 W@S
					Info.Warning[Info.WarningLoad][0] = 2; //警告位置
					Info.Warning[Info.WarningLoad][1] = '1'; //警告代号
					Info.WarningLoad = (Info.WarningLoad+1)%10; //下一个
					RoundStatus = 'S'; //继续执行
					break;
				case 'X': //温度计故障 E@B
					Info.Error[0] = 2; //错误位置
					Info.Error[1] = 'X'; //错误代号
					Signal = 'B';
					RoundStatus = 'S'; //暂停执行
					break;
				default: //没有错误
					RoundStatus = 'S';
					break;
			}
		}
		else{ //没有错
			RoundStatus = 'S';
		}
	}
	if(RoundStatus == 'S'){ //信号处理状态
		RoundError = 0;
		Info.CountEnable = 1;
		switch(Signal){
			case 'B': //暂停信号
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
						Signal = 0; //清除信号
						RoundStatus = 'B'; //进入暂停状态
					}
					else{
						RoundStatus = 'S';						
					}
				}
				break;
			case 'H': //停止信号
				Signal = 0; //清除信号
				RoundStatus = 'H'; //进入停止状态
				break;
			case 'R': //换料信号
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
						Signal = 0; //清除信号
						RoundStatus = 'R'; //进入换料状态
					}
					else{
						RoundStatus = 'S';						
					}
				}
				break;
			default: //没有信号
				Signal = 0; //清除信号
				if(RestBuffer != BUFFER_SIZE) //缓存仍有剩余
					RoundStatus = 'G'; //进入取指令状态
				else if(Status.FilePointer == Status.FileSize) //缓存空且文件读取完
					RoundStatus = 'H'; //进入停止状态
				else //缓存空但文件未读取完
					RoundStatus = 'G'; //进入取指令状态
				break;
		}
	}
	if(RoundStatus == 'R'){ //换料状态
		RoundError = 0;
		Info.CountEnable = 0;
		switch(Signal){
			case 'S': //退出换料状态
				if(Sconi.Motion(PrePosition,20) == 'D'){
					Signal = 0; //清除信号
					RoundStatus = 'S'; //进入信号处理状态		
				}
				else{
					RoundStatus = 'R';
				}
				break;
			case 'B': //暂停信号
				Signal  = 0; //清除信号
				RoundStatus = 'B'; //进入暂停状态
				break;
			case 'H': //停止信号
				Signal = 0; //清除信号
				RoundStatus = 'H'; //进入停止状态
				break;
			default:
				Signal = 0; //清除信号
				RoundStatus = 'R'; //保持换料状态
		}
	}
	if(RoundStatus == 'B'){ //暂停状态
		RoundError = 0;
		Info.CountEnable = 0;
		switch(Signal){
			case 'S': //退出暂停状态
				if(Sconi.Motion(PrePosition,20) == 'D'){
					Signal = 0; //清除信号
					RoundStatus = 'S'; //进入信号处理状态		
				}
				else{
					RoundStatus = 'R';
				}
				break;
			case 'R': //换料信号
				Signal  = 0; //清除信号
				RoundStatus = 'R'; //进入换料状态
				break;
			case 'H': //停止信号
				Signal = 0; //清除信号
				RoundStatus = 'H'; //进入停止状态
				break;
			default:
				Signal = 0; //清除信号
				RoundStatus = 'B'; //保持暂停状态
		}
	}
	if(RoundStatus == 'H'){ //停止状态
		RoundError = 0;
		Info.CountEnable = 0;
		for(u8 i=0;i<3;i++)
			Report.HMS[i] = Info.HMS[i];
		Report.InsCounter = StreamStatus.InsCounter;
		Report.Used = 1;
		Reset(); //复位
		RoundStatus = 'I'; //进入空闲状态
	}
	if(RoundError){ //其他
		RoundStatus = 'I'; //进入空闲状态
	}
}

void INTERPRETER::Tick(){ //计时
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
	if(RoundStatus != 'I') return 'K'; //正忙	
	if(!Sconi.TestLink()) return '1'; //运行系统故障
	if(!Tconi.TestLink()) return '2'; //热力系统故障
	if(!Fconi.TestLink()) return '3'; //文件系统故障
	if(!AuxIO.TestLink()) return '4'; //辅助系统故障
	Sconi.GetStatusHomed();
	if(!Status.StateHomed) return 'H'; //未复位
	Fconi.FileOpen();
	if(!Status.FileExist) return 'F'; //文件不存在
	
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
	
	Fconi.GetFilePointer(); //获取文件指针
	TempIns.BaseAddress = Status.FilePointer; //记录文件指针
	Fconi.FileReadLine(128,TempData); //读取一行指令
	
	if(!TempData[0]) return 'E'; //空指令

	for(u8 k=0;k<128&&TempData[k];k++){ //提取指令参数
		if(TempData[k] == 'G'){
			u8 Temp = AbsDecimal(CharToDecimal(TempData+k+1));
			if(Temp==0 || Temp==1) Buffer[LoadIndex].Type = 1; //移动
			if(Temp == 28) TempIns.GM_Para = 28; //复位
			if(Temp == 90) TempIns.GM_Para = 90; //绝对坐标
			if(Temp == 91) TempIns.GM_Para = 91; //相对坐标
			if(Temp == 92) TempIns.GM_Para = 92; //设置
			TempIns.GM_Exis = 1;
		}
		if(TempData[k] == 'M'){
			u8 Temp = AbsDecimal(CharToDecimal(TempData+k+1));
			if(Temp == 104) TempIns.GM_Para = 104; //设置挤出机温度
			if(Temp == 106) TempIns.GM_Para = 106; //设置风扇状态
			if(Temp == 107) TempIns.GM_Para = 107; //关闭风扇
			if(Temp == 109) TempIns.GM_Para = 109; //设置挤出机温度/阻塞
			if(Temp == 140) TempIns.GM_Para = 140; //设置基准面温度
			if(Temp == 190) TempIns.GM_Para = 190; //设置基准面温度/阻塞
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
	
	u32 Verify = 0; //校验和
	for(u8 i=0;i<128&&TempData[i];i++){
		if(TempData[i] == '*') break;
		Verify += TempData[i];
	}
	
	if(Verify%100==TempIns.V_Para || !TempIns.V_Exis)
		return 'D'; //校验成功
	else
		return 'R'; //校验失败
}

void INTERPRETER::LocalAnalyse(){
	if(TempIns.GM_Exis){ //Gx Mx
		if(TempIns.GM_Para==0 || TempIns.GM_Para==1){ //G0 G1移动指令
			Buffer[LoadIndex].Type = 1; //指令编号
			Buffer[LoadIndex].Segment = 0; //分段编号
			for(u8 i=0;i<5;i++) //设置存在坐标参数
				Buffer[LoadIndex].AxisExis[i] = 1;
			
			if(TempIns.X_Exis){ //已指定X坐标参数
				if(StreamStatus.AbsoluteCrod){ //文件使用绝对坐标
					StreamStatus.PositionPres[0] = TempIns.X_Para; //记录X坐标					
					Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0]; //写入X坐标
				}
				else{ //文件使用相对坐标
					StreamStatus.PositionPres[0] += TempIns.X_Para; //计算X坐标
					Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0]; //写入X坐标
				}
			}
			else{ //未指定X坐标参数
				Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0]; //写入X坐标
			}
			if(TempIns.Y_Exis){ //已指定Y坐标参数
				if(StreamStatus.AbsoluteCrod){ //文件使用绝对坐标
					StreamStatus.PositionPres[1] = TempIns.Y_Para; //记录Y坐标					
					Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1]; //写入Y坐标
				}
				else{ //文件使用相对坐标
					StreamStatus.PositionPres[1] += TempIns.Y_Para; //计算Y坐标
					Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1]; //写入Y坐标
				}
			}
			else{ //未指定Y坐标参数
				Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1]; //写入Y坐标
			}
			if(TempIns.Z_Exis){ //已指定Z坐标参数
				if(StreamStatus.AbsoluteCrod){ //文件使用绝对坐标
					StreamStatus.PositionPres[2] = TempIns.Z_Para; //记录Z坐标					
					Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2]; //写入Z坐标
				}
				else{ //文件使用相对坐标
					StreamStatus.PositionPres[2] += TempIns.Z_Para; //计算Z坐标
					Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2]; //写入Z坐标
				}
			}
			else{ //未指定Z坐标参数
				Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2]; //写入Z坐标
			}
			if(TempIns.A_Exis){ //已指定A坐标参数
				if(StreamStatus.AbsoluteCrod){ //文件使用绝对坐标
					StreamStatus.PositionPres[3] = TempIns.A_Para; //记录A坐标					
					Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //写入A坐标
				}
				else{ //文件使用相对坐标
					StreamStatus.PositionPres[3] += TempIns.A_Para; //计算A坐标
					Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //写入A坐标
				}
			}
			else{ //未指定A坐标参数
				Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //写入A坐标
			}
			if(TempIns.B_Exis){ //已指定B坐标参数
				if(StreamStatus.AbsoluteCrod){ //文件使用绝对坐标
					StreamStatus.PositionPres[4] = TempIns.B_Para; //记录B坐标					
					Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //写入B坐标
				}
				else{ //文件使用相对坐标
					StreamStatus.PositionPres[4] += TempIns.B_Para; //计算B坐标
					Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //写入B坐标
				}
			}
			else{ //未指定B坐标参数
				Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //写入B坐标
			}
			if(!TempIns.A_Exis && !TempIns.B_Exis && TempIns.E_Exis){ //已指定A/B坐标参数
				if(!StreamStatus.ExtruderPres){ //当前使用挤出机A
					if(StreamStatus.AbsoluteCrod){ //文件使用绝对坐标
						StreamStatus.PositionPres[3] = TempIns.E_Para; //记录A坐标					
						Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //写入A坐标
					}
					else{ //文件使用相对坐标
						StreamStatus.PositionPres[3] += TempIns.E_Para; //计算A坐标
						Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3]; //写入A坐标
					}
				}
				else{ //当前使用挤出机B
					if(StreamStatus.AbsoluteCrod){ //文件使用绝对坐标
						StreamStatus.PositionPres[4] = TempIns.E_Para; //记录B坐标					
						Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //写入B坐标
					}
					else{ //文件使用相对坐标
						StreamStatus.PositionPres[4] += TempIns.E_Para; //计算B坐标
						Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4]; //写入B坐标
					}
				}
			}
			if(TempIns.F_Exis){ //已指定F参数
				StreamStatus.VelocityPres = TempIns.F_Para/60; //记录速度
				if(StreamStatus.VelocityPres < 10) //速度不低于10mm/s
					StreamStatus.VelocityPres = 10;
				Buffer[LoadIndex].Velocity = StreamStatus.VelocityPres; //写入速度
			}
			else{ //未指定F参数
				if(StreamStatus.VelocityPres < 10) //速度不低于10mm/s
					StreamStatus.VelocityPres = 10;
				Buffer[LoadIndex].Velocity = StreamStatus.VelocityPres; //写入速度
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
		}
		else if(TempIns.GM_Para == 28){ //G28复位指令
			Buffer[LoadIndex].Type = 28; //指令编号
			Buffer[LoadIndex].Segment = 0; //分段编号
			for(u8 i=0;i<5;i++) //清除存在坐标参数
				Buffer[LoadIndex].AxisExis[i] = 0;
			
			bool AllAxis = 1;
			bool ABExis = 0;
			if(TempIns.X_Exis){ //已指定复位X轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[0] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[0] = 0; //X坐标
				StreamStatus.PositionPres[0] = 0; //清除X坐标
			}else{ //未指定复位X轴
				Buffer[LoadIndex].Position[0] = StreamStatus.PositionPres[0];
			}
			if(TempIns.Y_Exis){ //已指定复位Y轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[1] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[1] = 0;
				StreamStatus.PositionPres[1] = 0; //清除Y坐标
			}else{ //未指定复位Y轴
				Buffer[LoadIndex].Position[1] = StreamStatus.PositionPres[1];
			}
			if(TempIns.Z_Exis){ //已指定复位Z轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[2] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[2] = 0;
				StreamStatus.PositionPres[2] = 0; //清除Z坐标
			}else{ //未指定复位Z轴
				Buffer[LoadIndex].Position[2] = StreamStatus.PositionPres[2];
			}
			if(TempIns.A_Exis){ //已指定复位A轴
				AllAxis = 0;
				ABExis = 1;
				Buffer[LoadIndex].AxisExis[3] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3];
				StreamStatus.PositionPres[3] = 0; //清除A坐标
			}
			if(TempIns.B_Exis){ //已指定复位B轴
				AllAxis = 0;
				ABExis = 1;
				Buffer[LoadIndex].AxisExis[4] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4];
				StreamStatus.PositionPres[4] = 0; //清除B坐标
			}
			if(TempIns.E_Exis){ //已指定复位A/B轴
				AllAxis = 0;
				ABExis = 1;
				if(!StreamStatus.ExtruderPres){ //挤出机A
					Buffer[LoadIndex].AxisExis[3] = 1; //设置存在坐标参数
					Buffer[LoadIndex].Position[3] = StreamStatus.PositionPres[3];
					StreamStatus.PositionPres[3] = 0; //清除A坐标
				}
				else{ //挤出机B
					Buffer[LoadIndex].AxisExis[4] = 1; //设置存在坐标参数
					Buffer[LoadIndex].Position[4] = StreamStatus.PositionPres[4];
					StreamStatus.PositionPres[4] = 0; //清除B坐标
				}
			}
			if(AllAxis){ //复位所有轴
				ABExis = 1;
				for(u8 i=0;i<3;i++){
					Buffer[LoadIndex].AxisExis[i] = 1; //设置存在坐标参数 
					Buffer[LoadIndex].Position[i] = 0;
					StreamStatus.PositionPres[i] = 0; //清除坐标
				}
				for(u8 i=3;i<5;i++){
					Buffer[LoadIndex].AxisExis[i] = 1; //设置存在坐标参数
					Buffer[LoadIndex].Position[i] = StreamStatus.PositionPres[i];
					StreamStatus.PositionPres[i] = 0; //清除坐标
				}	
			}
			Buffer[LoadIndex].Velocity = 30;
			
			u8 PreIndex = LoadIndex; //前一段索引
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
			
			if(ABExis){ //需要分段执行
				Buffer[LoadIndex].Type = Buffer[PreIndex].Type; //复制编号
				Buffer[LoadIndex].LineCounter = Buffer[PreIndex].LineCounter; //复制行号
				Buffer[LoadIndex].BaseAddress = Buffer[PreIndex].BaseAddress; //复制基址
				Buffer[LoadIndex].Segment = 1; //分段编号
				for(u8 i=0;i<5;i++){ //复制坐标
					Buffer[LoadIndex].AxisExis[i] = Buffer[PreIndex].AxisExis[i];
					Buffer[LoadIndex].Position[i] = Buffer[PreIndex].Position[i];
				}
				Buffer[LoadIndex].Velocity = Buffer[PreIndex].Velocity; //复制速度
				LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
				RestBuffer--; //剩余缓存数量-1
			}
		}
		else if(TempIns.GM_Para == 90){ //G90使用绝对坐标
			StreamStatus.AbsoluteCrod = 1; //设置使用绝对坐标
		}
		else if(TempIns.GM_Para == 91){ //G91使用相对坐标
			StreamStatus.AbsoluteCrod = 0; //设置使用相对坐标
		}
		else if(TempIns.GM_Para == 92){ //G92设置坐标
			Buffer[LoadIndex].Type = 92; //指令编号
			Buffer[LoadIndex].Segment = 0; //分段编号
			for(u8 i=0;i<5;i++){ //清除存在坐标参数
				Buffer[LoadIndex].AxisExis[i] = 0;
				Buffer[LoadIndex].Position[i] = 0;
			}
			bool AllAxis = 1;
			if(TempIns.X_Exis){ //已指定设置X轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[0] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[0] = TempIns.X_Para; //设置X坐标
				StreamStatus.PositionPres[0] = TempIns.X_Para; //记录X坐标
			}
			if(TempIns.Y_Exis){ //已指定设置Y轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[1] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[1] = TempIns.Y_Para; //设置Y坐标
				StreamStatus.PositionPres[1] = TempIns.Y_Para; //记录Y坐标
			}
			if(TempIns.Z_Exis){ //已指定设置Z轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[2] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[2] = TempIns.Z_Para; //设置Z坐标
				StreamStatus.PositionPres[2] = TempIns.Z_Para; //记录Z坐标
			}
			if(TempIns.A_Exis){ //已指定设置A轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[3] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[3] = TempIns.A_Para; //设置A坐标
				StreamStatus.PositionPres[3] = TempIns.A_Para; //记录A坐标
			}
			if(TempIns.B_Exis){ //已指定设置B轴
				AllAxis = 0;
				Buffer[LoadIndex].AxisExis[4] = 1; //设置存在坐标参数
				Buffer[LoadIndex].Position[4] = TempIns.B_Para; //设置B坐标
				StreamStatus.PositionPres[4] = TempIns.B_Para; //记录B坐标
			}
			if(TempIns.E_Exis){ //已指定设置A/B轴
				AllAxis = 0;
				if(!StreamStatus.ExtruderPres){ //挤出机A
					Buffer[LoadIndex].AxisExis[3] = 1; //设置存在坐标参数
					Buffer[LoadIndex].Position[3] = TempIns.E_Para; //设置A坐标
					StreamStatus.PositionPres[3] = TempIns.E_Para; //记录A坐标
				}
				else{ //挤出机B
					Buffer[LoadIndex].AxisExis[4] = 1; //设置存在坐标参数
					Buffer[LoadIndex].Position[4] = TempIns.E_Para; //设置B坐标
					StreamStatus.PositionPres[4] = TempIns.E_Para; //记录B坐标
				}
			}
			if(AllAxis){ //所有轴设置为0
				for(u8 i=0;i<5;i++){
					Buffer[LoadIndex].AxisExis[i] = 1; //设置存在坐标参数
					Buffer[LoadIndex].Position[i] = 0; //设置坐标
					StreamStatus.PositionPres[i] = 0; //清除坐标
				} 
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
		}
		else if(TempIns.GM_Para == 104){ //M104设置挤出机温度/非阻塞
			Buffer[LoadIndex].Type = 104; //指令编号
			Buffer[LoadIndex].Segment = 0; //分段编号
			Buffer[LoadIndex].ExtruderExis[0] = 0;
			Buffer[LoadIndex].TemperatureE[0] = 0;
			Buffer[LoadIndex].ExtruderExis[1] = 0;
			Buffer[LoadIndex].TemperatureE[1] = 0;
			
			if(TempIns.T_Exis){ //已指定挤出机
				if(!TempIns.T_Para){ //挤出机A
					StreamStatus.ExtruderPres = 0; //当前使用挤出机A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //选择挤出机A
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[0] = 0; //清除选择
				}
				else{ //挤出机B
					StreamStatus.ExtruderPres = 1; //当前使用挤出机B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //选择挤出机B
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[1] = 0; //清除选择	
				}
			}
			else{ //未指定挤出机
				if(!StreamStatus.ExtruderPres){ //挤出机A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //选择挤出机A
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[0] = 0; //清除选择
				}
				else{ //挤出机B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //选择挤出机B
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[1] = 0; //清除选择	
				}
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
		}
		else if(TempIns.GM_Para == 106){ //M106设置风扇状态
			Buffer[LoadIndex].Type = 106;
			Buffer[LoadIndex].Segment = 0; //分段编号
			Buffer[LoadIndex].CoolerExis[0] = 0;
			Buffer[LoadIndex].CoolerExis[1] = 0;
			Buffer[LoadIndex].CoolerPower[0] = 0;
			Buffer[LoadIndex].CoolerPower[1] = 0;
			
			if(TempIns.T_Exis){ //已指定风扇
				if(!TempIns.T_Para){ //风扇A
					Buffer[LoadIndex].CoolerExis[0] = 1; //选择风扇A
					if(TempIns.P_Exis) //使用P参数指定状态
						Buffer[LoadIndex].CoolerPower[0] = TempIns.P_Para*20/51; //写入状态
					else if(TempIns.S_Exis) //使用S参数指定状态
						Buffer[LoadIndex].CoolerPower[0] = TempIns.S_Para*20/51; //写入状态
					else //未指定状态
						Buffer[LoadIndex].CoolerExis[0] = 0; //清除选择
				}
				else{ //风扇B
					Buffer[LoadIndex].CoolerExis[1] = 1; //选择风扇B
					if(TempIns.P_Exis) //使用P参数指定状态
						Buffer[LoadIndex].CoolerPower[1] = TempIns.P_Para*20/51; //写入状态
					else if(TempIns.S_Exis) //使用S参数指定状态
						Buffer[LoadIndex].CoolerPower[1] = TempIns.S_Para*20/51; //写入状态
					else //未指定状态
						Buffer[LoadIndex].CoolerExis[1] = 0; //清除选择	
				}
			}
			else{ //未指定风扇
				Buffer[LoadIndex].CoolerExis[0] = 1; //选择风扇A
				Buffer[LoadIndex].CoolerExis[1] = 1; //选择风扇B
				if(TempIns.P_Exis){ //使用P参数指定状态
					Buffer[LoadIndex].CoolerPower[0] = TempIns.P_Para*20/51; //写入状态
					Buffer[LoadIndex].CoolerPower[1] = TempIns.P_Para*20/51; //写入状态
				}
				else if(TempIns.S_Exis){ //使用S参数指定状态
					Buffer[LoadIndex].CoolerPower[0] = TempIns.S_Para*20/51; //写入状态
					Buffer[LoadIndex].CoolerPower[1] = TempIns.P_Para*20/51; //写入状态
				}
				else{ //未指定状态
					Buffer[LoadIndex].CoolerExis[0] = 0; //清除选择
					Buffer[LoadIndex].CoolerExis[1] = 0; //清除选择
				}
			}
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
		}
		else if(TempIns.GM_Para == 107){ //M107关闭风扇
			Buffer[LoadIndex].Type = 107;
			Buffer[LoadIndex].Segment = 0; //分段编号
			Buffer[LoadIndex].CoolerExis[0] = 0;
			Buffer[LoadIndex].CoolerExis[1] = 0;
			Buffer[LoadIndex].CoolerPower[0] = 0;
			Buffer[LoadIndex].CoolerPower[1] = 0;
			
			if(TempIns.T_Exis){ //已指定风扇
				if(!TempIns.T_Para) //风扇A
					Buffer[LoadIndex].CoolerExis[0] = 1; //选择风扇A
				else //风扇B
					Buffer[LoadIndex].CoolerExis[1] = 1; //选择风扇B
			}
			else{ //未指定风扇
				Buffer[LoadIndex].CoolerExis[0] = 1; //选择风扇A
				Buffer[LoadIndex].CoolerExis[1] = 1; //选择风扇B
			}

			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针	
			RestBuffer--; //剩余缓存数量-1			
		}
		else if(TempIns.GM_Para == 109){ //M109设置挤出机温度/阻塞
			Buffer[LoadIndex].Type = 109; //指令编号
			Buffer[LoadIndex].Segment = 0; //分段编号
			Buffer[LoadIndex].ExtruderExis[0] = 0;
			Buffer[LoadIndex].TemperatureE[0] = 0;
			Buffer[LoadIndex].ExtruderExis[1] = 0;
			Buffer[LoadIndex].TemperatureE[1] = 0;
			
			if(TempIns.T_Exis){ //已指定挤出机
				if(!TempIns.T_Para){ //挤出机A
					StreamStatus.ExtruderPres = 0; //当前使用挤出机A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //选择挤出机A
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[0] = 0; //清除选择
				}
				else{ //挤出机B
					StreamStatus.ExtruderPres = 1; //当前使用挤出机B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //选择挤出机B
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[1] = 0; //清除选择	
				}
			}
			else{ //未指定挤出机
				if(!StreamStatus.ExtruderPres){ //挤出机A
					Buffer[LoadIndex].ExtruderExis[0] = 1; //选择挤出机A
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[0] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[0] = 0; //清除选择
				}
				else{ //挤出机B
					Buffer[LoadIndex].ExtruderExis[1] = 1; //选择挤出机B
					if(TempIns.P_Exis) //使用P参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.P_Para; //写入温度
					else if(TempIns.S_Exis) //使用S参数指定温度
						Buffer[LoadIndex].TemperatureE[1] = TempIns.S_Para; //写入温度
					else //未指定温度
						Buffer[LoadIndex].ExtruderExis[1] = 0; //清除选择	
				}
			}
			
			u8 PreIndex = LoadIndex;
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针		
			RestBuffer--; //剩余缓存数量-1
			
			Buffer[LoadIndex].Type = Buffer[PreIndex].Type; //复制编号
			Buffer[LoadIndex].LineCounter = Buffer[PreIndex].LineCounter; //复制行号
			Buffer[LoadIndex].BaseAddress = Buffer[PreIndex].BaseAddress; //复制基址
			Buffer[LoadIndex].Segment = 1; //分段编号
			for(u8 i=0;i<2;i++){ //复制温度
				Buffer[LoadIndex].ExtruderExis[i] = Buffer[PreIndex].ExtruderExis[i]; 
				Buffer[LoadIndex].TemperatureE[i] = Buffer[PreIndex].TemperatureE[i];
			}
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针		
			RestBuffer--; //剩余缓存数量-1
			
		}
		else if(TempIns.GM_Para == 140){ //M140设置基准面温度/非阻塞
			Buffer[LoadIndex].Type = 140; //指令编号
			Buffer[LoadIndex].BasePlaneExis = 0;
			Buffer[LoadIndex].TemperatureB = 0;
			
			Buffer[LoadIndex].BasePlaneExis = 1; //选择基准面
			if(TempIns.P_Exis) //使用P参数指定温度
				Buffer[LoadIndex].TemperatureB = TempIns.P_Para; //写入温度
			else if(TempIns.S_Exis) //使用S参数指定温度
				Buffer[LoadIndex].TemperatureB = TempIns.S_Para; //写入温度
			else //未指定温度
				Buffer[LoadIndex].BasePlaneExis = 0; //清除选择

			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
		}
		else if(TempIns.GM_Para == 190){ //M190设置基准面温度/阻塞
			Buffer[LoadIndex].Type = 190; //指令编号
			Buffer[LoadIndex].BasePlaneExis = 0;
			Buffer[LoadIndex].TemperatureB = 0;
			
			Buffer[LoadIndex].BasePlaneExis = 1; //选择基准面
			if(TempIns.P_Exis) //使用P参数指定温度
				Buffer[LoadIndex].TemperatureB = TempIns.P_Para; //写入温度
			else if(TempIns.S_Exis) //使用S参数指定温度
				Buffer[LoadIndex].TemperatureB = TempIns.S_Para; //写入温度
			else //未指定温度
				Buffer[LoadIndex].BasePlaneExis = 0; //清除选择

			u8 PreIndex = LoadIndex;
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
			
			Buffer[LoadIndex].Type = Buffer[PreIndex].Type; //复制编号
			Buffer[LoadIndex].LineCounter = Buffer[PreIndex].LineCounter; //复制行号
			Buffer[LoadIndex].BaseAddress = Buffer[PreIndex].BaseAddress; //复制基址
			Buffer[LoadIndex].Segment = 1; //分段编号
			Buffer[LoadIndex].BasePlaneExis = Buffer[PreIndex].BasePlaneExis; 
			Buffer[LoadIndex].TemperatureB = Buffer[PreIndex].TemperatureB;
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针		
			RestBuffer--; //剩余缓存数量-1
		}
	}
	else if(TempIns.T_Exis){ //Tnnn选择挤出机
		if(!TempIns.T_Para){ //选择挤出机A
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 0;
			Buffer[LoadIndex].AxisExis[3] = 1;
			Buffer[LoadIndex].AxisExis[4] = 0;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
			
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 1;
			Buffer[LoadIndex].AxisExis[3] = 1;
			Buffer[LoadIndex].AxisExis[4] = 0;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
			
			StreamStatus.ExtruderPres = 0;
		}
		else{ //选择挤出机B
			StreamStatus.ExtruderPres = 1;
			
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 0;
			Buffer[LoadIndex].AxisExis[3] = 0;
			Buffer[LoadIndex].AxisExis[4] = 1;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
			
			Buffer[LoadIndex].Type = 205;
			Buffer[LoadIndex].Segment = 1;
			Buffer[LoadIndex].AxisExis[3] = 0;
			Buffer[LoadIndex].AxisExis[4] = 1;
			
			Buffer[LoadIndex].BaseAddress = TempIns.BaseAddress; //记录指令基址
			Buffer[LoadIndex].LineCounter = StreamStatus.InsCounter; //记录指令行号
			StreamStatus.InsCounter++; //行号+1
			LoadIndex = (LoadIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer--; //剩余缓存数量-1
		}
	}
	else if(TempIns.F_Exis){ //Fnnn设置速度
		StreamStatus.VelocityPres = TempIns.F_Para/60;
		if(StreamStatus.VelocityPres < 10)
			StreamStatus.VelocityPres = 10;
	}
}

char INTERPRETER::StatusCheck(){
	if(!Status.FileExist) //文件不存在
		return 'F';
	if(Buffer[LaunchIndex].Type == 0) //缓存空
		return 'E'; //缓存空
	if(Status.StateHomed!='E' || Status.StateFA == 'N')
		return 'H'; //未复位	
	if(Buffer[LaunchIndex].Type == 1){ //移动指令
		if(Buffer[LaunchIndex].AxisExis[3]){ //有A轴位移
			AuxIO.GetStatusMaterialA();
			if(Status.MaterialA == 0) return 'A'; //A缺料
		}
		if(Buffer[LaunchIndex].AxisExis[4]){ //有B轴位移
			AuxIO.GetStatusMaterialB();
			if(Status.MaterialB == 0) return 'B'; //B缺料
		}
	}
	return 'D';
}

char INTERPRETER::Process(){
	if(Buffer[LaunchIndex].Type == 0){ //缓存空
		if(RestBuffer != BUFFER_SIZE){
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			RestBuffer++; //剩余缓存数量+1
		}
		return 'E';
	}
	else if(Buffer[LaunchIndex].Type == 1){ //移动指令
		char Recv = Sconi.Motion(Buffer[LaunchIndex].Position,Buffer[LaunchIndex].Velocity);
		if(Recv == 'D'){ //执行完毕
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE; //移动指针
			RestBuffer++; //剩余缓存数量+1
			return 'D';
		}
		else if(Recv == 'F') //缓存满,认为执行完毕
			return 'D';
		else //出错
			return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 28){ //复位指令
		if(!Buffer[LaunchIndex].Segment){ //需要实际移动
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
			if(Recv == 'D'){ //执行完毕
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE; //移动指针
				RestBuffer++; //剩余缓存数量+1
				return 'D';
			}
			else if(Recv == 'F') //缓存满,认为执行完毕
				return 'D';
			else //出错
				return Recv;
		}
		else{ //需要进行设置
			char Recv = 'D';
			if(Buffer[LaunchIndex].AxisExis[3]){
				Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[3],'A');
				if(Recv == 'K') return 'D'; //禁止操作,认为执行无误
				if(Recv != 'D') return Recv;
			}
			if(Buffer[LaunchIndex].AxisExis[4]){
				Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[4],'B');
				if(Recv == 'K') return 'D'; //禁止操作,认为执行无误
				if(Recv != 'D') return Recv;
			}
			
			RestBuffer++; //剩余缓存数量+1
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			return Recv;
		}
	}
	else if(Buffer[LaunchIndex].Type == 92){ //设置坐标
		char Recv = 'D';
		if(Buffer[LaunchIndex].AxisExis[0]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[0],'X');
			if(Recv == 'K') return 'D'; //禁止操作,认为执行无误
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[1]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[1],'Y');
			if(Recv == 'K') return 'D'; //禁止操作,认为执行无误
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[2]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[2],'Z');
			if(Recv == 'K') return 'D'; //禁止操作,认为执行无误
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[3]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[3],'A');
			if(Recv == 'K') return 'D'; //禁止操作,认为执行无误
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].AxisExis[4]){
			Recv = Sconi.SetPosition(Buffer[LaunchIndex].Position[4],'B');
			if(Recv == 'K') return 'D'; //禁止操作,认为执行无误
			if(Recv != 'D') return Recv;
		}
		
		RestBuffer++; //剩余缓存数量+1
		LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 104){ //设置挤出机温度/非阻塞
		char Recv = 'D';
		
		if(Buffer[LaunchIndex].ExtruderExis[0])
			Recv = Tconi.SetTemperatureEASetting(Buffer[LaunchIndex].TemperatureE[0]);
		else if(Buffer[LaunchIndex].ExtruderExis[1])
			Recv = Tconi.SetTemperatureEBSetting(Buffer[LaunchIndex].TemperatureE[1]);
		
		if(Recv=='D' || Recv=='H' || Recv=='L'){
			RestBuffer++; //剩余缓存数量+1
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			if(Recv == 'H') return '2';
			if(Recv == 'L') return '1';
		}
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 106){ //M106设置风扇状态
		char Recv = 'D';
		
		if(Buffer[LaunchIndex].CoolerExis[0]){
			Recv = Tconi.SetCoolerPowerExtruderA(Buffer[LaunchIndex].CoolerPower[0]);
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].CoolerExis[1]){
			Recv = Tconi.SetCoolerPowerExtruderB(Buffer[LaunchIndex].CoolerPower[1]);
			if(Recv != 'D') return Recv;
		}
		
		RestBuffer++; //剩余缓存数量+1
		LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 107){ //M107关闭风扇
		char Recv = 'D';
		
		if(Buffer[LaunchIndex].CoolerExis[0]){
			Recv = Tconi.SetCoolerPowerExtruderA(0);
			if(Recv != 'D') return Recv;
		}
		if(Buffer[LaunchIndex].CoolerExis[1]){
			Recv = Tconi.SetCoolerPowerExtruderB(0);
			if(Recv != 'D') return Recv;
		}
		
		RestBuffer++; //剩余缓存数量+1
		LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 109){ //M109设置挤出机温度/阻塞
		if(!Buffer[LaunchIndex].Segment){ //设置温度
			char Recv = 'D';
			if(Buffer[LaunchIndex].ExtruderExis[0])
				Recv = Tconi.SetTemperatureEASetting(Buffer[LaunchIndex].TemperatureE[0]);
			else if(Buffer[LaunchIndex].ExtruderExis[1])
				Recv = Tconi.SetTemperatureEBSetting(Buffer[LaunchIndex].TemperatureE[1]);

			if(Recv=='D' || Recv=='H' || Recv=='L'){
				RestBuffer++; //剩余缓存数量+1
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
				if(Recv == 'H') return '2';
				if(Recv == 'L') return '1';
			}
			return Recv;
		}
		else{ //等待温度到达
			if(Buffer[LaunchIndex].ExtruderExis[0]){
				Tconi.GetStatusExtruderA();
				Tconi.GetTemperatureExtruderA();
				Tconi.GetTemperatureEASetting();
				if(!Status.TempExtruderA)
					return 'X';
				else if(Status.TempExtruderASet<150 && Status.TempExtruderA<100){ //冷却完成
					RestBuffer++; //剩余缓存数量+1
					LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;		
				}					
				else if((Status.TempExtruderASet-2) < Status.TempExtruderA){
					if((Status.TempExtruderASet+2) > Status.TempExtruderA){
						RestBuffer++; //剩余缓存数量+1 
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
				else if(Status.TempExtruderBSet<150 && Status.TempExtruderB<100){ //冷却完成
					RestBuffer++; //剩余缓存数量+1
					LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;		
				}					
				else if((Status.TempExtruderBSet-2) < Status.TempExtruderB){
					if((Status.TempExtruderBSet+2) > Status.TempExtruderB){
						RestBuffer++; //剩余缓存数量+1 
						LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
					}
				}
			}
			return 'D';
		}
	}
	else if(Buffer[LaunchIndex].Type == 140){ //M140设置基准面温度/非阻塞
		char Recv = 'D';

		if(Buffer[LaunchIndex].BasePlaneExis)
			Recv = Tconi.SetTemperatureBPSetting(Buffer[LaunchIndex].TemperatureB);

		if(Recv=='D' || Recv=='H' || Recv=='L'){
			RestBuffer++; //剩余缓存数量+1
			LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			if(Recv == 'H') return '2';
			if(Recv == 'L') return '1';
		}
		return Recv;
	}
	else if(Buffer[LaunchIndex].Type == 190){ //M190设置基准面温度/阻塞
		if(!Buffer[LaunchIndex].Segment){ //设置温度
			char Recv = 'D';
			if(Buffer[LaunchIndex].BasePlaneExis)
				Recv = Tconi.SetTemperatureBPSetting(Buffer[LaunchIndex].TemperatureB);
			
			if(Recv=='D' || Recv=='H' || Recv=='L'){
				RestBuffer++; //剩余缓存数量+1
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
				if(Recv == 'H') return '2';
				if(Recv == 'L') return '1';
			}
			return Recv;
		}
		else{ //等待温度到达
			Tconi.GetStatusBasePlane();
			Tconi.GetTemperatureBasePlane();
			Tconi.GetTemperatureBPSetting();
			if(!Status.TempBasePlane)
				return 'X';
			else if(Status.TempBasePlaneSet<35 && Status.TempBasePlane<40){ //冷却完成
				RestBuffer++; //剩余缓存数量+1
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;		
			}
			else if(Status.TempBasePlaneSet>=110 && Status.TempBasePlane>=110){
				RestBuffer++; //剩余缓存数量+1 
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;	
			}
			else if((Status.TempBasePlaneSet-2) < Status.TempBasePlane){
				if((Status.TempBasePlaneSet+2) > Status.TempBasePlane){
					RestBuffer++; //剩余缓存数量+1 
					LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
				}
			}
			return 'D';
		}
	}
	else if(Buffer[LaunchIndex].Type == 205){ //切换喷头
		if(!Buffer[LaunchIndex].Segment){ //发送切换喷头指令
			u8 NozzleA = 0;
			u8 NozzleB = 0;
			if(Buffer[LaunchIndex].AxisExis[3]) NozzleA = 1;
			if(Buffer[LaunchIndex].AxisExis[4]) NozzleB = 1;
			char Recv = Sconi.SetNozzleState(NozzleA,NozzleB);
			if(Recv == 'M'){
				RestBuffer++; //剩余缓存数量+1 
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;	
				return 'D';
			}
			else return Recv;
		}
		else{ //等待切换完成
			if(!Sconi.GetStatusBusy()){
				RestBuffer++; //剩余缓存数量+1 
				LaunchIndex = (LaunchIndex+1)%BUFFER_SIZE;
			}
			return 'D';
		}
	}
	else{
		RestBuffer++; //剩余缓存数量+1 
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
	for(IntNumber=0;IntNumber<11;IntNumber++){ //统计整数位数
		if(TempPtr[0]<'0' || TempPtr[0]>'9') break;
		else TempPtr++;
	}
	
	for(u8 i=0;i<IntNumber;i++) //整数部分
		Number += (*(Ptr++)-'0')*WeightI[IntNumber-i];
	
	if(Ptr[0] == '.'){ //有小数
		Ptr++; //跳过小数点
		for(u8 i=0;i<11;i++){ //小数部分
			if(Ptr[0]<'0' || Ptr[0]>'9') break;
			else Number += (*(Ptr++)-'0')*WeightD[i+1];
		}
	}
	Number *= Sign; //添加符号
	
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
	for(IntNumber=0;IntNumber<11;IntNumber++){ //统计整数位数
		if(TempPtr[0]<'0' || TempPtr[0]>'9') break;
		else TempPtr++;
	}
	
	for(u8 i=0;i<IntNumber;i++) //整数部分
		Number += (*(Ptr++)-'0')*Weight[IntNumber-i];
	
	Number *= Sign; //添加符号
	
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
