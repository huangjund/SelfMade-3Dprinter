#include "stm32f10x.h"


class SPIN{
	public:
		void Init();
		
		void EnableRx();
		void DisableRx();
	
		void Load(u8 Data);
		u8 Launch();
	
	
		u8 Buffer[64];
		u8 DataNumber;
		u8 LoadPtr;
		u8 LauncherPtr;
		
		bool RxEnableFlag;
};

extern class SPIN Spin;
