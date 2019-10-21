#include "stm32f10x.h"
#include "LCD.h"
#include "GUI.h"
#include "Status.h"
#include "COM.h"
/*
Grey: 0xC618,0x8430
Orange: 0xFBA0,0xFB20
Blue: 0x7D7C,0x6C7B
*/
#define COLORA 0x6C7B

void Refresh(); //к╒фа

int main(){
	RCC->APB2ENR |= 0x01;
  AFIO->MAPR &= 0xF0FFFFFF;
  AFIO->MAPR |= 0x02000000;
	LCDInterface.Init();
	Status.Init();
	Serial.Init();
	Refresh();
	while(1){
		if(Serial.RxFinish){
			Serial.Decoder();
			Serial.Tx('D');
			Serial.RxFinish = 0;
		}
	}
}

void Refresh(){
	Status.DisableCursor();
	static u16 RectanglePosi1[4] = {1,1,478,34};
	static u16 RectanglePosi2[4] = {1,38,478,318};
	Window.DrawFrame(RectanglePosi1,0xC618,0x8430);
	Window.DrawFrame(RectanglePosi2,0xC618,0x8430);

	Status.ShowStatus();
	Status.ShowData();
	Status.EnableCursor();
}


