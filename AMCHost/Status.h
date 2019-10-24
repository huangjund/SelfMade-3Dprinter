#include "stm32f10x.h"

class STATUS{ //状态
	public:

	//Temperature Controler
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
		
	//Motion Controler
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
		
		double TestPosition[5];
		
		bool NozzleState[2];
		
		//E--调平 D--未调平
		char StateLeveling;
		//D--失能 N--未归位 S--切换(Del) W--工作 I--空闲
		char StateFA; //自动机状态
		//E--启用 D--禁用
		char StateIDP; //IDP状态
		//E--启用 D--禁用
		char StateStepper; //步进电机状态
		//E--完成 D--未完成
		char StateHomed; //复位状态
		
	//FileSystem Controler	
		u8 CardTotalVolume; //存储卡总容量 [MB]
		u32 CardRestVolume; //存储卡剩余容量 [B]
		bool FileExist; //文件存在性
		char FileName[32]; //文件名
		u32 FileSize; //文件大小 [B]
		u32 CommandNumber; //文件指令数
		u32 EstimateTime; //文件预计时间 [min]
		u32 TotalDistance; //文件总行程 [cm]
		u32 FileInfo1; //文件附加信息1
		u32 FileInfo2; //文件附加信息2
		u32 FileInfo3; //文件附加信息3
		u32 FileInfo4; //文件附加信息4
		u32 FileInfo5; //文件附加信息5
		u32 FileInfo6; //文件附加信息6
		u32 FilePointer; //文件指针
		
	//Console
		//E--开启 D--关闭
		char StateSwitchA;
		//E--开启 D--关闭
		char StateSwitchB;
		// *************************************
		// T A S F M
		char StatusBar[38];
	//时钟系统
		u8 YMD[3];
		u8 HMS[3];
	
	//总线系统
		bool LinkToScon; //SC链路状态
		bool LinkToTcon; //TC链路状态
		bool LinkToFcon; //FC链路状态
		bool LinkToCcon; //CC链路状态
		bool LinkToAux1; //A1链路状态
		bool LinkToAux2; //A2链路状态
	//生产系统
		bool MaterialA; //材料A状态
		bool MaterialB; //材料B状态		
		
};

extern class STATUS Status;
