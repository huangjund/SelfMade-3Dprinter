#include "Automate.h"
#include "SCONI.h"
#include "TCONI.h"
#include "FCONI.h"
#include "CCONI.h"
#include "Shell.h"
#include "Status.h"

#define INFOLCDREFREASH 100 //每1s刷新一次状态显示屏
#define TCONGETSTATUS 100 //每1s获取一次热控制模块状态
#define SCONGETSTATUS 90 //每1s获取一次步进控制模块状态
#define CONSOLEGETCOMMAND 10 //每0.1s检测一次控制台状态

AUTOMATE Automate;

extern"C"void TIM3_IRQHandler(void){
	if(TIM3->SR&0x01){
		Automate.InfoLCDRefreashCount();
		Automate.TCONGetStatusCount();
		Automate.SCONGetStatusCount();
		Automate.ConsoleGetCommandCount();
		TIM3->SR&=~0x01;
	}
}



void AUTOMATE::Init(){
	RCC->APB1ENR|=0x01<<1;//TIM3使能
  TIM3->ARR=10000;//预装载 10ms
  TIM3->PSC=71;//分频
  TIM3->CR1|=0x01<<4;//向下计数
  TIM3->DIER|=0x01<<0;//禁止更新中断
  NVIC->IP[29]=0x00;//优先级0
  NVIC->ISER[0]|=0x01<<29;
  TIM3->CR1|=0x01<<0;//使能
  
	FlagInfoLCDRefreash = 0;
	InfoLCDRefreashCounter = 0;
	
	FlagTCONGetStatus = 0;
	TCONGetStatusCounter = 0;
	
	FlagSCONGetStatus = 0;
	SCONGetStatusCounter = 0;
	
	FlagConsoleGetCommand = 0;
	ConsoleGetCommandCounter = 0;
	
}

void AUTOMATE::Checking(){
	if(FlagInfoLCDRefreash){
		FlagInfoLCDRefreash = 0;
		InfoLCDRefreash();
	}
	if(FlagTCONGetStatus){
		FlagTCONGetStatus = 0;
		TCONGetStatus();
	}
	if(FlagSCONGetStatus){
		FlagSCONGetStatus = 0;
		SCONGetStatus();
	}
	if(FlagConsoleGetCommand){
		FlagConsoleGetCommand = 0;
		ConsoleGetCommand();
	}
}

void AUTOMATE::InfoLCDRefreash(){
	if(Cconi.TestLink()){
		Cconi.SetInfoScreen();
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

	