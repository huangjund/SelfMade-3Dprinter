#include "Instruction.h"
#include "Interpreter.h"
#include "TCONI.h"
#include "SCONI.h"
#include "CCONI.h"
#include "FCONI.h"
#include "AuxIO.h"
#include "Status.h"
#include "Shell.h"
#include "Automate.h"

#define u64 unsigned long long

INSTRUCTION Instruct;

void INSTRUCTION::SetTemExtA(u32 Temp){
	if(Status.LinkToTcon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Temp > 300) //温度太高了
			Cconi.ConsoleDisplay("\nForbidden:Temperature too High.",0);
		else //设置
			Recv = Tconi.SetTemperatureEASetting(Temp);

		if(Recv == 'D'){ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}else if(Recv == 'H'){ //温度太高
			Cconi.ConsoleDisplay("\nWarning:Temperature too High.",1);
			Cconi.ConsoleDisplay("\nSetting Done:280\037.",0);
		}else if(Recv == 'L'){ //温度太低
			Cconi.ConsoleDisplay("\nWarning:Temperature too Low.",1);
			Cconi.ConsoleDisplay("\nHeater Off.",0);	
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
		
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetTemExtB(u32 Temp){
	if(Status.LinkToTcon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Temp > 300) //温度太高了
			Cconi.ConsoleDisplay("\nForbidden:Temperature too High.",0);
		else //设置
			Recv = Tconi.SetTemperatureEBSetting(Temp);
		
		if(Recv == 'D'){ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}else if(Recv == 'H'){ //温度太高
			Cconi.ConsoleDisplay("\nWarning:Temperature too High.",1);
			Cconi.ConsoleDisplay("\nSetting Done:280\037.",0);
		}else if(Recv == 'L'){ //温度太低
			Cconi.ConsoleDisplay("\nWarning:Temperature too Low.",1);
			Cconi.ConsoleDisplay("\nHeater Off.",0);	
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetTemBP(u32 Temp){
	if(Status.LinkToTcon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Temp > 125) //温度太高了
			Cconi.ConsoleDisplay("\nForbidden:Temperature too High.",0);
		else //设置
			Recv = Tconi.SetTemperatureBPSetting(Temp);
		
		if(Recv == 'D'){ //设置成功
			static char Data[10] = {};
			char *TempStr = Data;
			TempStr = Num2Chr(TempStr,Temp);
			*(TempStr++) = '\'';
			*(TempStr++) = 'C';
			*(TempStr++) = '.';
			*(TempStr++) = 0;
			Cconi.ConsoleDisplay("\nSetting Done:",1);
			Cconi.ConsoleDisplay(Data,0);
		}else if(Recv == 'H'){ //温度太高
			Cconi.ConsoleDisplay("\nWarning:Temperature too High.",1);
			Cconi.ConsoleDisplay("\nSetting Done:120\037.",0);
		}else if(Recv == 'L'){ //温度太低
			Cconi.ConsoleDisplay("\nWarning:Temperature too Low.",1);
			Cconi.ConsoleDisplay("\nHeater Off.",0);	
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetColExtA(u32 Temp){
	if(Status.LinkToTcon){ //链路畅通
		u8 Recv = 'D';
		if(Temp > 100) Temp = 100;			
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else //设置
			Recv = Tconi.SetCoolerPowerExtruderA(Temp);

		if(Recv == 'D'){ //设置成功
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
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::SetColExtB(u32 Temp){
	if(Status.LinkToTcon){ //链路畅通
		u8 Recv = 'D';
		if(Temp > 100) Temp = 100;			
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else //设置
			Recv = Tconi.SetCoolerPowerExtruderB(Temp);
		
		if(Recv == 'D'){ //设置成功
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
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::AutoHome(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';		
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else //发送复位命令
			Recv = Sconi.AutoHoming();
		
		if(Recv == 'M'){ //成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool HomingStatus;
			for(u32 i=0; i<170; i++){ //等待复位完成最多170s
				for(u32 cnt=0; cnt<1000; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				HomingStatus = Sconi.GetStatusBusy();
				
				Automate.Checking(0xF7);
				if(HomingStatus) //复位未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!HomingStatus){ //复位成功
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
					if(Status.StateLeveling == 'E')
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
					Cconi.ConsoleDisplay("\nOperation Failed.",0);
				}
			}
			else{
				Cconi.ConsoleDisplay("\nError:System Fault.",0);
			}
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}

void INSTRUCTION::EnableNozzleA(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Status.StateFA == 'D') //失能
			Cconi.ConsoleDisplay("\nForbidden:Drives Disabled.",0);
		else if(Status.StateFA == 'N') //未复位
			Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
		else if(Status.StateFA == 'W') //其他任务占用
			Cconi.ConsoleDisplay("\nForbidden:In Processing.",0);
		else //发送移动命令
			Recv = Sconi.SetNozzleState(0x01,0xFF);
		
		if(Recv == 'M'){ //成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool SwitchStatus;
			for(u32 i=0;i<5;i++){ //等待最多2.5s
				SwitchStatus = Sconi.GetStatusBusy();
				
				for(u32 cnt=0; cnt<500; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				Automate.Checking(0xF7);
				if(SwitchStatus) //设置未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!SwitchStatus){ //设置成功
				Cconi.ConsoleDisplay("\nOperation Done.",0);
			}
			else //设置失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'H'){
			Cconi.ConsoleDisplay("\nError:Not Calibrated.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}

void INSTRUCTION::DisableNozzleA(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Status.StateFA == 'D') //失能
			Cconi.ConsoleDisplay("\nForbidden:Drives Disabled.",0);
		else if(Status.StateFA == 'N') //未复位
			Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
		else if(Status.StateFA == 'W') //其他任务占用
			Cconi.ConsoleDisplay("\nForbidden:In Processing.",0);
		else //发送移动命令
			Recv = Sconi.SetNozzleState(0x00,0xFF);
		
		if(Recv == 'M'){ //成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool SwitchStatus;
			for(u32 i=0;i<5;i++){ //等待最多2.5s
				SwitchStatus = Sconi.GetStatusBusy();
				
				for(u32 cnt=0; cnt<500; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				Automate.Checking(0xF7);
				if(SwitchStatus) //设置未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!SwitchStatus){ //设置成功
				Cconi.ConsoleDisplay("\nOperation Done.",0);
			}
			else //设置失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'H'){
			Cconi.ConsoleDisplay("\nError:Not Calibrated.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::EnableNozzleB(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Status.StateFA == 'D') //失能
			Cconi.ConsoleDisplay("\nForbidden:Drives Disabled.",0);
		else if(Status.StateFA == 'N') //未复位
			Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
		else if(Status.StateFA == 'W') //其他任务占用
			Cconi.ConsoleDisplay("\nForbidden:In Processing.",0);
		else //发送移动命令
			Recv = Sconi.SetNozzleState(0xFF,0x01);
		
		if(Recv == 'M'){ //成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool SwitchStatus;
			for(u32 i=0;i<5;i++){ //等待最多2.5s
				SwitchStatus = Sconi.GetStatusBusy();
				
				for(u32 cnt=0; cnt<500; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				Automate.Checking(0xF7);
				if(SwitchStatus) //设置未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!SwitchStatus){ //设置成功
				Cconi.ConsoleDisplay("\nOperation Done.",0);
			}
			else //设置失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'H'){
			Cconi.ConsoleDisplay("\nError:Not Calibrated.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::DisableNozzleB(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Status.StateFA == 'D') //失能
			Cconi.ConsoleDisplay("\nForbidden:Drives Disabled.",0);
		else if(Status.StateFA == 'N') //未复位
			Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
		else if(Status.StateFA == 'W') //其他任务占用
			Cconi.ConsoleDisplay("\nForbidden:In Processing.",0);
		else //发送移动命令
			Recv = Sconi.SetNozzleState(0xFF,0x00);
		
		if(Recv == 'M'){ //成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool SwitchStatus;
			for(u32 i=0;i<5;i++){ //等待最多2.5s
				SwitchStatus = Sconi.GetStatusBusy();
				
				for(u32 cnt=0; cnt<500; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				Automate.Checking(0xF7);
				if(SwitchStatus) //设置未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!SwitchStatus){ //设置成功
				Cconi.ConsoleDisplay("\nOperation Done.",0);
			}
			else //设置失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'H'){
			Cconi.ConsoleDisplay("\nError:Not Calibrated.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}

void INSTRUCTION::MoveAxisX(u32 Temp){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Status.StateFA == 'D') //失能
			Cconi.ConsoleDisplay("\nForbidden:Drives Disabled.",0);
		else if(Status.StateFA == 'N') //未复位
			Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
		else if(Status.StateFA == 'W') //其他任务占用
			Cconi.ConsoleDisplay("\nForbidden:In Processing.",0);
		else //发送移动命令
			Recv = Sconi.MoveAxis(Temp,0);
		
		if(Recv == 'M'){ //成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool MovingStatus;
			for(u32 i=0;i<20;i++){ //等待移动完成最多20s
				MovingStatus = Sconi.GetStatusBusy();
				
				for(u32 cnt=0; cnt<1000; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				Automate.Checking(0xF7);
				if(MovingStatus) //移动未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!MovingStatus){ //移动成功
				Cconi.ConsoleDisplay("\nOperation Done.",0);
			}
			else //移动失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'H'){
			Cconi.ConsoleDisplay("\nError:Not Calibrated.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::MoveAxisY(u32 Temp){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Status.StateFA == 'D') //失能
			Cconi.ConsoleDisplay("\nForbidden:Drives Disabled.",0);
		else if(Status.StateFA == 'N') //未复位
			Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
		else if(Status.StateFA == 'W') //其他任务占用
			Cconi.ConsoleDisplay("\nForbidden:In Processing.",0);
		else //发送移动命令
			Recv = Sconi.MoveAxis(Temp,1);
		
		if(Recv == 'M'){ //发送移动命令成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool MovingStatus;
			for(u32 i=0;i<20;i++){ //等待移动完成最多20s
				MovingStatus = Sconi.GetStatusBusy();
				
				for(u32 cnt=0; cnt<1000; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				Automate.Checking(0xF7);
				if(MovingStatus) //移动未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!MovingStatus){ //移动成功
				Cconi.ConsoleDisplay("\nOperation Done.",0);
			}
			else //移动失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'H'){
			Cconi.ConsoleDisplay("\nError:Not Calibrated.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::MoveAxisZ(u32 Temp){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else if(Status.StateFA == 'D') //失能
			Cconi.ConsoleDisplay("\nForbidden:Drives Disabled.",0);
		else if(Status.StateFA == 'N') //未复位
			Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
		else if(Status.StateFA == 'W') //其他任务占用
			Cconi.ConsoleDisplay("\nForbidden:In Processing.",0);
		else //发送移动命令
			Recv = Sconi.MoveAxis(Temp,2);
		
		if(Recv == 'M'){ //发送移动命令成功
			Cconi.ConsoleDisplay("\nOperation Now Processing.",1);
			Cconi.ConsoleDisplay("\nPlease Wait...",1);
			bool MovingStatus;
			for(u32 i=0;i<40;i++){ //等待移动完成最多40s
				MovingStatus = Sconi.GetStatusBusy();
				
				for(u32 cnt=0; cnt<1000; cnt++){
        	SysTick->LOAD = 72000;
        	SysTick->CTRL = 0x00000005;
        	while(!(SysTick->CTRL & 0x00010000));
        	SysTick->CTRL = 0x00000004;					
				}
				
				Automate.Checking(0xF7);
				if(MovingStatus) //移动未完成
					Cconi.HangUpConsole(); //挂起控制台
				else
					break; 
			}
			if(!MovingStatus){ //移动成功
				Cconi.ConsoleDisplay("\nOperation Done.",0);
			}
			else //移动失败
				Cconi.ConsoleDisplay("\nError:System Fault.",0);			
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'H'){
			Cconi.ConsoleDisplay("\nError:Not Calibrated.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::EnableStepper(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		if(Interpreter.RoundStatus != 'I') //正在制造
			Cconi.ConsoleDisplay("\nForbidden:In Manufacturing.",0);
		else
			Recv = Sconi.EnableStepper();
		
		if(Recv == 'D'){ //设置成功
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'K'){
			Cconi.ConsoleDisplay("\nError:Subsystem Busy.",0);	
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::EnableIDP(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		Recv = Sconi.EnableIDP();
		
		if(Recv == 'D'){ //设置成功
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::DisableIDP(){
	if(Status.LinkToScon){ //链路畅通
		u8 Recv = 'D';
		Recv = Sconi.DisableIDP();
		
		if(Recv == 'D'){ //设置成功
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}else if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{
			Cconi.ConsoleDisplay("\nError:System Fault.",0);
		}
	}
	else{ //链路故障
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::RefreshDict(){
	if(Status.LinkToCcon){ //链路畅通
		Shell.DictInit();
		Cconi.ConsoleDisplay("\nOperation Done.",0);
	}
}

void INSTRUCTION::EnableMainLight(){
	if(AuxIO.TestLink()){
		u8 Recv = 'D';
		Recv = AuxIO.SetStatusMainLight(1);
		
		if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{ //设置成功
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}
	}
	else{
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}

void INSTRUCTION::DisableMainLight(){
	if(AuxIO.TestLink()){
		u8 Recv = 'D';
		Recv = AuxIO.SetStatusMainLight(0);
		
		if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{ //设置成功
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}
	}
	else{
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}
void INSTRUCTION::EnableAuxiliaryLight(){
	if(AuxIO.TestLink()){
		u8 Recv = 'D';
		Recv = AuxIO.SetStatusAuxiliaryLight(1);
		
		if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{ //设置成功
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}
	}
	else{
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}

void INSTRUCTION::DisableAuxiliaryLight(){
	if(AuxIO.TestLink()){
		u8 Recv = 'D';
		Recv = AuxIO.SetStatusAuxiliaryLight(0);
		
		if(Recv == 'R'){
			Cconi.ConsoleDisplay("\nError:Verification Failed.",0);
		}else if(Recv == 'N'){
			Cconi.ConsoleDisplay("\nError:Instruction Failed.",0);			
		}else if(Recv == 'T'){
			Cconi.ConsoleDisplay("\nError:Link Timeout.",0);
		}else{ //设置成功
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}
	}
	else{
		Cconi.ConsoleDisplay("\nError:Request Timeout.",0);
	}
}


void INSTRUCTION::StorageInfo(){
	if(Status.FileExist){
		Cconi.ConsoleDisplay("\nName:",1);
		Cconi.ConsoleDisplay(Status.FileName,1);
		Cconi.ConsoleDisplay("\nSize :",1);
		PrintNum(Status.FileSize);
		Cconi.ConsoleDisplay("[B]",1);
		Cconi.ConsoleDisplay("\nCount:",1);
		PrintNum(Status.CommandNumber);
		Cconi.ConsoleDisplay("\nE.T.C:",1);
		PrintNum(Status.EstimateTime);		
		Cconi.ConsoleDisplay("[s]",1);
		Cconi.ConsoleDisplay("\nDistS:",1);
		PrintNum(Status.TotalDistance);		
		Cconi.ConsoleDisplay("[cm]",1);
		Cconi.ConsoleDisplay("\nDistA:",1);
		PrintNum(Status.FileInfo1);		
		Cconi.ConsoleDisplay("[cm]",1);
		Cconi.ConsoleDisplay("\nDistB:",1);
		PrintNum(Status.FileInfo2);		
		Cconi.ConsoleDisplay("[cm]",1);
		Cconi.ConsoleDisplay("\nStorage Capacity:",1);
		PrintNum((Status.CardRestVolume+4096)/1048576);		
		Cconi.ConsoleDisplay("/",1);
		PrintNum(Status.CardTotalVolume);		
		Cconi.ConsoleDisplay("[MB]",0);
	}
	else if(Status.CardTotalVolume){
		Cconi.ConsoleDisplay("\nWarning:File Not Found.",1);
		Cconi.ConsoleDisplay("\nStorage Capacity:",1);
		PrintNum((Status.CardRestVolume+4096)/1048576);		
		Cconi.ConsoleDisplay("/",1);
		PrintNum(Status.CardTotalVolume);		
		Cconi.ConsoleDisplay("[MB]",0);
	}
	else{
		Cconi.ConsoleDisplay("\nWarning:Storage Device Not Found.",0);
	}
}

void INSTRUCTION::StartManufacturing(){
	char Recv = Interpreter.Start();
	if(Recv == 'D'){
		Cconi.ConsoleDisplay("\nOperation Done.",0);
	}
	else if(Recv == 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Busy.",0);
	}
	else if(Recv == '1'){
		Cconi.ConsoleDisplay("\nError:Link Fault.",1);
		Cconi.ConsoleDisplay("\nInfo:Link To Motion Controller.",0);
	}
	else if(Recv == '2'){
		Cconi.ConsoleDisplay("\nError:Link Fault.",1);
		Cconi.ConsoleDisplay("\nInfo:Link To Heat Controller.",0);
	}
	else if(Recv == '3'){
		Cconi.ConsoleDisplay("\nError:Link Fault.",1);
		Cconi.ConsoleDisplay("\nInfo:Link To File Controller.",0);
	}
	else if(Recv == '4'){
		Cconi.ConsoleDisplay("\nError:Link Fault.",1);
		Cconi.ConsoleDisplay("\nInfo:Link To Auxiliary Controller.",0);
	}
	else if(Recv == 'H'){
		Cconi.ConsoleDisplay("\nForbidden:Not Calibrated.",0);
	}
	else if(Recv == 'F'){
		Cconi.ConsoleDisplay("\nForbidden:File Not Found.",0);
	}
	else{
		Cconi.ConsoleDisplay("\nError:System Fault.",0);
	}
}

void INSTRUCTION::StopManufacturing(){
	char Recv = Interpreter.Stop();
	if(Recv == 'D'){
		Cconi.ConsoleDisplay("\nOperation Done.",0);
	}
	else if(Recv == 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Idle.",0);		
	}
	else{
		Cconi.ConsoleDisplay("\nError:System Fault.",0);
	}
}

void INSTRUCTION::PauseManufacturing(){
	char Recv = Interpreter.Pause();
	if(Recv == 'D'){
		Cconi.ConsoleDisplay("\nOperation Done.",0);
	}
	else if(Recv == 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Idle.",0);		
	}
	else{
		Cconi.ConsoleDisplay("\nError:System Fault.",0);
	}
}

void INSTRUCTION::ResumeManufacturing(){
	char Recv = Interpreter.Resume();
	if(Recv == 'D'){
		if(Interpreter.Info.Error[0] == 0xFF){
			Cconi.ConsoleDisplay("\nOperation Done.",0);
		}
		else{
			Cconi.ConsoleDisplay("\nOperation Done.",1);
			Cconi.ConsoleDisplay("\nWarning:Error Message.",0);
		}
	}
	else if(Recv == 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Idle.",0);		
	}
	else if(Recv == 'K'){
		Cconi.ConsoleDisplay("\nForbidden:System Busy.",0);		
	}
	else{
		Cconi.ConsoleDisplay("\nError:System Fault.",0);
	}
}

void INSTRUCTION::ReloadFilament(){
	char Recv = Interpreter.ReLoad();
	if(Recv == 'D'){
		Cconi.ConsoleDisplay("\nOperation Done.",0);
	}
	else if(Recv == 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Idle.",0);		
	}
	else{
		Cconi.ConsoleDisplay("\nError:System Fault.",0);
	}
}
void INSTRUCTION::WarningInfo(){
	if(Interpreter.RoundStatus == 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Idle.",0);
	}
	else{
		u8 WarningCount = 0;
		for(u8 i=0;i<10;i++)
			if(Interpreter.Info.Warning[i][0] != 0xFF) WarningCount++;
		Cconi.ConsoleDisplay("\nInfo: ",1);
		PrintNum(WarningCount);
		Cconi.ConsoleDisplay(" Warning(s).",1);
		for(u8 i=0;i<WarningCount;i++){
			u8 Index = (Interpreter.Info.WarningLoad+9-i)%10;
			if(Interpreter.Info.Warning[Index][0] == 0){
				switch(Interpreter.Info.Warning[Index][1]){
					case 'E':
						Cconi.ConsoleDisplay("\nWarning(",1);
						PrintNum(i);
						Cconi.ConsoleDisplay("): Invalid Instruction(0).",1);
						break;
				}
			}
			else if(Interpreter.Info.Warning[Index][0] == 1){
				switch(Interpreter.Info.Warning[Index][1]){
					case 'E':
						Cconi.ConsoleDisplay("\nWarning(",1);
						PrintNum(i);
						Cconi.ConsoleDisplay("): Empty Buffer(1).",1);
						break;
				}
			}
			else if(Interpreter.Info.Warning[Index][0] == 2){
				switch(Interpreter.Info.Warning[Index][1]){
					case 'E':
						Cconi.ConsoleDisplay("\nWarning(",1);
						PrintNum(i);
						Cconi.ConsoleDisplay("): Empty Buffer(2).",1);
						break;
					case '1':
						Cconi.ConsoleDisplay("\nWarning(",1);
						PrintNum(i);
						Cconi.ConsoleDisplay("): Temperature Too Low(2).",1);
						break;
					case '2':
						Cconi.ConsoleDisplay("\nWarning(",1);
						PrintNum(i);
						Cconi.ConsoleDisplay("): Temperature Too High(2).",1);
						break;
				}
			}
		}
		Cconi.ConsoleDisplay(" ",0);
	}
	for(u8 i=0;i<10;i++){
		Interpreter.Info.Warning[i][0] = 0xFF;
		Interpreter.Info.Warning[i][1] = 0;	
	}
}

void INSTRUCTION::ErrorInfo(){
	if(Interpreter.RoundStatus == 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Idle.",0);
	}
	else{
		if(Interpreter.Info.Error[0] == 0xFF){
			Cconi.ConsoleDisplay("\nInfo:No Errors.",1);
		}
		else if(Interpreter.Info.Error[0] == 0){
			char Temp[64] = {0};
			for(u8 i=0;i<63;i++){
				Temp[i] = Interpreter.TempData[i];
				if(!Temp[i]) break;
			}
			switch(Interpreter.Info.Error[1]){
				case 'R':
					Cconi.ConsoleDisplay("\nIns:",1);
					Cconi.ConsoleDisplay(Temp,1);
					Cconi.ConsoleDisplay("\nInfo:File Verification Failed(0).",1);
					break;
			}
		}
		else if(Interpreter.Info.Error[0] == 1){
			switch(Interpreter.Info.Error[1]){
				case 'F':
					Cconi.ConsoleDisplay("\nInfo:File Not Found(1).",1);
					break;
				case 'H':
					Cconi.ConsoleDisplay("\nInfo:Not Calibrated(1).",1);
					break;
				case 'A':
					Cconi.ConsoleDisplay("\nInfo:FilamentA Insufficient(1).",1);
					break;
				case 'B':
					Cconi.ConsoleDisplay("\nInfo:FilamentB Insufficient(1).",1);
					break;	
			}
		}
		else if(Interpreter.Info.Error[0] == 2){
			switch(Interpreter.Info.Error[1]){
				case 'H':
					Cconi.ConsoleDisplay("\nInsCode:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Type);
					Cconi.ConsoleDisplay("\nInsSegment:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Segment);
					Cconi.ConsoleDisplay("\nInfo:Not Calibrated(2).",1);
					break;
				case 'R':
					Cconi.ConsoleDisplay("\nInsCode:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Type);
					Cconi.ConsoleDisplay("\nInsSegment:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Segment);
					Cconi.ConsoleDisplay("\nInfo:Verification Failed(2).",1);
					break;
				case 'N':
					Cconi.ConsoleDisplay("\nInsCode:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Type);
					Cconi.ConsoleDisplay("\nInsSegment:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Segment);
					Cconi.ConsoleDisplay("\nInfo:Instruction Failed(2).",1);
					break;
				case 'T':
					Cconi.ConsoleDisplay("\nInsCode:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Type);
					Cconi.ConsoleDisplay("\nInsSegment:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Segment);
					Cconi.ConsoleDisplay("\nInfo:Link Timeout(2).",1);
					break;	
				case 'X':
					Cconi.ConsoleDisplay("\nInsCode:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Type);
					Cconi.ConsoleDisplay("\nInsSegment:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Segment);
					Cconi.ConsoleDisplay("\nInfo:Thermometer Fault(2).",1);
					break;
				case 'U':
					Cconi.ConsoleDisplay("\nInsCode:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Type);
					Cconi.ConsoleDisplay("\nInsSegment:",1);
					PrintNum(Interpreter.Buffer[Interpreter.LaunchIndex].Segment);
					Cconi.ConsoleDisplay("\nInfo:System Faule(2).",1);
					break;					
			}
		}
		Cconi.ConsoleDisplay(" ",0);
	}
	Interpreter.Info.Error[0] = 0xFF;
	Interpreter.Info.Error[1] = 0;
}

void INSTRUCTION::ManufacturingInfo(){
	switch(Interpreter.RoundStatus){
		case 'I':Cconi.ConsoleDisplay("\nSystem Status:Idle.",1);break;
		case 'B':Cconi.ConsoleDisplay("\nSystem Status:Paused.",1);break;
		case 'R':Cconi.ConsoleDisplay("\nSystem Status:Reloading.",1);break;
		default :Cconi.ConsoleDisplay("\nSystem Status:Working.",1);break;
	}
	
	u8 WarningCount = 0;
	for(u8 i=0;i<10;i++)
		if(Interpreter.Info.Warning[i][0] != 0xFF) WarningCount++;
	Cconi.ConsoleDisplay("\nWarning(s): ",1);
	PrintNum(WarningCount);
	
	if(Interpreter.Info.Error[0] == 0xFF){
		Cconi.ConsoleDisplay("\nError(s): 0",0);
	}
	else{
		Cconi.ConsoleDisplay("\nError(s): 1",0);
	}
}

void INSTRUCTION::ManufacturingReport(){
	if(Interpreter.RoundStatus != 'I'){
		Cconi.ConsoleDisplay("\nForbidden:System Busy.",0);
	}
	else if(!Interpreter.Report.Used){
		Cconi.ConsoleDisplay("\nInfo:No Report.",0);
	}
	else{
		Cconi.ConsoleDisplay("\nTime Consumption: \n",1);
		PrintNum(Interpreter.Report.HMS[0]);
		Cconi.ConsoleDisplay("Hour(s) ",1);
		PrintNum(Interpreter.Report.HMS[1]);
		Cconi.ConsoleDisplay("Minute(s)",1);
		Cconi.ConsoleDisplay("\nInstruction Count Value: \n",1);
		PrintNum(Interpreter.Report.InsCounter);
		Cconi.ConsoleDisplay(" ",0);
	}
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
