#include "stm32f10x.h"

class PWM{
	public:
		void Init();
		void SetPWMHeaterExtruderA(u32 DC); //设置挤出机A加热器占空比
		void SetPWMHeaterExtruderB(u32 DC); //设置挤出机B加热器占空比
		void SetPWMHeaterBasePlane(u32 DC); //设置基准面加热器占空比
		void SetPWMCoolerExtruderA(u32 DC); //设置挤出机A冷却器占空比
		void SetPWMCoolerExtruderB(u32 DC); //设置挤出机B冷却器占空比
	
		u32 GetPWMHeaterExtruderA(); //读取挤出机A加热器占空比
		u32 GetPWMHeaterExtruderB(); //读取挤出机B加热器占空比
		u32 GetPWMHeaterBasePlane(); //读取基准面加热器占空比
		u32 GetPWMCoolerExtruderA(); //读取挤出机A冷却器占空比
		u32 GetPWMCoolerExtruderB(); //读取挤出机B冷却器占空比
};

extern class PWM Digital_Output;
