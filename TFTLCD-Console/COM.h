#include "stm32f10x.h"

/*
Address = C

  0   1    4   5   6
  %   III  *   *   *************************************
  -   ---  -   -   -------------------------------------
  |    |   |   |                     |    
Start Addr |  COM                    |
          ADD                      STATUS

         
*/

class COM{
	public:
		void Init(); //初始化
		void Decoder(); //译码并执行

		u8 TempBuffer[64];
	  u8 DataCounter;	
	
		u8 Buffer[64];
		u8 DataNumber;	
	
		bool RxFinish;

		bool TestAddress();
	
		void Tx(u8 data);
};

extern class COM Serial;

