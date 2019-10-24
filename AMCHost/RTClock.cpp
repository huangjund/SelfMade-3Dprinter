#include "RTClock.h"
#include "Status.h"

RTCLOCK RTClock;

inline void IIC_SDA(bool State){
	if(State) GPIOA->BSRR |= 0x02;
	else GPIOA->BRR |= 0x02;
}
inline void IIC_SCL(bool State){
	if(State) GPIOA->BSRR |= 0x01;
	else GPIOA->BRR |= 0x01;
}
inline bool IIC_GetSDA(){
	if(GPIOA->IDR & 0x02) return 1;
	else return 0;
}

void RTCLOCK::Init(){
	RCC->APB2ENR |= 0x01<<2;
	GPIOA->CRL &= 0xFFFFFF00;
	GPIOA->CRL |= 0x00000037;
	
	IIC_Init();
	Write(0x00,0x0E);
	Write(0x82,0x18);
	GetTime();
}

void RTCLOCK::GetTime(){
	u8 Second = 0x00;
	Read(Second,0x00);
	Status.HMS[2] = Second&0x0F + ((Second&0xF0)>>4)*10;
	
	u8 Minute = 0x00;
	Read(Minute,0x01);
	Status.HMS[1] = Minute&0x0F + ((Minute&0xF0)>>4)*10;
	
	u8 Hour = 0x00;
	Read(Hour,0x02);
	if(Hour & 0x80){ //24H
		Status.HMS[0] = Hour&0x0F + ((Hour&0x70)>>4)*10;		
	}
	else{ //12H
		Hour = Hour&0x0F + ((Hour&0x70)>>4)*10;
		if(Hour>=1 && Hour<=11) Status.HMS[0] = Hour;
		else if(Hour>=21 && Hour<=31) Status.HMS[0] = Hour-8;
		else if(Hour == 32) Status.HMS[0] = 12;
		else Status.HMS[0] = 0;
	}

	u8 Day = 0x00;
	Read(Day,0x04);
	Status.YMD[2] = Day&0x0F + ((Day&0xF0)>>4)*10;
	
	u8 Month = 0x00;
	Read(Month,0x05);
	Status.YMD[1] = Month&0x0F + ((Month&0xF0)>>4)*10;
	
	u8 Year = 0x00;
	Read(Year,0x06);
	Status.YMD[2] = Year&0x0F + ((Year&0xF0)>>4)*10;
}

void RTCLOCK::SetTime(){
}

bool RTCLOCK::Write(u8 Data,u8 Address){
	for(u8 i=0;i<5;i++){
		if(!WriteEnable()) continue;
		WriteData(Data,Address);
		u8 Temp = ReadData(Address);
		if(Temp != Data) continue;
		if(WriteDisable()) return 1;
	}
	WriteDisable();
	return 0;
}

bool RTCLOCK::Read(u8 &Data,u8 Address){
	for(u8 i=0;i<5;i++){
		u8 Temp = ReadData(Address);
		if(Temp == ReadData(Address)){
			Data = Temp;
			return 1;
		}
	}
	Data = 0x00;
	return 0;
}

void RTCLOCK::WriteData(u8 Data,u8 Address){
	IIC_Start(); //开始
	IIC_SendByte(0x64); //写模式
	IIC_WaitAck(); //等待应答
	IIC_SendByte(Address); //内部地址
	IIC_WaitAck(); //等待应答
	IIC_SendByte(Data); //
	IIC_WaitAck(); //等待应答
	IIC_Stop(); //停止
}

u8 RTCLOCK::ReadData(u8 Address){
	IIC_Start(); //开始
	IIC_SendByte(0x64); //写模式
	IIC_WaitAck(); //等待应答
	IIC_SendByte(Address); //内部地址
	IIC_WaitAck(); //等待应答
	IIC_Start(); //重新开始
	IIC_SendByte(0x65); //读模式
	IIC_WaitAck(); //等待应答
	u8 Data = IIC_ReadByte(1); //读取数据
	IIC_Stop(); //停止
	return  Data;
}

bool RTCLOCK::WriteEnable(){
	WriteData(0x80,0x10);
	WriteData(0x84,0x0F);
	u8 CTR2 = ReadData(0x10);
	u8 CTR1 = ReadData(0x0F);
	if(CTR2&0x80 && CTR1&0x84)
		return 1;
	else
		return 0;
}

bool RTCLOCK::WriteDisable(){
	WriteData(0x00,0x0F);	
	WriteData(0x00,0x10);
	u8 CTR2 = ReadData(0x10);
	u8 CTR1 = ReadData(0x0F);
	if(CTR2&0x80 || CTR1&0x84)
		return 0;
	else
		return 1;
}

void RTCLOCK::IIC_Init(){
		IIC_SCL(1);
		IIC_SDA(1);
}

void RTCLOCK::IIC_SendByte(u8 Data){
	IIC_SDA(1);
	IIC_SCL(0);
	for(u8 i=0;i<8;i++){
		if(Data & 0x80) IIC_SDA(1);
		else IIC_SDA(0);
		Data >>= 1;
		DelayUs(2);
		IIC_SCL(1);
		DelayUs(2);
		IIC_SCL(0);
		DelayUs(2);
	}
}

u8 RTCLOCK::IIC_ReadByte(bool Ack){
	IIC_SDA(1);
	u8 Data = 0;
	for(u8 i=0;i<8;i++){
		IIC_SCL(0);
		DelayUs(2);
		IIC_SCL(1);
		Data <<= 1;
		if(IIC_GetSDA()) Data |= 0x01;
		DelayUs(2);
	}
	if(!Ack) IIC_NAck();
	else IIC_Ack();
	return Data;
}

void RTCLOCK::IIC_Start(){
	//SDA ----+
	//        +-------
	//
	//SCL----------+
	//             +--
	IIC_SDA(1);
	IIC_SCL(1);
	DelayUs(4);
	IIC_SDA(0);
	DelayUs(4);
	IIC_SCL(0);
}

void RTCLOCK::IIC_Stop(){
	//SDA     +----+
	//    ----+    +----
	//
	//SCL     +----+
	//    ----+    +----
	IIC_SCL(0);
	IIC_SDA(0);
	DelayUs(4);
	IIC_SCL(1);
	IIC_SDA(1);
	DelayUs(4);
}

void RTCLOCK::IIC_Ack(){
	//SDA   
	//    ---------
	//
	//SCL   +--+
	//    --+  +---
	IIC_SCL(0);
	IIC_SDA(0);
	DelayUs(2);
	IIC_SCL(1);
	DelayUs(2);
	IIC_SCL(0);
}

void RTCLOCK::IIC_NAck(){
	//SDA ---------  
	//    
	//
	//SCL   +--+
	//    --+  +---
	IIC_SCL(0);
	IIC_SDA(1);
	DelayUs(2);
	IIC_SCL(1);
	DelayUs(2);
	IIC_SCL(0);
}

bool RTCLOCK::IIC_WaitAck(){
	IIC_SDA(1);
	DelayUs(1);
	IIC_SCL(1);
	DelayUs(1);
	while(IIC_GetSDA()){
		for(u8 Try=0;Try<255;Try++){
			if(Try > 250){
				IIC_Stop();
				return 1;
			}
		}
	}
	IIC_SCL(0);
	return 0;
}

void RTCLOCK::DelayUs(u8 Us){
	SysTick->LOAD = 72*Us;
	SysTick->CTRL = 0x00000005;
	while(!(SysTick->CTRL & 0x00010000));
	SysTick->CTRL = 0x00000004;
}

