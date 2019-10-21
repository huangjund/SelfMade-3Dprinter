#include "stm32f10x.h"

class CCON{
	public:
		void Init();
		void SetPowerCoolerExtruderA(u32 P); //���ü�����A��ȴ������
		void SetPowerCoolerExtruderB(u32 P); //���ü�����B��ȴ������
	
		u32 GetPowerCoolerExtruderA(); //��ȡ������A��ȴ������
		u32 GetPowerCoolerExtruderB(); //��ȡ������B��ȴ������
};

extern class CCON CoolerControler;
