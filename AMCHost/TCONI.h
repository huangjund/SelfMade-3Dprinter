#include "stm32f10x.h"

//ÎÂ¶È¿ØÖÆ·â×°
class TCONI{
	public:
		char GetTemperatureExtruderA();
		char GetTemperatureExtruderB();
		char GetTemperatureBasePlane();
		char GetTemperatureEnvironment();
	
		char SetTemperatureEASetting(u16 T);
		char SetTemperatureEBSetting(u16 T);
		char SetTemperatureBPSetting(u16 T);
	
		char GetTemperatureEASetting();
		char GetTemperatureEBSetting();
		char GetTemperatureBPSetting();
	
		char SetCoolerPowerExtruderA(u8 P);
		char SetCoolerPowerExtruderB(u8 P);
	
		char GetCoolerPowerExtruderA();
		char GetCoolerPowerExtruderB();
	
		char GetHeaterPowerExtruderA();
		char GetHeaterPowerExtruderB();
		char GetHeaterPowerBasePlane();
		
		char GetStatusExtruderA();           
		char GetStatusExtruderB();
		char GetStatusBasePlane();
			
		bool TestLink();
	private:
		char* Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign);
		double Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign);
	
};

extern class TCONI Tconi;



