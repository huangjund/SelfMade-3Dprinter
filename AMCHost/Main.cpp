#include "stm32f10x.h"
#include "CBUS.h"
#include "IBUS.h"
#include "ABUS.h"
#include "Shell.h"
#include "RTClock.h"
#include "Automate.h"
#include "Interpreter.h"

void Delay_Ms(u32 n){
	while(n--){
		SysTick->LOAD = 72000;
		SysTick->CTRL = 0x00000005;
		while(!(SysTick->CTRL & 0x00010000));
		SysTick->CTRL = 0x00000004;
	}
}



int main(){
	RCC->APB2ENR = 0x01;
  AFIO->MAPR &= 0xF0FFFFFF;
  AFIO->MAPR |= 0x02000000;
	Delay_Ms(5000);
	RTClock.Init();
	Cbus.Init();
	Ibus.Init();
	Abus.Init();
	Automate.Init();
	Shell.Init();
	Interpreter.Init();	
	
	while(1)
		Automate.Checking(0xFF);
}


