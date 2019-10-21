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
			void Init(); //��ʼ��
			bool InitPosition(double Posi[5]); //��ʼ��λ��
			bool TestMaxEndPointX(); //X���յ���λ��״̬
			bool TestMaxEndPointY(); //Y���յ���λ��״̬
			bool TestMaxEndPointZ(); //Z���յ���λ��״̬

			bool TestMinEndPointX(); //X�������λ��״̬
			bool TestMinEndPointY(); //Y�������λ��״̬
			bool TestMinEndPointZ(); //Z�������λ��״̬
		
			void AddOneStepToX(bool Direction); //X���경��
			void AddOneStepToY(bool Direction); //Y���경��
			void AddOneStepToZ(bool Direction); //Z���경��
			void AddOneStepToA(bool Direction); //A���경��
			void AddOneStepToB(bool Direction); //B���경��
			
			void SetPosition(double Posi,u8 Selection); //(��)��������
		
			double GetPosition(u8 Selection); //��ȡ����
			
			double TestPoint[5]; //5�����Ե�����
		

			bool TestIfLevel(); //����Ƿ��ƽ
			bool TestIfHomed(); //����Ƿ��Ѿ���λ
			bool TestIfEnable(); //����Ƿ��Ѿ�ʹ��
			bool TestIfIDP(); //���IDP״̬
			
			void SetEnable(bool S); //����ʹ�ܱ�־״̬
			void SetIDP(bool S); //����IDP��־״̬
//		private:
			bool Leveling; //��ƽ��־
			bool IDP; //���ܶ�̬�滮
			bool Enable; //���ʹ�ܱ�־			
			double Position[5]; //��ǰ����
			bool Homed; //�ѹ�λ��־
			
			
	};

	extern class STATUS Status;
#endif
