#include "Automate.h"
#include "SCONI.h"
#include "TCONI.h"
#include "FCONI.h"
#include "CCONI.h"
#include "AuxIO.h"
#include "Shell.h"
#include "RTClock.h"
#include "Status.h"
#include "Interpreter.h"

#define INFOLCDREFREASH 100 //每1s刷新一次状态显示屏
#define TCONGETSTATUS 75 //每0.75s获取一次热控制模块状态
#define SCONGETSTATUS 75 //每0.75s获取一次步进控制模块状态
#define FCONGETSTATUS 75 //每0.75s获取一次文件控制模块状态
#define CONSOLEGETCOMMAND 10 //每0.1s检测一次控制台状态
#define AUX1GETSTATUS 75 //每0.75s获取一次辅助模块1状态
#define RTCGETSTATUS 50 //每0.5s获取一次实时时钟状态
#define INTRPROUND 2//每0.02s刷新一次解释器状态

AUTOMATE Automate;

extern"C"void TIM3_IRQHandler(void){
	if(TIM3->SR&0x01){
		Automate.ClickCount();
		Automate.InfoLCDRefreashCount();
		Automate.TCONGetStatusCount();
		Automate.SCONGetStatusCount();
		Automate.FCONGetStatusCount();
		Automate.ConsoleGetCommandCount();
		Automate.AUX1GetStatusCount();
		Automate.RTCGetStatusCount();
		Automate.InterpreterRoundCount();
		TIM3->SR&=~0x01;
	}
}



void AUTOMATE::Init(){
	RCC->APB1ENR|=0x01<<1;
  TIM3->ARR=10000;//预装载 10ms
  TIM3->PSC=71;//分频
  TIM3->CR1|=0x01<<4;
  TIM3->DIER|=0x01<<0;
  NVIC->IP[29]=0x00;//优先级0
  NVIC->ISER[0]|=0x01<<29;
  TIM3->CR1|=0x01<<0;//使能

}

void AUTOMATE::Checking(u8 Select){
	if(FlagInfoLCDRefreash){
		FlagInfoLCDRefreash = 0;
		if(Select&0x01) InfoLCDRefreash();
	}
	if(FlagTCONGetStatus){
		FlagTCONGetStatus = 0;
		if(Select&0x02) TCONGetStatus();
	}
	if(FlagSCONGetStatus){
		FlagSCONGetStatus = 0;
		if(Select&0x04) SCONGetStatus();
	}
	if(FlagConsoleGetCommand){
		FlagConsoleGetCommand = 0;
		if(Select&0x08) ConsoleGetCommand();
	}
	if(FlagAUX1GetStatus){
		FlagAUX1GetStatus = 0;
		if(Select&0x10) AUX1GetStatus();
	}
	if(FlagFCONGetStatus){
		FlagFCONGetStatus = 0;
		if(Select&0x20) FCONGetStatus();
	}
	if(FlagRTCGetStatus){
		FlagRTCGetStatus = 0;
//		if(Select&0x40) RTCGetStatus();
	}
	if(FlagInterpreterRound){
		FlagInterpreterRound = 0;
		InterpreterRound();
	}
}

void AUTOMATE::ClickCount(){
	ClickCounter++;
	if(ClickCounter == 100){
		Interpreter.Tick();
		ClickCounter = 0;
	}
}

void AUTOMATE::StatusBarRefresh(){
	for(u32 i=0;i<37;i++)
		Status.StatusBar[i] = ' ';
	
	Status.StatusBar[37] = 0;
	
	static u8 OFFSET_LINK = 0;
	static u8 OFFSET_MATERIAL = 9;
	static u8 OFFSET_INTRP = 17;
	static u8 OFFSET_TIME = 24;
	
	Status.StatusBar[OFFSET_LINK + 0] = '\001'; //通信状态
	Status.StatusBar[OFFSET_LINK + 1] = '[';
	if(Status.LinkToTcon) Status.StatusBar[OFFSET_LINK + 2] = 'T'; //HeatControler	
	else Status.StatusBar[OFFSET_LINK + 2] = '?';
	if(Status.LinkToAux1) Status.StatusBar[OFFSET_LINK + 3] = 'A'; //Auxiliary
	else Status.StatusBar[OFFSET_LINK + 3] = '?';	
	if(Status.LinkToScon) Status.StatusBar[OFFSET_LINK + 4] = 'S'; //MotionControler
	else Status.StatusBar[OFFSET_LINK + 4] = '?';
	if(Status.LinkToFcon) Status.StatusBar[OFFSET_LINK + 5] = 'F'; //FileControler
	else Status.StatusBar[OFFSET_LINK + 5] = '?';
	if(Status.FileExist) Status.StatusBar[OFFSET_LINK + 6] = '\004'; //FileExistance
	else Status.StatusBar[OFFSET_LINK + 6] = '?';
	Status.StatusBar[OFFSET_LINK + 7] = ']';
	
	
	Status.StatusBar[OFFSET_MATERIAL + 0] = '\002'; //原料状态
	Status.StatusBar[OFFSET_MATERIAL + 1] = '[';
	Status.StatusBar[OFFSET_MATERIAL + 2] = 'A';
	if(Status.MaterialA) Status.StatusBar[OFFSET_MATERIAL + 3] = '/';
	else Status.StatusBar[OFFSET_MATERIAL + 3] = '!';
	Status.StatusBar[OFFSET_MATERIAL + 4] = 'B';
	if(Status.MaterialB) Status.StatusBar[OFFSET_MATERIAL + 5] = '/';
	else Status.StatusBar[OFFSET_MATERIAL + 5] = '!';
	Status.StatusBar[OFFSET_MATERIAL + 6] = ']';
	
	Status.StatusBar[OFFSET_INTRP + 0] = '\007'; //解释器状态
	Status.StatusBar[OFFSET_INTRP + 1] = '['; 
	switch(Interpreter.RoundStatus){
		case'I':Status.StatusBar[OFFSET_INTRP + 2] = 'I';break;
		case'B':Status.StatusBar[OFFSET_INTRP + 2] = 'P';break;
		case'R':Status.StatusBar[OFFSET_INTRP + 2] = 'R';break;
		default:Status.StatusBar[OFFSET_INTRP + 2] = 'W';break;
	}
	bool ExisWarning = 0;
	for(u8 i=0;i<10;i++)
		if(Interpreter.Info.Warning[i][0] != 0xFF)
			ExisWarning = 1;
	if(ExisWarning)
		Status.StatusBar[OFFSET_INTRP + 3] = '!';
	else
		Status.StatusBar[OFFSET_INTRP + 3] = '-';
	if(Interpreter.Info.Error[0] != 0xFF)
		Status.StatusBar[OFFSET_INTRP + 4] = '\010';
	else
		Status.StatusBar[OFFSET_INTRP + 4] = '-';
	Status.StatusBar[OFFSET_INTRP + 5] = ']';
	
	Status.StatusBar[OFFSET_TIME + 0] = '\003'; //计时器状态
	Status.StatusBar[OFFSET_TIME + 1] = '[';
	Status.StatusBar[OFFSET_TIME + 2] = Interpreter.Info.HMS[0]%1000/100 + '0';
	Status.StatusBar[OFFSET_TIME + 3] = Interpreter.Info.HMS[0]%100/10 + '0';
	Status.StatusBar[OFFSET_TIME + 4] = Interpreter.Info.HMS[0]%10/1 + '0';
	Status.StatusBar[OFFSET_TIME + 5] = ':';
	Status.StatusBar[OFFSET_TIME + 6] = Interpreter.Info.HMS[1]%100/10 + '0';
	Status.StatusBar[OFFSET_TIME + 7] = Interpreter.Info.HMS[1]%10/1 + '0';
	Status.StatusBar[OFFSET_TIME + 8] = '@';
	Status.StatusBar[OFFSET_TIME + 9] = Interpreter.Info.Percent%100/10 + '0';
	Status.StatusBar[OFFSET_TIME + 10] = Interpreter.Info.Percent%10/1 + '0';
	Status.StatusBar[OFFSET_TIME + 11] = ']';
	
	Cconi.SetStatusBar();
}

void AUTOMATE::InfoLCDRefreash(){
	if(Cconi.TestLink()){
		Cconi.SetInfoScreen();
		StatusBarRefresh();
		Status.LinkToCcon = 1;
	}
	else
		Status.LinkToCcon = 0;
}

void AUTOMATE::InfoLCDRefreashCount(){
	InfoLCDRefreashCounter++;
	if(InfoLCDRefreashCounter == INFOLCDREFREASH){
		InfoLCDRefreashCounter = 0;
		FlagInfoLCDRefreash = 1;
	}
}

void AUTOMATE::TCONGetStatus(){
	if(Tconi.TestLink()){
		Tconi.GetTemperatureExtruderA();
		Tconi.GetTemperatureExtruderB();
		Tconi.GetTemperatureBasePlane();
		Tconi.GetTemperatureEnvironment();
	
		Tconi.GetTemperatureEASetting();
		Tconi.GetTemperatureEBSetting();
		Tconi.GetTemperatureBPSetting();
	
		Tconi.GetCoolerPowerExtruderA();
		Tconi.GetCoolerPowerExtruderB();
	
		Tconi.GetHeaterPowerExtruderA();
		Tconi.GetHeaterPowerExtruderB();
		Tconi.GetHeaterPowerBasePlane();
		
		Tconi.GetStatusExtruderA();           
		Tconi.GetStatusExtruderB();
		Tconi.GetStatusBasePlane();
		
		Status.LinkToTcon = 1;
	}
	else
		Status.LinkToTcon = 0;
}

void AUTOMATE::TCONGetStatusCount(){
	TCONGetStatusCounter++;
	if(TCONGetStatusCounter == TCONGETSTATUS){
		TCONGetStatusCounter = 0;
		FlagTCONGetStatus = 1;
	}
}

void AUTOMATE::SCONGetStatus(){
	if(Sconi.TestLink()){
		Sconi.GetPositionX();
		Sconi.GetPositionY();
		Sconi.GetPositionZ();
		Sconi.GetPositionA();
		Sconi.GetPositionB();
		
		Sconi.GetPositionXMax();
		Sconi.GetPositionYMax();
		Sconi.GetPositionZMax();
		
//		Sconi.GetTestPointA();
//		Sconi.GetTestPointB();
//		Sconi.GetTestPointC();
//		Sconi.GetTestPointD();
//		Sconi.GetTestPointM();
		
		Sconi.GetStatusFA();
		Sconi.GetStatusIDP();
//		Sconi.GetStatusLeveling();
		Sconi.GetStatusStepper();
		Sconi.GetStatusHomed();
		
		Sconi.GetL1Rest();
		Sconi.GetL2Rest();
		Sconi.GetL1Max();
		Sconi.GetL2Max();
		
		Status.LinkToScon = 1;
	}
	else
		Status.LinkToScon = 0;
}

void AUTOMATE::SCONGetStatusCount(){
	SCONGetStatusCounter++;
	if(SCONGetStatusCounter == SCONGETSTATUS){
		SCONGetStatusCounter = 0;
		FlagSCONGetStatus = 1;
	}
}


void AUTOMATE::FCONGetStatus(){
	if(Fconi.TestLink()){
		Fconi.FileTest();
		Fconi.GetTotalVolume();
		Fconi.GetRestVolume();
		Fconi.GetFileName();
		Fconi.GetFileSize();
		Fconi.GetFileCommandNumber();
		Fconi.GetFileEstimateTime();
		Fconi.GetFileTotalDistance();
		Fconi.GetFileInfo1();
		Fconi.GetFileInfo2();
		Fconi.GetFileInfo3();
		Fconi.GetFileInfo4();
		Fconi.GetFileInfo5();
		Fconi.GetFileInfo6();	
		Fconi.GetFilePointer();		
		Status.LinkToFcon = 1;
	}
	else
		Status.LinkToFcon = 0;
}

void AUTOMATE::FCONGetStatusCount(){
	FCONGetStatusCounter++;
	if(FCONGetStatusCounter == FCONGETSTATUS){
		FCONGetStatusCounter = 0;
		FlagFCONGetStatus = 1;
	}
}



void AUTOMATE::ConsoleGetCommand(){
	if(Cconi.TestLink()){
		if(Cconi.GetCommand() == 'D'){
			Shell.Decoder();
		}
		Status.LinkToCcon = 1;
	}
	else
		Status.LinkToCcon = 0;
}

void AUTOMATE::ConsoleGetCommandCount(){
	ConsoleGetCommandCounter++;
	if(ConsoleGetCommandCounter == CONSOLEGETCOMMAND){
		ConsoleGetCommandCounter = 0;
		FlagConsoleGetCommand = 1;
	}
}

void AUTOMATE::AUX1GetStatus(){
	if(AuxIO.TestLink()){
		AuxIO.GetStatusMaterialA();
		AuxIO.GetStatusMaterialB();
		
		Status.LinkToAux1 = 1;
	}
	else
		Status.LinkToAux1 = 0;
}

void AUTOMATE::AUX1GetStatusCount(){
	AUX1GetStatusCounter++;
	if(AUX1GetStatusCounter == AUX1GETSTATUS){
		AUX1GetStatusCounter = 0;
		FlagAUX1GetStatus = 1;
	}
}

void AUTOMATE::RTCGetStatus(){
	RTClock.GetTime();
	
}

void AUTOMATE::RTCGetStatusCount(){
	RTCGetStatusCounter++;
	if(RTCGetStatusCounter == RTCGETSTATUS){
		RTCGetStatusCounter = 0;
		FlagRTCGetStatus = 1;
	}
}

void AUTOMATE::InterpreterRound(){
	Interpreter.Round();
}
void AUTOMATE::InterpreterRoundCount(){
	InterpreterRoundCounter++;
	if(InterpreterRoundCounter == INTRPROUND){
		InterpreterRoundCounter = 0;
		FlagInterpreterRound = 1;
	}
}

