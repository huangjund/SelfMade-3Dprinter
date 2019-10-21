/*
#ifndef _INCLUDE_STM32F10X_H
	#define _INCLUDE_STM32F10X_H
	#include "stm32f10x.h"
#endif

#ifndef _INCLUDE_INTERFACE_H
	#define _INCLUDE_INTERFACE_H
	#include "Interface.h"
#endif

#ifndef _INCLUDE_STEPPERCONFIGURATION_H
	#define _INCLUDE_STEPPERCONFIGURATION_H
	#include "StepperConfiguration.h"
#endif

#ifndef _INCLUDE_PLANNER_H
	#define _INCLUDE_PLANNER_H
	#include "Planner.h"
#endif
*/


#ifndef _STATUS_H
	#define _STATUS_H
	#include "stm32f10x.h"
	#include "Interface.h"
	#include "StepperConfiguration.h"
	#include "Planner.h"
	

	class STATUS{
		public:
			void Init(); //初始化
			bool InitPosition(double Posi[5]); //初始化位置
			bool TestMaxEndPointX(); //X轴终点限位器状态
			bool TestMaxEndPointY(); //Y轴终点限位器状态
			bool TestMaxEndPointZ(); //Z轴终点限位器状态

			bool TestMinEndPointX(); //X轴起点限位器状态
			bool TestMinEndPointY(); //Y轴起点限位器状态
			bool TestMinEndPointZ(); //Z轴起点限位器状态
		
			void AddOneStepToX(bool Direction); //X坐标步进
			void AddOneStepToY(bool Direction); //Y坐标步进
			void AddOneStepToZ(bool Direction); //Z坐标步进
			void AddOneStepToA(bool Direction); //A坐标步进
			void AddOneStepToB(bool Direction); //B坐标步进
			
			void SetPosition(double Posi,u8 Selection); //(仅)设置坐标
		
			double GetPosition(u8 Selection); //读取坐标
			
			double TestPoint[5]; //5个测试点坐标
		

			bool TestIfLevel(); //检测是否调平
			bool TestIfHomed(); //检测是否已经归位
			bool TestIfEnable(); //检测是否已经使能
			bool TestIfIDP(); //检测IDP状态
			
			void SetEnable(bool S); //设置使能标志状态
			void SetIDP(bool S); //设置IDP标志状态
//		private:
			bool Leveling; //调平标志
			bool IDP; //智能动态规划
			bool Enable; //电机使能标志			
			double Position[5]; //当前坐标
			bool Homed; //已归位标志
			
			
	};

	extern class STATUS Status;
#endif
