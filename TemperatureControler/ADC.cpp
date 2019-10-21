#include "ADC.h"
#include "TempTable_100K.txt"
#include "TEmpTable_10K.txt"



ANALOG Analog_Input;


//u64 Filter_LowPass[21] = { //21Points
//	2656,209,214,223,229,233,237,240,242,244,
//	 244,244,242,240,237,233,229,223,214,209,2656
//};

//s64 Filter_LowPass[5] = { //5Points
//	4315,271,267,271,4315
//};

s64 Filter_Window[5] = { //5Points
	2000,2000,2000,2000,2000
};

//s64 Filter_Smooth[21] = { //21Points
//	476,476,476,476,476,476,476,476,476,476,
//	476,476,476,476,476,476,476,476,476,476,476
//};

s64 *FILTER = Filter_Window;

void ANALOG::Init(){
	RCC->APB2ENR |= 0x01<<2; //����PAʱ��
	GPIOA->CRL &= 0xFFFF0000; //PA��4λ����Ϊģ������
	RCC->APB2ENR |= 0x01<<9; //����ADC1ʱ��
	RCC->CFGR &= ~(0x03<<14); //���ADC��Ƶϵ��
	for(int i=3;i;i--); //�ӳ�һ��
	RCC->CFGR |= 0x02<<14; //ADCʱ��6��Ƶ(F=12MHz)
	for(int i=3;i;i--); //�ӳ�һ��	
	ADC1->JOFR1 = 0x00; //ע��ͨ��1����ƫ��0
	ADC1->JOFR2 = 0x00; //ע��ͨ��2����ƫ��0
	ADC1->JOFR3 = 0x00; //ע��ͨ��3����ƫ��0
	ADC1->JOFR4 = 0x00; //ע��ͨ��4����ƫ��0
	ADC1->JSQR  =0x03<<20; //ע��ͨ�����г���4
	ADC1->JSQR |=0x00<<0; //ת��1ʹ��ͨ��0
	ADC1->JSQR |=0x01<<5; //ת��2ʹ��ͨ��1
	ADC1->JSQR |=0x02<<10; //ת��3ʹ��ͨ��2
	ADC1->JSQR |=0x03<<15; //ת��4ʹ��ͨ��3
	ADC1->SMPR2  =0x07<<0; //ͨ��0����239.5����
	ADC1->SMPR2 |=0x07<<3; //ͨ��1����239.5����
	ADC1->SMPR2 |=0x07<<6; //ͨ��2����239.5����
	ADC1->SMPR2 |=0x07<<9; //ͨ��3����239.5����
	ADC1->CR1  =0x01<<8; //ʹ��ɨ��ģʽ
	ADC1->CR2  =0x01<<1; //ʹ������ת��ģʽ
	ADC1->CR1 |=0x01<<10; //�����Զ���ע��ͨ����ת��
	ADC1->CR2 |=0x01<<0; //����ģ��ת����
	ADC1->CR2 |=0x01<<3; //��ʼ����λУ׼
	while(ADC1->CR2&0x08); //�ȴ���λУ׼���
	ADC1->CR2 |= 0x01<<2; //��ʼ��ADУ׼
	while(ADC1->CR2&0x04); //�ȴ�ADУ׼���
	ADC1->CR2 |= 0x01<<0; //����ת��
//	ADC1->CR2 |= 0x01<<0; //����ת��
	
}

u16 ANALOG::GetTemperatureExtruderA(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[0][i] = TemperaturePool[0][i+1];
	
	#ifdef TEMP_EXTRUDERA_100K //ʹ��100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR1]; //���ת��
	#else //ʹ��10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR1]; //���ת��
	#endif	
	
	TemperaturePool[0][TEMP_POOL_NUMBER-1] = Temp;
	
//	return Average((s64*)TemperaturePool[0]);	
	return Convolution(FILTER,(s64*)TemperaturePool[0]);

}

u16 ANALOG::GetTemperatureExtruderB(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[1][i] = TemperaturePool[1][i+1];
	TemperaturePool[1][TEMP_POOL_NUMBER-1] = (u16)ADC1->JDR2;
	
	#ifdef TEMP_EXTRUDERB_100K //ʹ��100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR2]; //���ת��
	#else //ʹ��10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR2]; //���ת��
	#endif	
	
	TemperaturePool[1][TEMP_POOL_NUMBER-1] = Temp;

//	return Average((s64*)TemperaturePool[1]);	
	return Convolution(FILTER,(s64*)TemperaturePool[1]);
}

u16 ANALOG::GetTemperatureBasePlane(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[2][i] = TemperaturePool[2][i+1];
	TemperaturePool[2][TEMP_POOL_NUMBER-1] = (u16)ADC1->JDR3;

	#ifdef TEMP_BASEPLANE_100K //ʹ��100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR3]; //���ת��
	#else //ʹ��10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR3]; //���ת��
	#endif	
	
	TemperaturePool[2][TEMP_POOL_NUMBER-1] = Temp;

//	return Average((s64*)TemperaturePool[2]);	
	return Convolution(FILTER,(s64*)TemperaturePool[2]);
}

u16 ANALOG::GetTemperatureEnvironment(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[3][i] = TemperaturePool[3][i+1];
	TemperaturePool[3][TEMP_POOL_NUMBER-1] = (u16)ADC1->JDR4;
	
	#ifdef TEMP_ENVIRONMENT_100K //ʹ��100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR4]; //���ת��
	#else //ʹ��10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR4]; //���ת��
	#endif	
	
	TemperaturePool[3][TEMP_POOL_NUMBER-1] = Temp;
	
//	return Average((s64*)TemperaturePool[3]);
	return Convolution(FILTER,(s64*)TemperaturePool[3]);
}

u64 ANALOG::Convolution(s64 H[],s64 X[]){
	s64 Value = 0;
	for(u32 i=0;i<TEMP_POOL_NUMBER;i++)
	Value += (X[i]*100*H[TEMP_POOL_NUMBER-i-1])/1000000;
	if(Value < 0) return 0;
	return Value;
}

s64 ANALOG::Average(s64 X[]){
	s64 Value = 0;
	for(u32 i=0;i<TEMP_POOL_NUMBER;i++)
		Value += X[i];
	return Value/TEMP_POOL_NUMBER;
}
