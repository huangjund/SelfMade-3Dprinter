#include "stm32f10x.h"

#ifndef _LCD_H
	#define _LCD_H
	#define REFRESH_CYCLE 150
#endif


/*
¦²=34
  0    1     4     7     10    13    16    19    22    25    28    31  32  33
  %    ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   *   *   *
  -    ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   -   -   -
  |     |     |     |     |     |     |     |     |     |     |    |   |   |
Start   |     PY    |    PXM    |    PZM    |     L2    |    L2M   |  IDP  |
        PX          PZ         PYM          L1         L1M        SFA     RST

*/

class LCD{
	public:
		void Init();
		void SentStatus();
	
		bool RestartFlag;
	private:
		void Print(u8 *data);
		void Tx(u8 data);
		void Tx_num(u32 data);
};

extern class LCD Lcd;
