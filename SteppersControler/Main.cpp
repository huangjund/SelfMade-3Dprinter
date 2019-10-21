#include "stm32f10x.h"
#include "Stepper.h"
#include "Planner.h"
#include "LCD.h"
#include "COM.h"
#include "Status.h"
#include "Servo.h"

int main(){
	RCC->APB2ENR |= 0x01;
  AFIO->MAPR &= 0xF0FFFFFF;//IO����ӳ��
  AFIO->MAPR |= 0x02000000;
	Stepper.Init();
	Planner.Init();
	Serial.Init();
	Status.Init();
	Lcd.Init();
	PROB_ENABLE(0);
	
//	Delay_Ms(1000);
//Stepper.AutoHoming();
	
	while(1){
		if(Serial.RxFinish) //�������
			Serial.Decoder();	
		
		if(!Stepper.PtrLoader->Full){ //L1�����
			if(Planner.PtrLauncher->Full){ //L2����ǿ�
				Planner.Launch(Stepper.PtrLoader); //װ���
				Stepper.PtrLoader = Stepper.PtrLoader->Next; //�ƶ�ָ��
			}
		}
	}
	
}

