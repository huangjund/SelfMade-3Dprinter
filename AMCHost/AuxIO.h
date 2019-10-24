#include "stm32f10x.h"

class AUXIO{
	public:
		char GetStatusMaterialA();
		char GetStatusMaterialB();
//		char GetStatusSW3();
//		char GetStatusSW4();
//		char GetStatusSW5();
	
		char SetStatusMainLight(bool Bit);
		char SetStatusAuxiliaryLight(bool Bit);
//		char SetStatusSwitch3(bool Bit);
			
		bool TestLink();
};

extern class AUXIO AuxIO;

