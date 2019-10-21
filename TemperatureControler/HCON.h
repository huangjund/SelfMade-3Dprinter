#include "stm32f10x.h"

class HCON{
	public:
		void Init();

		void SetTemperatureExtruderA(u32 T); //设定挤出机A预定温度
		void SetTemperatureExtruderB(u32 T); //设定挤出机B预定温度
		void SetTemperatureBasePlane(u32 T); //设定基准面预定温度
	
		u32 GetExtruderASetting(); //读取挤出机A设定温度
		u32 GetExtruderBSetting(); //读取挤出机B设定温度
		u32 GetBasePlaneSetting(); //读取基准面设定温度
	
		u32 GetPowerHeaterExtruderA(); //读取挤出机A加热器功率
		u32 GetPowerHeaterExtruderB(); //读取挤出机BA加热器功率
		u32 GetPowerHeaterBasePlane(); //读取基准面加热器功率
	
	
		char GetStatusExtruderA(); //读取挤出机A状态
		char GetStatusExtruderB(); //读取挤出机B状态
		char GetStatusBasePlane(); //读取基准面状态
	
		void CaclStatusExtruderA(); //计算挤出机A状态
		void CaclStatusExtruderB(); //计算挤出机B状态
		void CaclStatusBasePlane(); //计算基准面状态
		
		void CaclPWMExtruderA(); //计算挤出机A占空比
		void CaclPWMExtruderB(); //计算挤出机B占空比
		void CaclPWMBasePlane(); //计算基准面占空比
		
	private:
		u32 TemperatureExtruderASetting; //挤出机A设定温度
		u32 TemperatureExtruderBSetting; //挤出机B设定温度
		u32 TemperatureBasePlaneSetting; //基准面设定温度
		
		char StatusExtruderA; //挤出机A状态
		char StatusExtruderB; //挤出机B状态
		char StatusBasePlane; //基准面状态
	

};

extern class HCON HeaterControler;
