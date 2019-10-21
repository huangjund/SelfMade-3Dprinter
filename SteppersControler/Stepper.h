/*
#ifndef _INCLUDE_STM32F10X_H
	#define _INCLUDE_STM32F10X_H
	#include "stm32f10x.h"
#endif

#ifndef _INCLUDE_STEPPERCONFIGURATION_H
	#define _INCLUDE_STEPPERCONFIGURATION_H
	#include "StepperConfiguration.h"
#endif

#ifndef _INCLUDE_STATUS_H
	#define _INCLUDE_STATUS_H
	#include "Status.h"
#endif

#ifndef _INCLUDE_INTERFACE_H
	#define _INCLUDE_INTERFACE_H
	#include "Interface.h"
#endif
*/
#include "stm32f10x.h"
#include "StepperConfiguration.h"
#include "Status.h"
#include "Interface.h"
#ifndef _STEPPER_H
	#define _STEPPER_H
  #define u64 unsigned long long
	#define s64 long long
	

	/*Automate

																			BufferFull
			 BufferEmpty                    +-------+  
			+----------+                   /         \
		 /            \                 +           +
		+              V                 \         V               
		 \         +------+  BufferFull  +---------+ BufferEmpty  +-----------+
			+--------| Idle |------------->| Working |------------->| Switching |
							 +------+              +---------+              +-----------+
								 A  A                    A                         /|
								/    \                    \      BufferFull       / |
	 +-------+   /      \                    +---------------------+  +
	 | Error |--+        \      BufferEmpty                          /
	 +-------+            +-----------------------------------------+
	 
	*/


	class STEPPER{
		public:
			void Init(); //初始化
			bool AutoHoming(); //自动归位
			void EnableStepper(); //使能所有步进电机
			bool DisableStepper(); //失能所有步进电机	
			
			void SetAllDirection(bool Direction[5]); //根据需求设置方向	
			void GeneratePluse(u8 Selection); //根据需求产生脉冲
			
		
			void TestMove(u8 Selection,bool Direction,double Length); //使用相对坐标
			void SectionMove(double Position,u8 Selection); //使用绝对坐标
			struct _L1{
				bool Direction[5]; //方向
				u64 Step[5]; //步数
				u64 ClockCounter; //节拍计数器
				float NextPluseClock[5]; //下一脉冲到来时刻
				float TPluse[5]; //脉冲节拍周期
				u64 Accelerate; //加速节拍数
				u64 ConstSpeed; //匀速节拍数
				u64 Decelerate; //减速节拍数
				float CycleIn; //切入周期
				float Acceleration; //加速度
				float Deceleration; //负加速度
				float VelocityIn; //切入速度
				float VelocityOut; //切换速度
				bool Full; //已填充标志
				_L1 *Pror; //逆向循环指针
				_L1 *Next; //循环指针
			}L1_Cache[L1_SIZE];
			
			_L1 *PtrLoader;
			_L1 *PtrLauncher;
			
			s64 VirtualStep[3]; //虚位移
			
			u8 L1RestAmount;
			
			char FAStatus; //自动机状态
	};
	

	extern class STEPPER Stepper;

#endif





