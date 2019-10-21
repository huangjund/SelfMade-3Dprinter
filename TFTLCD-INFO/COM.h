#include "stm32f10x.h"

/*
//Address I
Σ=76
  0    1     4     7     10    13    16    19    22    25    28    31    34    37    40  41  42
  %    III   ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   *   *   *  
  -    ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   -   -   -  
  |     |     |     |     |     |     |     |     |     |     |     |     |     |    |   |   |  
Start  Addr   |    TEB    |    TEAS   |    TBPS   |    PEBH   |    PEAC   |    TENV  |  SEB  |  
             TEA         TBP         TEBS        PEAH        PBPH        PEBC       SEA     SBP
			 
 43    46    49    52    55    58    61    64    67    70    73  74  75
 ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   *   *   *
 ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   -   -   -
  |     |     |     |     |     |     |     |     |     |    |   |   |
  |     PY    |    PXM    |    PZM    |     L2    |    L2M   |  IDP RST
  PX          PZ         PYM          L1         L1M        SFA

*/


class COM{
	public:
		void Init(); //初始化
		void Decoder(); //译码并执行
		
		u8 TempBuffer[128];
		
		u8 Buffer[128];
		u8 DataNumber;
		bool RxFinish;
		u8 DataCounter;

		bool TestAddress();	
	
		void Tx(u8 data);	
	
	private:
		u32 C2Q(u8 *data);
	

};

extern class COM Serial;

