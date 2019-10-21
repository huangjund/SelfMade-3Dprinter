#include "CCON.h"
#include "PWM.h"

CCON CoolerControler;

void CCON::Init(){
	SetPowerCoolerExtruderA(0);
	SetPowerCoolerExtruderB(0);
}	

void CCON::SetPowerCoolerExtruderA(u32 P){
	Digital_Output.SetPWMCoolerExtruderA(P);
}

void CCON::SetPowerCoolerExtruderB(u32 P){
	Digital_Output.SetPWMCoolerExtruderB(P);
}

u32 CCON::GetPowerCoolerExtruderA(){
	return Digital_Output.GetPWMCoolerExtruderA();
}

u32 CCON::GetPowerCoolerExtruderB(){
	return Digital_Output.GetPWMCoolerExtruderB();
}

