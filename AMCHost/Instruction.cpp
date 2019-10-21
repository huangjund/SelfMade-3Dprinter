#include "Instruction.h"
#include "TCONI.h"
#include "SCONI.h"
#include "CCONI.h"
#include "Status.h"
#include "Shell.h"

#define u64 unsigned long long

INSTRUCTION Instruct;

void INSTRUCTION::GetTemExtA(){
	if(Status.LinkToTcon){ //链路畅通
		if(Status.TempExtruderA == 0){ //温度计开路
			Cconi.ConsoleDisplay("\nError:Thermometer Offline.",0);
		}
		else if(Status.TempExtruderA == 655){ //温度计短路
			Cconi.ConsoleDisplay("\nError:Thermometer Short Out.",0);
		}
		else{ //温度正常
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Status.TempExtruderA);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nTemperature:",1);
			if(Status.TempExtruderA > 280){
				Cconi.ConsoleDisplay(Data,1);
				Cconi.ConsoleDisplay("\nWarning:Overheating.",0);
			}
			else
				Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetTemExtAInit(){
	static char* Data[5];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Temperature";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Extruder";
	Shell.Expand(Data[2]);
	
	Data[3] = "A";
	Shell.Expand(Data[3]);
	
	Data[4] = "";
	GetTemExtA_InsSet = Data;
	GetTemExtAOperand = 0;
}

void INSTRUCTION::GetTemExtB(){
	if(Status.LinkToTcon){ //链路畅通
		if(Status.TempExtruderB == 0){ //温度计开路
			Cconi.ConsoleDisplay("\nError:Thermometer Offline.",0);
		}
		else if(Status.TempExtruderB == 655){ //温度计短路
			Cconi.ConsoleDisplay("\nError:Thermometer Short Out.",0);
		}
		else{ //温度正常
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Status.TempExtruderB);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nTemperature:",1);
			if(Status.TempExtruderB > 280){
				Cconi.ConsoleDisplay(Data,1);
				Cconi.ConsoleDisplay("\nWarning:Overheating.",0);
			}
			else
				Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetTemExtBInit(){
	static char* Data[5];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Temperature";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Extruder";
	Shell.Expand(Data[2]);
	
 	Data[3] = "B";
	Shell.Expand(Data[3]);
	
	Data[4] = "";
	GetTemExtB_InsSet = Data;
	GetTemExtBOperand = 0;
}

void INSTRUCTION::GetTemBP(){
	if(Status.LinkToTcon){ //链路畅通
		if(Status.TempBasePlane == 0){ //温度计开路
			Cconi.ConsoleDisplay("\nError:Thermometer Offline.",0);
		}
		else if(Status.TempBasePlane == 655){ //温度计短路
			Cconi.ConsoleDisplay("\nError:Thermometer Short Out.",0);
		}
		else{ //温度正常
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Status.TempBasePlane);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nTemperature:",1);
			if(Status.TempBasePlane > 120){
				Cconi.ConsoleDisplay(Data,1);
				Cconi.ConsoleDisplay("\nWarning:Overheating.",0);
			}
			else
				Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetTemBPInit(){
	static char* Data[4];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Temperature";
	Shell.Expand(Data[1]);
	
 	Data[2] = "BasePlane";
	Shell.Expand(Data[2]);
	
	Data[3] = "";
	GetTemBP_InsSet = Data;
	GetTemBPOperand = 0;
}

void INSTRUCTION::GetTemEnv(){
	if(Status.LinkToTcon){ //链路畅通
		if(Status.TempAir == 0){ //温度计开路
			Cconi.ConsoleDisplay("\nError:Thermometer Offline.",0);
		}
		else if(Status.TempAir == 655){ //温度计短路
			Cconi.ConsoleDisplay("\nError:Thermometer Short Out.",0);
		}
		else{ //温度正常
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Status.TempAir);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nTemperature:",1);
			Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetTemEnvInit(){
	static char* Data[4];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Temperature";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Environment";
	Shell.Expand(Data[2]);
	
	Data[3] = "";
	GetTemEnv_InsSet = Data;
	GetTemEnvOperand = 0;
}

void INSTRUCTION::GetColPowEA(){
	if(Status.LinkToTcon){ //链路畅通
		static char Data[10] = {};
		char *TempStr = Data;
		TempStr = Num2Chr(TempStr,Status.PowerCoolerExtruderA);
		*(TempStr++) = '/';
		*(TempStr++) = '1';
		*(TempStr++) = '0';
		*(TempStr++) = '0';
		*(TempStr++) = '.';
		*(TempStr++) = 0;		
		Cconi.ConsoleDisplay("\nPower:",1);
		Cconi.ConsoleDisplay(Data,0);
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetColPowEAInit(){
	static char* Data[6];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Cooler";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Power";
	Shell.Expand(Data[2]);
	
 	Data[3] = "Extruder";
	Shell.Expand(Data[3]);
	
 	Data[4] = "A";
	Shell.Expand(Data[4]);
	
	Data[5] = "";
	GetColPowEA_InsSet = Data;
	GetColPowEAOperand = 0;
}

void INSTRUCTION::GetColPowEB(){
	if(Status.LinkToTcon){ //链路畅通
		static char Data[10] = {};
		char *TempStr = Data;
		TempStr = Num2Chr(TempStr,Status.PowerCoolerExtruderB);
		*(TempStr++) = '/';
		*(TempStr++) = '1';
		*(TempStr++) = '0';
		*(TempStr++) = '0';
		*(TempStr++) = '.';
		*(TempStr++) = 0;		
		Cconi.ConsoleDisplay("\nPower:",1);
		Cconi.ConsoleDisplay(Data,0);
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetColPowEBInit(){
	static char* Data[6];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Cooler";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Power";
	Shell.Expand(Data[2]);
	
 	Data[3] = "Extruder";
	Shell.Expand(Data[3]);
	
 	Data[4] = "B";
	Shell.Expand(Data[4]);
	
	Data[5] = "";
	GetColPowEB_InsSet = Data;
	GetColPowEBOperand = 0;
}

void INSTRUCTION::GetTemEAS(){
	if(Status.LinkToTcon){ //链路畅通
		static char Data[10] = {};
		char *TempStr = Data;
		TempStr = Num2Chr(TempStr,Status.TempExtruderASet);
		*(TempStr++) = '\'';
		*(TempStr++) = 'C';
		*(TempStr++) = '.';
		*(TempStr++) = 0;		
		Cconi.ConsoleDisplay("\nSetting Temperature:",1);
		Cconi.ConsoleDisplay(Data,0);
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetTemEASInit(){
	static char* Data[6];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Aim";
	Shell.Expand(Data[1]);
	
	Data[2] = "Temperature";
	Shell.Expand(Data[2]);
	
 	Data[3] = "Extruder";
	Shell.Expand(Data[3]);
	
	Data[4] = "A";
	Shell.Expand(Data[4]);
	
	Data[5] = "";
	GetTemEAS_InsSet = Data;
	GetTemEASOperand = 0;
}

void INSTRUCTION::GetTemEBS(){
	if(Status.LinkToTcon){ //链路畅通
		static char Data[10] = {};
		char *TempStr = Data;
		TempStr = Num2Chr(TempStr,Status.TempExtruderBSet);
		*(TempStr++) = '\'';
		*(TempStr++) = 'C';
		*(TempStr++) = '.';
		*(TempStr++) = 0;		
		Cconi.ConsoleDisplay("\nSetting Temperature:",1);
		Cconi.ConsoleDisplay(Data,0);
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetTemEBSInit(){
	static char* Data[6];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Aim";
	Shell.Expand(Data[1]);
	
	Data[2] = "Temperature";
	Shell.Expand(Data[2]);
	
 	Data[3] = "Extruder";
	Shell.Expand(Data[3]);
	
	Data[4] = "B";
	Shell.Expand(Data[4]);
	
	Data[5] = "";
	GetTemEBS_InsSet = Data;
	GetTemEBSOperand = 0;
}

void INSTRUCTION::GetTemBPS(){
	if(Status.LinkToTcon){ //链路畅通
		static char Data[10] = {};
		char *TempStr = Data;
		TempStr = Num2Chr(TempStr,Status.TempBasePlaneSet);
		*(TempStr++) = '\'';
		*(TempStr++) = 'C';
		*(TempStr++) = '.';
		*(TempStr++) = 0;		
		Cconi.ConsoleDisplay("\nSetting Temperature:",1);
		Cconi.ConsoleDisplay(Data,0);
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetTemBPSInit(){
	static char* Data[5];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Aim";
	Shell.Expand(Data[1]);
	
	Data[2] = "Temperature";
	Shell.Expand(Data[2]);
	
 	Data[3] = "BasePlane";
	Shell.Expand(Data[3]);
	
	Data[4] = "";
	GetTemBPS_InsSet = Data;
	GetTemBPSOperand = 0;
}

void INSTRUCTION::SetTemExtA(){
	if(Status.LinkToTcon){ //链路畅通
		u32 Temp = Shell.GetNumber(0);
		if(Status.Manufacturing){ //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		}
		else if(Temp > 280){ //温度太高了
			Cconi.ConsoleDisplay("\nForbidden:Temperature too High.",0);
		}
		else if(Tconi.SetTemperatureEASetting(Temp) != 'D'){ //设置失败
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
		else{ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetTemExtAInit(){
	static char* Data[5];
	Data[0] = "Set";
	Shell.Expand(Data[0]);
	
	Data[1] = "Temperature";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Extruder";
	Shell.Expand(Data[2]);
	
	Data[3] = "A";
	Shell.Expand(Data[3]);
	
	Data[4] = "";
	SetTemExtA_InsSet = Data;
	SetTemExtAOperand = 1;
}

void INSTRUCTION::SetTemExtB(){
	if(Status.LinkToTcon){ //链路畅通
		u32 Temp = Shell.GetNumber(0);
		if(Status.Manufacturing){ //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		}
		else if(Temp > 280){ //温度太高了
			Cconi.ConsoleDisplay("\nForbidden:Temperature too High.",0);
		}
		else if(Tconi.SetTemperatureEBSetting(Temp) != 'D'){ //设置失败
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
		else{ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetTemExtBInit(){
	static char* Data[5];
	Data[0] = "Set";
	Shell.Expand(Data[0]);
	
	Data[1] = "Temperature";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Extruder";
	Shell.Expand(Data[2]);
	
	Data[3] = "B";
	Shell.Expand(Data[3]);
	
	Data[4] = "";
	SetTemExtB_InsSet = Data;
	SetTemExtBOperand = 1;
}

void INSTRUCTION::SetTemBP(){
	if(Status.LinkToTcon){ //链路畅通
		u32 Temp = Shell.GetNumber(0);
		if(Status.Manufacturing){ //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		}
		else if(Temp > 120){ //温度太高了
			Cconi.ConsoleDisplay("\nForbidden:Temperature too High.",0);
		}
		else if(Tconi.SetTemperatureBPSetting(Temp) != 'D'){ //设置失败
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
		else{ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetTemBPInit(){
	static char* Data[4];
	Data[0] = "Set";
	Shell.Expand(Data[0]);
	
	Data[1] = "Temperature";
	Shell.Expand(Data[1]);
	
 	Data[2] = "BasePlane";
	Shell.Expand(Data[2]);
	
	Data[3] = "";
	SetTemBP_InsSet = Data;
	SetTemBPOperand = 1;
}

void INSTRUCTION::SetColExtA(){
	if(Status.LinkToTcon){ //链路畅通
		u32 Temp = Shell.GetNumber(0);
		if(Temp > 100) Temp = 100;		
		if(Status.Manufacturing){ //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		}
		else if(Tconi.SetCoolerPowerExtruderA(Temp) != 'D'){ //设置失败
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
		else{ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '/';
			*(TempStr++) = '1';
			*(TempStr++) = '0';
			*(TempStr++) = '0';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetColExtAInit(){
	static char* Data[6];
	Data[0] = "Set";
	Shell.Expand(Data[0]);
	
	Data[1] = "Cooler";
	Shell.Expand(Data[1]);
	
	Data[2] = "Power";
	Shell.Expand(Data[2]);
	
 	Data[3] = "Extruder";
	Shell.Expand(Data[3]);
	
	Data[4] = "A";
	Shell.Expand(Data[4]);
	
	Data[5] = "";
	SetColExtA_InsSet = Data;
	SetColExtAOperand = 1;
}

void INSTRUCTION::SetColExtB(){
	if(Status.LinkToTcon){ //链路畅通
		u32 Temp = Shell.GetNumber(0);
		if(Temp > 100) Temp = 100;		
		if(Status.Manufacturing){ //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		}
		else if(Tconi.SetCoolerPowerExtruderB(Temp) != 'D'){ //设置失败
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
		else{ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '/';
			*(TempStr++) = '1';
			*(TempStr++) = '0';
			*(TempStr++) = '0';
			*(TempStr++) = '.';
			*(TempStr++) = 0;		
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetColExtBInit(){
	static char* Data[6];
	Data[0] = "Set";
	Shell.Expand(Data[0]);
	
	Data[1] = "Cooler";
	Shell.Expand(Data[1]);
	
	Data[2] = "Power";
	Shell.Expand(Data[2]);
	
 	Data[3] = "Extruder";
	Shell.Expand(Data[3]);
	
	Data[4] = "B";
	Shell.Expand(Data[4]);
	
	Data[5] = "";
	SetColExtB_InsSet = Data;
	SetColExtBOperand = 1;
}

void INSTRUCTION::GetStaExtA(){
	if(Status.LinkToTcon){ //链路畅通
		if(Status.StateExtruderA == 'H'){ //加热状态
			Cconi.ConsoleDisplay("\nStatus:Heating.",0);
		}
		else if(Status.StateExtruderA == 'C'){ //冷却状态
			Cconi.ConsoleDisplay("\nStatus:Cooling.",0);
		}
		else if(Status.StateExtruderA == 'M'){ //保持状态
			Cconi.ConsoleDisplay("\nStatus:Holding.",0);
		}
		else{ //温度正常
			Cconi.ConsoleDisplay("\nStatus:Power Down.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetStaExtAInit(){
	static char* Data[5];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Status";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Extruder";
	Shell.Expand(Data[2]);
	
	Data[3] = "A";
	Shell.Expand(Data[3]);
	
	Data[4] = "";
	GetStaExtA_InsSet = Data;
	GetStaExtAOperand = 0;
}

void INSTRUCTION::GetStaExtB(){
	if(Status.LinkToTcon){ //链路畅通
		if(Status.StateExtruderB == 'H'){ //加热状态
			Cconi.ConsoleDisplay("\nStatus:Heating.",0);
		}
		else if(Status.StateExtruderB == 'C'){ //冷却状态
			Cconi.ConsoleDisplay("\nStatus:Cooling.",0);
		}
		else if(Status.StateExtruderB == 'M'){ //保持状态
			Cconi.ConsoleDisplay("\nStatus:Holding.",0);
		}
		else{ //温度正常
			Cconi.ConsoleDisplay("\nStatus:Power Down.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetStaExtBInit(){
	static char* Data[5];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Status";
	Shell.Expand(Data[1]);
	
 	Data[2] = "Extruder";
	Shell.Expand(Data[2]);
	
	Data[3] = "A";
	Shell.Expand(Data[3]);
	
	Data[4] = "";
	GetStaExtB_InsSet = Data;
	GetStaExtBOperand = 0;
}

void INSTRUCTION::GetStaBP(){
	if(Status.LinkToTcon){ //链路畅通
		if(Status.StateBasePlane == 'H'){ //加热状态
			Cconi.ConsoleDisplay("\nStatus:Heating.",0);
		}
		else if(Status.StateBasePlane == 'C'){ //冷却状态
			Cconi.ConsoleDisplay("\nStatus:Cooling.",0);
		}
		else if(Status.StateBasePlane == 'M'){ //保持状态
			Cconi.ConsoleDisplay("\nStatus:Holding.",0);
		}
		else{ //温度正常
			Cconi.ConsoleDisplay("\nStatus:Power Down.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::GetStaBPInit(){
	static char* Data[4];
	Data[0] = "Get";
	Shell.Expand(Data[0]);
	
	Data[1] = "Status";
	Shell.Expand(Data[1]);
	
 	Data[2] = "BasePlane";
	Shell.Expand(Data[2]);
	
	Data[3] = "";
	Shell.Expand(Data[3]);

	GetStaBP_InsSet = Data;
	GetStaBPOperand = 0;
}

void INSTRUCTION::AutoHome(){
	if(Status.LinkToScon){ //链路畅通
		if(Sconi.AutoHoming() == 'M'){ //发送复位命令成功
			Cconi.ConsoleDisplay("\nOperation Now Executing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			char HomingStatus;
			for(u32 i=0;i<180;i++){ //等待复位完成最多180s
				HomingStatus = Sconi.WaitForHoming(1000);
				if(HomingStatus == 'T') //复位未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(HomingStatus == 'D'){ //复位成功
				bool FlagTestPoint = 1;
				char LevelingState = 0;
				if(Sconi.GetTestPointA() != 'D') FlagTestPoint = 0;
				if(Sconi.GetTestPointB() != 'D') FlagTestPoint = 0;
				if(Sconi.GetTestPointC() != 'D') FlagTestPoint = 0;
				if(Sconi.GetTestPointD() != 'D') FlagTestPoint = 0;
				if(Sconi.GetTestPointM() != 'D') FlagTestPoint = 0;
				LevelingState = Sconi.GetStatusLeveling();
				if(LevelingState!='D'&& LevelingState!='E') FlagTestPoint = 0;
				if(FlagTestPoint == 1){
					if(LevelingState == 'E')
						Cconi.ConsoleDisplay("\nLeveling Done.",1);
					else
						Cconi.ConsoleDisplay("\nLeveling Failed.",1);
					
					Cconi.ConsoleDisplay("\nTestpointA: ",1);
					PrintNum(Status.TestPosition[0]*1000);
					Cconi.ConsoleDisplay("um.",1);
					
					Cconi.ConsoleDisplay("\nTestpointB: ",1);
					PrintNum(Status.TestPosition[1]*1000);
					Cconi.ConsoleDisplay("um.",1);

					Cconi.ConsoleDisplay("\nTestpointC: ",1);
					PrintNum(Status.TestPosition[2]*1000);
					Cconi.ConsoleDisplay("um.",1);
					
					Cconi.ConsoleDisplay("\nTestpointD: ",1);
					PrintNum(Status.TestPosition[3]*1000);
					Cconi.ConsoleDisplay("um.",1);
					
					Cconi.ConsoleDisplay("\nTestpointM: ",1);
					PrintNum(Status.TestPosition[4]*1000);
					Cconi.ConsoleDisplay("um.",1);
					Cconi.ConsoleDisplay("\nOperation Done.",0);					
				}
				else{
					Cconi.ConsoleDisplay("\nLeveling Request Timeout",1);					
					Cconi.ConsoleDisplay("\nOperation Done.",0);
				}
			}
			else //复位失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}
		else{//发送复位命令不成功
		Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::AutoHomeInit(){
	static char* Data[2];
	Data[0] = "AutoHoming";
	Shell.Expand(Data[0]);
	
	Data[1] = "";
	
	AutoHome_InsSet = Data;
	AutoHomeOperand = 0;
}

void INSTRUCTION::RefreshDict(){
	if(Status.LinkToCcon){ //链路畅通
		Shell.DictInit();
		Cconi.ConsoleDisplay("\nOperation Done.",0);
	}
}
void INSTRUCTION::RefreshDictInit(){
	static char* Data[3];
	Data[0] = "Refresh";
	Shell.Expand(Data[0]);
	
	Data[1] = "Dictionary";
	Shell.Expand(Data[1]);
	
	Data[2] = "";
	RefreshDict_InsSet = Data;
	RefreshDictOperand = 0;
}





void INSTRUCTION::NotFound(){
	Cconi.ConsoleDisplay("\nError:Command Not Found.",0);
}

void INSTRUCTION::PrintNum(u32 Num){
	static char Temp[80];
	char *Ptr = Temp;
	
	Ptr = Num2Chr(Ptr,Num);
	*Ptr = 0;
	
	Cconi.ConsoleDisplay(Temp,1);
}

char* INSTRUCTION::Num2Chr(char *Data,u32 Num){
	static u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};
	
	if(!Num){
		*(Data++) = '0';
		return Data;
	}		
	
	u32 Length;
	for(Length=8;Length>0;Length--)
		if(Num/Weight[Length]) break;


	for(u32 i=Length;i>0;i--) //整数部分
		*(Data++) = Num%Weight[i+1]/Weight[i] + '0';
	
	return Data;
}
