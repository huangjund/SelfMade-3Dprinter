#include "HCON.h"
#include "PWM.h"
#include "ADC.h"
#include "LCD.h"

#define s64 long long
	
#define REFRESH_CYCLE 10

HCON HeaterControler;

const s64 PID_P_ExtA = 2500;
const s64 PID_I_ExtA = 0;
const s64 PID_D_ExtA = 0;//-100;

const s64 PID_P_ExtB = 2500;
const s64 PID_I_ExtB = 0;
const s64 PID_D_ExtB = 0;//-100;


extern"C"void TIM2_IRQHandler(void){
	if(TIM2->SR&0x01){
		static u32 Counter = 0;

		Counter++;
		if(Counter == REFRESH_CYCLE){ //PWMˢ��
			
			HeaterControler.CaclPWMExtruderA();
			HeaterControler.CaclPWMExtruderB();
			HeaterControler.CaclPWMBasePlane();
			
			Analog_Input.GetTemperatureEnvironment();
			
			HeaterControler.CaclStatusExtruderA();
			HeaterControler.CaclStatusExtruderB();
			HeaterControler.CaclStatusBasePlane();
			
			Counter = 0;
		}
		
		TIM2->SR&=~0x01;
	}
}


void HCON::Init(){
	TemperatureExtruderASetting = 0;
	TemperatureExtruderBSetting = 0;
	TemperatureBasePlaneSetting = 0;
	
	StatusExtruderA = 'O'; //�ر�
 	StatusExtruderB = 'O'; //�ر�
	StatusBasePlane = 'O'; //�ر�
	
	RCC->APB1ENR |= 0x01<<0;
  TIM2->ARR = 249;//Ԥװ��
  TIM2->PSC = 71;//��Ƶ
  TIM2->CR1 |= 0x01<<4; //���¼���
  TIM2->DIER |= 0x01<<0;
  NVIC->IP[28] = 0x10;//���ȼ�1
  NVIC->ISER[0] |= 0x01<<28;
  TIM2->CR1 |= 0x01<<0;//ʹ��
}

void HCON::SetTemperatureExtruderA(u32 T){ //�趨������AԤ���¶�
	if(T > 320) TemperatureExtruderASetting = 320;
	else TemperatureExtruderASetting = T;
}

void HCON::SetTemperatureExtruderB(u32 T){ //�趨������BԤ���¶�
	if(T > 320) TemperatureExtruderBSetting = 320;
	else TemperatureExtruderBSetting = T;
}

void HCON::SetTemperatureBasePlane(u32 T){ //�趨��׼��Ԥ���¶�
	if(T > 150) TemperatureBasePlaneSetting = 150;
	else TemperatureBasePlaneSetting = T;
}

u32 HCON::GetExtruderASetting(){ //��ȡ������A�趨�¶�
	if(TemperatureExtruderASetting > 320) return 320;
	else return TemperatureExtruderASetting;
}
u32 HCON::GetExtruderBSetting(){ //��ȡ������B�趨�¶�
	if(TemperatureExtruderBSetting > 320) return 320;
	else return TemperatureExtruderBSetting;
}

u32 HCON::GetBasePlaneSetting(){ //��ȡ��׼���趨�¶�
	if(TemperatureBasePlaneSetting > 150) return 150;
	else return TemperatureBasePlaneSetting;
}

u32 HCON::GetPowerHeaterExtruderA(){ //��ȡ������A����������
	return Digital_Output.GetPWMHeaterExtruderA();
}

u32 HCON::GetPowerHeaterExtruderB(){ //��ȡ������BA����������
	return Digital_Output.GetPWMHeaterExtruderB();
}

u32 HCON::GetPowerHeaterBasePlane(){ //��ȡ��׼�����������
	return Digital_Output.GetPWMHeaterBasePlane();
}

char HCON::GetStatusExtruderA(){ //��ȡ������A״̬
	return StatusExtruderA;
}

char HCON::GetStatusExtruderB(){ //��ȡ������B״̬
	return StatusExtruderB;
}

char HCON::GetStatusBasePlane(){ //��ȡ��׼��״̬
	return StatusBasePlane;
}

void HCON::CaclStatusExtruderA(){ //���㼷����A״̬
	u32 TemperaturePresent = Analog_Input.GetTemperatureExtruderA();
	u32 TemperatureSetting = TemperatureExtruderASetting*100;
	
	char TempStatus = 'O';
	
	if(TemperatureSetting == 0) TempStatus = 'O';
	else if(TemperatureSetting > TemperaturePresent+200) TempStatus = 'H';
	else if(TemperatureSetting+200 < TemperaturePresent) TempStatus = 'C';
	else TempStatus = 'M';
	
	StatusExtruderA = TempStatus;
}

void HCON::CaclStatusExtruderB(){ //���㼷����B״̬
	u32 TemperaturePresent = Analog_Input.GetTemperatureExtruderB();
	u32 TemperatureSetting = TemperatureExtruderBSetting*100;
	
	char TempStatus = 'O';
	
	if(TemperatureSetting == 0) TempStatus = 'O';
	else if(TemperatureSetting > TemperaturePresent+200) TempStatus = 'H';
	else if(TemperatureSetting+200 < TemperaturePresent) TempStatus = 'C';
	else TempStatus = 'M';
	
	StatusExtruderB = TempStatus;
}

void HCON::CaclStatusBasePlane(){ //�����׼��״̬
	u32 TemperaturePresent = Analog_Input.GetTemperatureBasePlane();
	u32 TemperatureSetting = TemperatureBasePlaneSetting*100;
	
	char TempStatus = 'O';
	
	if(TemperatureSetting == 0) TempStatus = 'O';
	else if(TemperatureSetting > TemperaturePresent+200) TempStatus = 'H';
	else if(TemperatureSetting+200 < TemperaturePresent) TempStatus = 'C';
	else TempStatus = 'M';
	
	StatusBasePlane = TempStatus;
}

void HCON::CaclPWMExtruderA(){ //���㼷����Aռ�ձ�
	static s64 Register[2];
	Register[0] = Register[1];
	Register[1] = Analog_Input.GetTemperatureExtruderA();
	
	static s64 Integrator = 0;
	
	s64 Difference = Register[1]-Register[0]; //�¶Ȳ��
	s64 Error = TemperatureExtruderASetting*100 - Register[1]; //�¶����
	
	s64 TempPWM = 0;
	
	if(TemperatureExtruderASetting < 10){
		TempPWM = 0;
		Integrator = 0;
	}
	else if(Error > 200){ //�����趨�¶�2'����
		TempPWM = 100;
		Integrator = 0;
	}
	else if(Error < -200){ //�����趨�¶�2'����
		TempPWM = 0;
		Integrator = 0;
	}
	else{
		Error += 200;
		Integrator += Error;
		TempPWM = Error*PID_P_ExtA/10000;
		TempPWM += Integrator*PID_I_ExtA/10000;
		TempPWM += Difference*PID_D_ExtA/10000;
		
		if(TempPWM > 100) TempPWM = 100;
		else if(TempPWM < 0) TempPWM = 0;

	}
	
	Digital_Output.SetPWMHeaterExtruderA(TempPWM);	
}

void HCON::CaclPWMExtruderB(){ //���㼷����Bռ�ձ�
	static s64 Register[2];
	Register[0] = Register[1];
	Register[1] = Analog_Input.GetTemperatureExtruderB();
	
	static s64 Integrator = 0;
	
	s64 Difference = Register[1]-Register[0]; //�¶Ȳ��
	s64 Error = TemperatureExtruderBSetting*100 - Register[1]; //�¶����
	
	s64 TempPWM = 0;
	
	if(TemperatureExtruderBSetting < 10){
		TempPWM = 0;
		Integrator = 0;
	}
	else if(Error > 200){ //�����趨�¶�2'����
		TempPWM = 100;
		Integrator = 0;
	}
	else if(Error < -200){ //�����趨�¶�2'����
		TempPWM = 0;
		Integrator = 0;
	}
	else{
		Integrator += Error;
		Error += 200;
		TempPWM = Error*PID_P_ExtB/10000;
		TempPWM += Integrator*PID_I_ExtB/10000;
		TempPWM += Difference*PID_D_ExtB/10000;
		
		if(TempPWM > 100) TempPWM = 100;
		else if(TempPWM < 0) TempPWM = 0;
	}
	
	Digital_Output.SetPWMHeaterExtruderB(TempPWM);	
}

void HCON::CaclPWMBasePlane(){ //�����׼��ռ�ձ�
	u32 TemperatureSetting = TemperatureBasePlaneSetting*100;
	u32 TemperaturePresent = Analog_Input.GetTemperatureBasePlane();
	u32 TempPWM = 0;
	
	if(TemperatureSetting == 0) TempPWM = 0; //�ر�
	if(TemperatureSetting > TemperaturePresent+100) TempPWM = 100;
	if(TemperatureSetting+100 < TemperaturePresent) TempPWM = 0;
	else TempPWM = (TemperatureSetting+100-TemperaturePresent)/2;
	
	Digital_Output.SetPWMHeaterBasePlane(TempPWM);	
}

