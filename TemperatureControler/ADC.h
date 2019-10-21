#include "stm32f10x.h"

#define TEMP_EXTRUDERA_100K
#define TEMP_EXTRUDERB_100K
//#define TEMP_BASEPLANE_100K
//#define TEMP_ENVIRONMENT_100K
#define TEMP_POOL_NUMBER 5

#define u64 unsigned long long 
#define s64 long long


class ANALOG{
	public:
		void Init(); //初始化
		u16 GetTemperatureExtruderA(); //获得挤出机A温度
		u16 GetTemperatureExtruderB(); //获得挤出机B温度
		u16 GetTemperatureBasePlane(); //获得基准面温度
		u16 GetTemperatureEnvironment(); //获得环境温度
	
		u64 Convolution(s64 H[],s64 X[]); //线性卷积 (X*H)[0]
	
		s64 Average(s64 X[]);
	
		u64 TemperaturePool[4][TEMP_POOL_NUMBER];
};

extern class ANALOG Analog_Input;

