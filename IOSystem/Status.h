#include "stm32f10x.h"

class STATUS{
	public:
		void Init();
		
		void SetInfo(u8 Buffer[],u32 DataNumber);
	
		u32 C2Q(u8 *data);
		
		u16 TempExtruderA; //挤出机A当前温度
		u16 TempExtruderB; //挤出机B当前温度
		u16 TempBasePlane; //基准面当前温度
		u16 TempAir; //环境温度
		u16 TempExtruderASet; //挤出机A设定温度
		u16 TempExtruderBSet; //挤出机B设定温度
		u16 TempBasePlaneSet; //基准面设定温度
		u8 PowerHeaterExtruderA; //挤出机A加热功率
		u8 PowerHeaterExtruderB; //挤出机B加热功率
		u8 PowerHeaterBasePlane; //基准面加热功率
		u8 PowerCoolerExtruderA; //挤出机A冷却功率
		u8 PowerCoolerExtruderB; //挤出机B冷却功率
		//H--加热 C--冷却 M--保持 O--关闭
		char StateExtruderA; //挤出机A状态
		char StateExtruderB; //挤出机B状态
		char StateBasePlane; //基准面状态

		u16 PosiX; //X轴当前坐标
		u16 PosiY; //Y轴当前坐标
		u16 PosiZ; //Z轴当前坐标
		u16 PosiXMax; //X轴最大坐标
		u16 PosiYMax; //Y轴最大坐标
		u16 PosiZMax; //Z轴最大坐标
		u16 L1Used; //L1缓存已使用容量
		u16 L2Used; //L2缓存已使用容量
		u16 L1Max; //L1缓存最大容量
		u16 L2Max; //L2缓存最大容量

		//D--失能 N--未归位 S--切换(Del) W--工作 I--空闲
		char StateFA; //自动机状态
		
		//E--启用 D--禁用
		char StateIDP; //IDP状态
		
		char StateBar[38]; //状态栏
};


extern class STATUS Status;

