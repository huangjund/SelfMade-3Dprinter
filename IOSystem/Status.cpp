#include "Status.h"

STATUS Status;

void STATUS::Init(){
	TempExtruderA = 0; //挤出机A当前温度
	TempExtruderB = 0; //挤出机B当前温度
	TempBasePlane = 0; //基准面当前温度
	TempAir = 0; //环境温度
	TempExtruderASet = 0; //挤出机A设定温度
	TempExtruderBSet = 0; //挤出机B设定温度
	TempBasePlaneSet = 0; //基准面设定温度
	PowerHeaterExtruderA = 0; //挤出机A加热功率
	PowerHeaterExtruderB = 0; //挤出机B加热功率
	PowerHeaterBasePlane = 0; //基准面加热功率
	PowerCoolerExtruderA = 0; //挤出机A冷却功率
	PowerCoolerExtruderB = 0; //挤出机B冷却功率
	//H--加热 C--冷却 M--保持 O--关闭
	StateExtruderA = 'O'; //挤出机A状态
	StateExtruderB = 'O'; //挤出机B状态
	StateBasePlane = 'O'; //基准面状态

	PosiX = 0; //X轴当前坐标
	PosiY = 0; //Y轴当前坐标
	PosiZ = 0; //Z轴当前坐标
	PosiXMax = 0; //X轴最大坐标
	PosiYMax = 0; //Y轴最大坐标
	PosiZMax = 0; //Z轴最大坐标
	L1Used = 0; //L1缓存已使用容量
	L2Used = 0; //L2缓存已使用容量
	L1Max = 0; //L1缓存最大容量
	L2Max = 0; //L2缓存最大容量

	//D--失能 N--未归位 S--切换(Del) W--工作 I--空闲
	StateFA = 'D'; //自动机状态
	
	//E--启用 D--禁用
	StateIDP = 'D'; //IDP状态
	
	for(u32 i=0;i<37;i++)
		StateBar[i] = ' '; //状态栏
	StateBar[37] = 0;
}

void STATUS::SetInfo(u8 Buffer[],u32 DataNumber){
	if(DataNumber < 3) return;
	Status.TempExtruderA = C2Q(Buffer);
	if(Status.TempExtruderA > 320)
		Status.TempExtruderA = 320;	
	
	if(DataNumber < 6) return;
	Status.TempExtruderB = C2Q(Buffer+3);
	if(Status.TempExtruderB > 320)
		Status.TempExtruderB = 320;	
	
	if(DataNumber < 9) return;
	Status.TempBasePlane = C2Q(Buffer+6);
	if(Status.TempBasePlane > 150)
		Status.TempBasePlane = 150;
	
	if(DataNumber < 12) return;
	Status.TempExtruderASet = C2Q(Buffer+9);
	if(Status.TempExtruderASet > 320)
		Status.TempExtruderASet = 320;	
	
	if(DataNumber < 15) return;
	Status.TempExtruderBSet = C2Q(Buffer+12);
	if(Status.TempExtruderBSet > 320)
		Status.TempExtruderBSet = 320;	
	
	if(DataNumber < 18) return;
	Status.TempBasePlaneSet = C2Q(Buffer+15);
	if(Status.TempBasePlaneSet > 150)
		Status.TempBasePlaneSet = 150;
	
	if(DataNumber < 21) return;
	Status.PowerHeaterExtruderA = C2Q(Buffer+18);
	if(Status.PowerHeaterExtruderA > 100)
		Status.PowerHeaterExtruderA = 100;	
	
	if(DataNumber < 24) return;
	Status.PowerHeaterExtruderB = C2Q(Buffer+21);
	if(Status.PowerHeaterExtruderB > 100)
		Status.PowerHeaterExtruderB = 100;	
	
	if(DataNumber < 27) return;
	Status.PowerHeaterBasePlane = C2Q(Buffer+24);
	if(Status.PowerHeaterBasePlane > 100)
		Status.PowerHeaterBasePlane = 100;	
	
	if(DataNumber < 30) return;
	Status.PowerCoolerExtruderA = C2Q(Buffer+27);
	if(Status.PowerCoolerExtruderA > 100)
		Status.PowerCoolerExtruderA = 100;	
	
	if(DataNumber < 33) return;
	Status.PowerCoolerExtruderB = C2Q(Buffer+30);
	if(Status.PowerCoolerExtruderB > 100)
		Status.PowerCoolerExtruderB = 100;
	
	if(DataNumber < 36) return;
	Status.TempAir = C2Q(Buffer+33);
	if(Status.TempAir > 100)
		Status.TempAir = 100;
	
	if(DataNumber < 37) return;
	Status.StateExtruderA = (char)Buffer[36];
	
	if(DataNumber < 38) return;
	Status.StateExtruderB = (char)Buffer[37];
	
	if(DataNumber < 19) return;
	Status.StateBasePlane = (char)Buffer[38];
	
	if(DataNumber < 42) return;
	Status.PosiX = C2Q(Buffer+39);
	
	if(DataNumber < 45) return;
	Status.PosiY = C2Q(Buffer+42);
	
	if(DataNumber < 48) return;
	Status.PosiZ = C2Q(Buffer+45);
	
	if(DataNumber < 51) return;
	Status.PosiXMax = C2Q(Buffer+48);
	
	if(DataNumber < 54) return;
	Status.PosiYMax = C2Q(Buffer+51);
	
	if(DataNumber < 57) return;
	Status.PosiZMax = C2Q(Buffer+54);
	
	if(Status.PosiX > Status.PosiXMax)
		Status.PosiX = Status.PosiXMax;
	if(Status.PosiY > Status.PosiYMax)
		Status.PosiY = Status.PosiYMax;
	if(Status.PosiZ > Status.PosiZMax)
		Status.PosiZ = Status.PosiZMax;
	
	if(DataNumber < 60) return;
	Status.L1Used = C2Q(Buffer+57);
	
	if(DataNumber < 63) return;
	Status.L2Used = C2Q(Buffer+60);
	
	if(DataNumber < 66) return;
	Status.L1Max = C2Q(Buffer+63);
	
	if(DataNumber < 69) return;
	Status.L2Max = C2Q(Buffer+66);
	
	if(Status.L1Used > Status.L1Max)
		Status.L1Used = Status.L1Max;
	if(Status.L2Used > Status.L2Max)
		Status.L2Used = Status.L2Max;
	
	if(DataNumber < 70) return;
	Status.StateFA = (char)Buffer[69];
	
	if(DataNumber < 71) return;
	Status.StateIDP = (char)Buffer[70];
	
}


u32 STATUS::C2Q(u8 *data){
	static u32 weight[] = {1,1,10,100};
	u32 value = 0;
	for(u8 i=0;i<3;i++) //转换为数字
		value += (data[i]-'0')*weight[3-i];
	return value;
}
