#include "stm32f10x.h"

class SCONI{
	public:
		char Motion(double Position[5],float Vmax); //移动指令接口
		char InitPosition(double Position[5]); //仅初始化位置
		char AutoHoming(); //自动复位
	
		char MoveAxis(double Position,u8 Selection); //移动轴
	
		char SetNozzleState(u8 ExtA,u8 ExtB); //设置喷头状态
	
		bool GetStatusBusy(); //测试后台是否忙
	
		char GetTestPointA(); //获取测试点A数据
		char GetTestPointB(); //获取测试点B数据
		char GetTestPointC(); //获取测试点C数据
		char GetTestPointD(); //获取测试点D数据
		char GetTestPointM(); //获取测试点M数据
	
		char GetPositionX();
		char GetPositionY();
		char GetPositionZ();
		char GetPositionA();
		char GetPositionB();
	
		char GetPositionXMax();	
		char GetPositionYMax();	
		char GetPositionZMax();	
	
		char GetStatusNozzleA(); //获取喷头A状态
		char GetStatusNozzleB(); //获取喷头B状态
	
		char GetStatusFA();
		char GetStatusIDP();
	
		char GetStatusStepper();
		char GetStatusHomed();
	
		char GetStatusLeveling();
	
		char GetL1Rest();
		char GetL2Rest();
		char GetL1Max();
		char GetL2Max();
		
		char SetPosition(double Position,char Select);
		
		char EnableStepper();
		char DisableStepper();
		
		char EnableIDP();
		char DisableIDP();
		
		bool TestLink();
	private:
		char* Flo2Chr(char *Buf,double Num,u32 Int,u32 Dec,bool Sign);
		double Chr2Flo(char *Buf,u32 Int,u32 Dec,bool Sign);
	
};

extern class SCONI Sconi;

