#include "stm32f10x.h"

#ifndef _CARD_H
	#define _CARD_H
	
	class CARD{
		public:
			void Init(); //接口初始化
			u8 ReadWrite(u8 Data,bool Read); //基本读写接口
			bool WaitForReady(u8 Select); //等待单元准备好
			u8 ReadCapacity(u8 Select); //获取单元容量
			void WriteEnable(u8 Select); //单元允许写入
			void WriteDisable(u8 Select); //单元禁止写入
			void GetStatus(); //获取存储卡信息
			bool GetAddress(u32 Index,u8 &Card,u32 &Address); //计算逻辑扇区物理地址
		
			void SectorErase(u8 Select,u32 Address); //擦除一个扇区
			
			bool ReadData(u32 Address,u8 *Data,u32 Length); //读取一定长度数据
			bool WritePage(u32 Address,u8 *Data,u32 Length); //页写
			bool ReadSector(u32 Index,u8 *Data); //读取一个扇区
			bool WriteSector(u32 Index,u8 *Data); //写入一个扇区
			u8 GetVolume(); //获取存储卡容量
			
		
			struct{
				bool Connected[8]; //单元在线
				u8 Capacity[8]; //单元容量
				u32 SectorNumber[8]; //单元总物理扇区数
				u32 StartSector[8]; //单元起始逻辑扇区号
			}Status; //存储卡状态
			
	};
	
	extern CARD Card;

#endif

