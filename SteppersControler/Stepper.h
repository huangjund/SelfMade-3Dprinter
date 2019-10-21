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
			void Init(); //��ʼ��
			bool AutoHoming(); //�Զ���λ
			void EnableStepper(); //ʹ�����в������
			bool DisableStepper(); //ʧ�����в������	
			
			void SetAllDirection(bool Direction[5]); //�����������÷���	
			void GeneratePluse(u8 Selection); //���������������
			
		
			void TestMove(u8 Selection,bool Direction,double Length); //ʹ���������
			void SectionMove(double Position,u8 Selection); //ʹ�þ�������
			struct _L1{
				bool Direction[5]; //����
				u64 Step[5]; //����
				u64 ClockCounter; //���ļ�����
				float NextPluseClock[5]; //��һ���嵽��ʱ��
				float TPluse[5]; //�����������
				u64 Accelerate; //���ٽ�����
				u64 ConstSpeed; //���ٽ�����
				u64 Decelerate; //���ٽ�����
				float CycleIn; //��������
				float Acceleration; //���ٶ�
				float Deceleration; //�����ٶ�
				float VelocityIn; //�����ٶ�
				float VelocityOut; //�л��ٶ�
				bool Full; //������־
				_L1 *Pror; //����ѭ��ָ��
				_L1 *Next; //ѭ��ָ��
			}L1_Cache[L1_SIZE];
			
			_L1 *PtrLoader;
			_L1 *PtrLauncher;
			
			s64 VirtualStep[3]; //��λ��
			
			u8 L1RestAmount;
			
			char FAStatus; //�Զ���״̬
	};
	

	extern class STEPPER Stepper;

#endif





