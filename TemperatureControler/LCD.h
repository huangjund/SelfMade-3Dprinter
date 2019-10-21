#include "stm32f10x.h"



/*
  %    ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   ***   *   *   *
  -    ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   -   -   -
  |     |     |     |     |     |     |     |     |     |     |     |     |    |   |   |
Start   |    TEB    |    TEAS   |    TBPS   |    PEBH   |    PEAC   |    TENV  |  SEB  |
       TEA         TBP         TEBS        PEAH        PBPH        PEBC       SEA     SBP



*/

class LCD{
	public:
		void Init();
		void SentStatus();
	private:
		void Print(u8 *data);
		void Tx(u8 data);
		void Tx_num(u32 data);
};

extern class LCD Lcd;

