#include "stm32f10x.h"

class CCON{
	public:
		void Init();
		void SetPowerCoolerExtruderA(u32 P); //设置挤出机A冷却器功率
		void SetPowerCoolerExtruderB(u32 P); //设置挤出机B冷却器功率
	
		u32 GetPowerCoolerExtruderA(); //读取挤出机A冷却器功率
		u32 GetPowerCoolerExtruderB(); //读取挤出机B冷却器功率
};

extern class CCON CoolerControler;
