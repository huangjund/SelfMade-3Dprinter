#include "stm32f10x.h"

class RTCLOCK{
	public:
		void Init();
		void SetTime();
		void GetTime();
		
	private:
		bool Write(u8 Data,u8 Address);
		bool Read(u8 &Data,u8 Address);
	
		void WriteData(u8 Data,u8 Address);
		u8 ReadData(u8 Address);
		bool WriteEnable();
		bool WriteDisable();
	
		void IIC_SendByte(u8 Data);
		u8 IIC_ReadByte(bool Ack);
		void IIC_Init();
		void IIC_Start();
		void IIC_Stop();
		void IIC_Ack();
		void IIC_NAck();
		bool IIC_WaitAck();
		void DelayUs(u8 Us);
};

extern class RTCLOCK RTClock;

