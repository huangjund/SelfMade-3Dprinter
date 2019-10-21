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
		if(Counter == REFRESH_CYCLE){ //PWM刷新
			
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
	
	StatusExtruderA = 'O'; //关闭
 	StatusExtruderB = 'O'; //关闭
	StatusBasePlane = 'O'; //关闭
	
	RCC->APB1ENR |= 0x01<<0;
  TIM2->ARR = 249;//预装载
  TIM2->PSC = 71;//分频
  TIM2->CR1 |= 0x01<<4; //向下计数
  TIM2->DIER |= 0x01<<0;
  NVIC->IP[28] = 0x10;//优先级1
  NVIC->ISER[0] |= 0x01<<28;
  TIM2->CR1 |= 0x01<<0;//使能
}

void HCON::SetTemperatureExtruderA(u32 T){ //设定挤出机A预定温度
	if(T > 320) TemperatureExtruderASetting = 320;
	else TemperatureExtruderASetting = T;
}

void HCON::SetTemperatureExtruderB(u32 T){ //设定挤出机B预定温度
	if(T > 320) TemperatureExtruderBSetting = 320;
	else TemperatureExtruderBSetting = T;
}

void HCON::SetTemperatureBasePlane(u32 T){ //设定基准面预定温度
	if(T > 150) TemperatureBasePlaneSetting = 150;
	else TemperatureBasePlaneSetting = T;
}

u32 HCON::GetExtruderASetting(){ //读取挤出机A设定温度
	if(TemperatureExtruderASetting > 320) return 320;
	else return TemperatureExtruderASetting;
}
u32 HCON::GetExtruderBSetting(){ //读取挤出机B设定温度
	if(TemperatureExtruderBSetting > 320) return 320;
	else return TemperatureExtruderBSetting;
}

u32 HCON::GetBasePlaneSetting(){ //读取基准面设定温度
	if(TemperatureBasePlaneSetting > 150) return 150;
	else return TemperatureBasePlaneSetting;
}

u32 HCON::GetPowerHeaterExtruderA(){ //读取挤出机A加热器功率
	return Digital_Output.GetPWMHeaterExtruderA();
}

u32 HCON::GetPowerHeaterExtruderB(){ //读取挤出机BA加热器功率
	return Digital_Output.GetPWMHeaterExtruderB();
}

u32 HCON::GetPowerHeaterBasePlane(){ //读取基准面加热器功率
	return Digital_Output.GetPWMHeaterBasePlane();
}

char HCON::GetStatusExtruderA(){ //读取挤出机A状态
	return StatusExtruderA;
}

char HCON::GetStatusExtruderB(){ //读取挤出机B状态
	return StatusExtruderB;
}

char HCON::GetStatusBasePlane(){ //读取基准面状态
	return StatusBasePlane;
}

void HCON::CaclStatusExtruderA(){ //计算挤出机A状态
	u32 TemperaturePresent = Analog_Input.GetTemperatureExtruderA();
	u32 TemperatureSetting = TemperatureExtruderASetting*100;
	
	char TempStatus = 'O';
	
	if(TemperatureSetting == 0) TempStatus = 'O';
	else if(TemperatureSetting > TemperaturePresent+200) TempStatus = 'H';
	else if(TemperatureSetting+200 < TemperaturePresent) TempStatus = 'C';
	else TempStatus = 'M';
	
	StatusExtruderA = TempStatus;
}

void HCON::CaclStatusExtruderB(){ //计算挤出机B状态
	u32 TemperaturePresent = Analog_Input.GetTemperatureExtruderB();
	u32 TemperatureSetting = TemperatureExtruderBSetting*100;
	
	char TempStatus = 'O';
	
	if(TemperatureSetting == 0) TempStatus = 'O';
	else if(TemperatureSetting > TemperaturePresent+200) TempStatus = 'H';
	else if(TemperatureSetting+200 < TemperaturePresent) TempStatus = 'C';
	else TempStatus = 'M';
	
	StatusExtruderB = TempStatus;
}

void HCON::CaclStatusBasePlane(){ //计算基准面状态
	u32 TemperaturePresent = Analog_Input.GetTemperatureBasePlane();
	u32 TemperatureSetting = TemperatureBasePlaneSetting*100;
	
	char TempStatus = 'O';
	
	if(TemperatureSetting == 0) TempStatus = 'O';
	else if(TemperatureSetting > TemperaturePresent+200) TempStatus = 'H';
	else if(TemperatureSetting+200 < TemperaturePresent) TempStatus = 'C';
	else TempStatus = 'M';
	
	StatusBasePlane = TempStatus;
}

void HCON::CaclPWMExtruderA(){ //计算挤出机A占空比
	static s64 Register[2];
	Register[0] = Register[1];
	Register[1] = Analog_Input.GetTemperatureExtruderA();
	
	static s64 Integrator = 0;
	
	s64 Difference = Register[1]-Register[0]; //温度差分
	s64 Error = TemperatureExtruderASetting*100 - Register[1]; //温度误差
	
	s64 TempPWM = 0;
	
	if(TemperatureExtruderASetting < 10){
		TempPWM = 0;
		Integrator = 0;
	}
	else if(Error > 200){ //低于设定温度2'以上
		TempPWM = 100;
		Integrator = 0;
	}
	else if(Error < -200){ //高于设定温度2'以上
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

void HCON::CaclPWMExtruderB(){ //计算挤出机B占空比
	static s64 Register[2];
	Register[0] = Register[1];
	Register[1] = Analog_Input.GetTemperatureExtruderB();
	
	static s64 Integrator = 0;
	
	s64 Difference = Register[1]-Register[0]; //温度差分
	s64 Error = TemperatureExtruderBSetting*100 - Register[1]; //温度误差
	
	s64 TempPWM = 0;
	
	if(TemperatureExtruderBSetting < 10){
		TempPWM = 0;
		Integrator = 0;
	}
	else if(Error > 200){ //低于设定温度2'以上
		TempPWM = 100;
		Integrator = 0;
	}
	else if(Error < -200){ //高于设定温度2'以上
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

void HCON::CaclPWMBasePlane(){ //计算基准面占空比
	u32 TemperatureSetting = TemperatureBasePlaneSetting*100;
	u32 TemperaturePresent = Analog_Input.GetTemperatureBasePlane();
	u32 TempPWM = 0;
	
	if(TemperatureSetting == 0) TempPWM = 0; //关闭
	if(TemperatureSetting > TemperaturePresent+100) TempPWM = 100;
	if(TemperatureSetting+100 < TemperaturePresent) TempPWM = 0;
	else TempPWM = (TemperatureSetting+100-TemperaturePresent)/2;
	
	Digital_Output.SetPWMHeaterBasePlane(TempPWM);	
}

