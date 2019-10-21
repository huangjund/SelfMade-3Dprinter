#include "stm32f10x.h"
#include "COM.h"
#include "ADC.h"
#include "PWM.h"
#include "LCD.h"
#include "HCON.h"
#include "CCON.h"


int main(){
	RCC->APB2ENR |= 0x01;
  AFIO->MAPR &= 0xF0FFFFFF;
  AFIO->MAPR |= 0x02000000;
	Serial.Init();
	Analog_Input.Init();
	Digital_Output.Init();
	Lcd.Init();
	HeaterControler.Init();
	CoolerControler.Init();
	while(1){
		if(Serial.RxFinish)
			Serial.Decoder();
	}
}
