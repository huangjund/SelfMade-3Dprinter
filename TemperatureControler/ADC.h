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
		void Init(); //��ʼ��
		u16 GetTemperatureExtruderA(); //��ü�����A�¶�
		u16 GetTemperatureExtruderB(); //��ü�����B�¶�
		u16 GetTemperatureBasePlane(); //��û�׼���¶�
		u16 GetTemperatureEnvironment(); //��û����¶�
	
		u64 Convolution(s64 H[],s64 X[]); //���Ծ�� (X*H)[0]
	
		s64 Average(s64 X[]);
	
		u64 TemperaturePool[4][TEMP_POOL_NUMBER];
};

extern class ANALOG Analog_Input;

