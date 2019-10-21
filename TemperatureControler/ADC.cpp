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
	RCC->APB2ENR |= 0x01<<2; //启动PA时钟
	GPIOA->CRL &= 0xFFFF0000; //PA低4位配置为模拟输入
	RCC->APB2ENR |= 0x01<<9; //启动ADC1时钟
	RCC->CFGR &= ~(0x03<<14); //清除ADC分频系数
	for(int i=3;i;i--); //延迟一会
	RCC->CFGR |= 0x02<<14; //ADC时钟6分频(F=12MHz)
	for(int i=3;i;i--); //延迟一会	
	ADC1->JOFR1 = 0x00; //注入通道1数据偏移0
	ADC1->JOFR2 = 0x00; //注入通道2数据偏移0
	ADC1->JOFR3 = 0x00; //注入通道3数据偏移0
	ADC1->JOFR4 = 0x00; //注入通道4数据偏移0
	ADC1->JSQR  =0x03<<20; //注入通道序列长度4
	ADC1->JSQR |=0x00<<0; //转换1使用通道0
	ADC1->JSQR |=0x01<<5; //转换2使用通道1
	ADC1->JSQR |=0x02<<10; //转换3使用通道2
	ADC1->JSQR |=0x03<<15; //转换4使用通道3
	ADC1->SMPR2  =0x07<<0; //通道0采样239.5周期
	ADC1->SMPR2 |=0x07<<3; //通道1采样239.5周期
	ADC1->SMPR2 |=0x07<<6; //通道2采样239.5周期
	ADC1->SMPR2 |=0x07<<9; //通道3采样239.5周期
	ADC1->CR1  =0x01<<8; //使用扫描模式
	ADC1->CR2  =0x01<<1; //使用连续转换模式
	ADC1->CR1 |=0x01<<10; //开启自动的注入通道组转换
	ADC1->CR2 |=0x01<<0; //唤醒模数转换器
	ADC1->CR2 |=0x01<<3; //初始化复位校准
	while(ADC1->CR2&0x08); //等待复位校准完成
	ADC1->CR2 |= 0x01<<2; //初始化AD校准
	while(ADC1->CR2&0x04); //等待AD校准完成
	ADC1->CR2 |= 0x01<<0; //启动转换
//	ADC1->CR2 |= 0x01<<0; //启动转换
	
}

u16 ANALOG::GetTemperatureExtruderA(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[0][i] = TemperaturePool[0][i+1];
	
	#ifdef TEMP_EXTRUDERA_100K //使用100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR1]; //查表转换
	#else //使用10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR1]; //查表转换
	#endif	
	
	TemperaturePool[0][TEMP_POOL_NUMBER-1] = Temp;
	
//	return Average((s64*)TemperaturePool[0]);	
	return Convolution(FILTER,(s64*)TemperaturePool[0]);

}

u16 ANALOG::GetTemperatureExtruderB(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[1][i] = TemperaturePool[1][i+1];
	TemperaturePool[1][TEMP_POOL_NUMBER-1] = (u16)ADC1->JDR2;
	
	#ifdef TEMP_EXTRUDERB_100K //使用100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR2]; //查表转换
	#else //使用10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR2]; //查表转换
	#endif	
	
	TemperaturePool[1][TEMP_POOL_NUMBER-1] = Temp;

//	return Average((s64*)TemperaturePool[1]);	
	return Convolution(FILTER,(s64*)TemperaturePool[1]);
}

u16 ANALOG::GetTemperatureBasePlane(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[2][i] = TemperaturePool[2][i+1];
	TemperaturePool[2][TEMP_POOL_NUMBER-1] = (u16)ADC1->JDR3;

	#ifdef TEMP_BASEPLANE_100K //使用100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR3]; //查表转换
	#else //使用10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR3]; //查表转换
	#endif	
	
	TemperaturePool[2][TEMP_POOL_NUMBER-1] = Temp;

//	return Average((s64*)TemperaturePool[2]);	
	return Convolution(FILTER,(s64*)TemperaturePool[2]);
}

u16 ANALOG::GetTemperatureEnvironment(){
	for(u32 i=0;i<TEMP_POOL_NUMBER-1;i++)
		TemperaturePool[3][i] = TemperaturePool[3][i+1];
	TemperaturePool[3][TEMP_POOL_NUMBER-1] = (u16)ADC1->JDR4;
	
	#ifdef TEMP_ENVIRONMENT_100K //使用100K NTC
		u32 Temp = Temp_Table_100K[(u16)ADC1->JDR4]; //查表转换
	#else //使用10K NTC
		u32 Temp = Temp_Table_10K[(u16)ADC1->JDR4]; //查表转换
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
