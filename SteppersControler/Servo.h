#include "stm32f10x.h"

#ifndef _SERVO_H
	#define _SERVO_H
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

class SERVO{
	public:
		void Init();
		void EnableProbe();
		void DisableProbe();

		void SetPWM(u8 PWM);
		u32 PWM;
		u32 Counter;
	
		void DelayMs(u32 n);

};

extern class SERVO Servo;
