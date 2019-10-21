#include "stm32f10x.h"

class PWM{
	public:
		void Init();
		void SetPWMHeaterExtruderA(u32 DC); //���ü�����A������ռ�ձ�
		void SetPWMHeaterExtruderB(u32 DC); //���ü�����B������ռ�ձ�
		void SetPWMHeaterBasePlane(u32 DC); //���û�׼�������ռ�ձ�
		void SetPWMCoolerExtruderA(u32 DC); //���ü�����A��ȴ��ռ�ձ�
		void SetPWMCoolerExtruderB(u32 DC); //���ü�����B��ȴ��ռ�ձ�
	
		u32 GetPWMHeaterExtruderA(); //��ȡ������A������ռ�ձ�
		u32 GetPWMHeaterExtruderB(); //��ȡ������B������ռ�ձ�
		u32 GetPWMHeaterBasePlane(); //��ȡ��׼�������ռ�ձ�
		u32 GetPWMCoolerExtruderA(); //��ȡ������A��ȴ��ռ�ձ�
		u32 GetPWMCoolerExtruderB(); //��ȡ������B��ȴ��ռ�ձ�
};

extern class PWM Digital_Output;
