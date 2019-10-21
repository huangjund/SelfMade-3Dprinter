#include "stm32f10x.h"

class STATUS{ //状态
	public:

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
		

		double PosiX; //X轴当前坐标
		double PosiY; //Y轴当前坐标
		double PosiZ; //Z轴当前坐标
		double PosiA;
		double PosiB;
		
		double PosiXMax; //X轴最大坐标
		double PosiYMax; //Y轴最大坐标
		double PosiZMax; //Z轴最大坐标
		u16 L1Reset; //L1缓存剩余容量
		u16 L2Reset; //L2缓存剩余容量
		u16 L1Max; //L1缓存最大容量
		u16 L2Max; //L2缓存最大容量

		//E--调平 D--未调平
		char StateLeveling;
		
		double TestPosition[5];

		//D--失能 N--未归位 S--切换(Del) W--工作 I--空闲
		char StateFA; //自动机状态
		
		//E--启用 D--禁用
		char StateIDP; //IDP状态
		
		//E--启用 D--禁用
		char StateStepper; //IDP状态
		
		//E--完成 D--未完成
		char StateHomed; 
		
		//E--开启 D--关闭
		char StateSwitchA;
		
		//E--开启 D--关闭
		char StateSwitchB;
		
		char StatusBar[38];
		
		bool LinkToScon;
		
		bool LinkToTcon;
		
		bool LinkToFcon;
		
		bool LinkToCcon;
		
		bool Manufacturing;
		
		bool Running;
};

extern class STATUS Status;
