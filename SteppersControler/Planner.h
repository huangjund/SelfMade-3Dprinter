/*
#ifndef _INCLUDE_STM32F10X_H
	#define _INCLUDE_STM32F10X_H
	#include "stm32f10x.h"
#endif

#ifndef _INCLUDE_STEPPER_H
	#define _INCLUDE_STEPPER_H
	#include "Stepper.h"
#endif

#ifndef _INCLUDE_MATH_H
	#define _INCLUDE_MATH_H
	#include "math.h"
#endif

#ifndef _INCLUDE_STEPPERCONFIGURATION_H
	#define _INCLUDE_STEPPERCONFIGURATION_H
	#include "StepperConfiguration.h"
#endif

#ifndef _INCLUDE_STATUS_H
	#define _INCLUDE_STATUS_H
	#include "Status.h"
#endif
*/


#ifndef _PLANNER_H
	#define _PLANNER_H
	#include "stm32f10x.h"
	#include "Stepper.h"
	#include "math.h"
	#include "StepperConfiguration.h"
	#include "Status.h"
	
	#define u64 unsigned long long
	#define s64 long long
	/*
	�˶��滮��
	�ٶȿ���

			A
		Vm|         +--------------+     +-----+
			|        /                \   /       \
			|       /                  \ /         \
			|      /                    +           \
		Vs|     +                                  +
			|     |                                  |
			|			|                                  |
			+------------------------------------------>
																									t

	�����ٶ�Vs���趨��
		�����������ʱת�ٲ���̫�ߣ������ʧ��
		һ������£�������ֲ��ж����漰����ٶ�
		���ｫ�������ת�ٶ�Ӧ���ٶȼ�ΪVjerk
		
		��Ϊ�����VsҲ���л����̵���С�л��ٶ�
		���ǵ��ٶȷ���仯Ϊ180��������Ӧ����Vs < Vjerk/2
		��|��V| < Vjerk ���ȶ����������ȡ Vs = 0.4*Vjerk

	��V1��V2 �� 0 ʱ������л�/�����ٶȼ��㣺

												A
											 / \
										V2/   \��V 
										 /     \
										/��     \
									 ----------> 
													 V1
													V1��V2
							 cos�� = ------------ 
												|V1|*|V2|
												 ______________
							 sin�� = �� 1 - (cos��)^2
						 ____________________________________
	 |��V| = ��(|V2|*sin��)^2 + (|V2|*cos��-|V1|)^2

	 |��V| = 2*|V1|*sin��/2 = 2*|V2|*sin��/2 ��V1 = V2ʱ

		���˶�����ı�ʱ�ٶȿ��ܷ������䣬������䲻��̫��
		��������Ҫ�������ٶȳ�Ϊ����л�/�����ٶ�(�ֱ��Ӧ����ǰ/��)
		Ϊ��ȷ����������Ҫ�󣬿�����Ҫ���ȼ��ٵ�����л��ٶ�

		����ͨ�����Ҷ�����������λ�Ʒ���ʸ���нǵ�����ֵ
		��ͨ������ֵ�����λ�Ʒ���ʸ���нǵ�����ֵ
		������˵��λ�Ʋ�������AB�����λ��

		Ϊ�˵õ�����л�/�����ٶ�V1V2�����Ǧ�Vmax
		һ������¦�Vmax����һ���Զ����������ܱ仯������ٶ�
		Ҳ����˵ ��Vmax = T*Amax��T���Զ����������ڡ�Amax�������ٶ�
		��Ȼ��T�Ǻ�V1�йصģ���XYZ��Ĳ�������L������� T = L/V1
		��Vmax = L*Amax/V1��������|V| = |V1| = |V2|��Ҳ���ǵ������л�
		��ʱ��|��Vmax| = |��V|���õ�|��Vmax| = 2*|V1|*sin��/2
															 ____________________
		������|V| = |V1| = |V2| = �� L*Amax / 2*sin��/2

		�������������
		1.|V| > |Vmax| ����ʱ����ֱ���л��������
		2.|V|�� [|Vs|,|Vmax|] ����ʱ������Ҫ�ȼ��ٵ�|V|
		 ע����ĳЩ����¼��ٲ���ȫ���ٶȿ��ܵ���V
		3.|V| < |Vs| ����Ҫ���ٵ������ٶȣ��������һ�����۷�����ͣʱ����

	�ӡ����������ٽ��������㣺
		�ȼ����X��Y��Z�Ĳ���Sx��Sy��Sz
		�ڼ��㲽��ʱֻ�᲻��      ___________________
		�ɲ���������ϳɲ���S = �� Sx^2 + Sy^2 + Sz^2
		�ɺϳɲ���������ܽ�����C = S/L ��L��XYZ��Ĳ���
		�ڼ����ܽ�����ʱֻ�벻��
		
		ȡ����һ��λ�Ƶ��л��ٶ�Vi��˶�λ�Ƶ��л��ٶ�Vo
		��Vo^2 -Vi^2 > 2*Amax*S*L ����˵��Vi��Vo������
																				___________________
		��Vi < Vo ����Ӧ����СVo ��ʹ��Vo = �� Vi^2 + 2*Amax*S*L
		��ʱ���н��ľ�Ϊ���ٽ���
																							___________________
		��֮����Vi > Vo ����Ӧ����СVi ��ʹ��Vi = �� Vo^2 - 2*Amax*S*L
		��ʱ���н��ľ�Ϊ���ٽ���

		��һ�����ע�⣺�ı�Vi��ζ����һ��λ�Ƶ��ٶ�������Ҫ���¼���
		�������ܵ�����һ��λ�Ƶ�ViҲ��ò����� ���Դ�����......
		����㷨��Ҫ���㹻��ǰհ�ԣ����ڴ��������ܽ�ǿ������Ԥ����16����������
		��Ȼ����ʱԤ�����þ�Ҳ�޷�����(�޷�ʹ�����е��л��ٶȶ�����)
		��ʱ���ȡǿ��������ʩ�����ݸ���λ��֮�͡�S�ʵ����������ٶ�Amax
		�����һ���̶��Ͻ��Ͳ�Ʒ������������������ǿ��ǰհ������������ǳ�����
		
		��Vi��Vo��Ϊ����ֵʱ������ӡ����ٽ�����
		���ٽ�����Cacc = (Vmax^2 - Vi^2)*Csum/(2*Amax*L) ��ֻ�᲻��
		���ٽ�����Cdec = (Vmax^2 - Vo^2)*Csum/(2*Amax*L) ��ֻ�᲻��
		
		���ܽ�����C �� Cacc + Cdec ʱ �������ٽ�����Ccon = C - Cacc - Cdec
		
		���ܽ�����C < Cacc + Cdec ʱ�����ڲ���ȫ����״̬
		A  Vpeak +
	V |       /|\
		|      /   + Vo  ....
		|  Vi +  | |
		|     |    |
		+--------+---------------->
					 Ta Td             T
		Ta = (Vpeak - Vi)/Amax
		Td = (Vpeak - Vo)/Amax
		2*Vpeak^2 - Vi^2 - Vo^2 = 2*Amax*S
							__________________________
		Vpeak = ��(2*Amax*S + Vi^2 + Vo^2)/2
		���ٽ�����Cacc = (Vpeak^2 - Vi^2)/(2*Amax*L)��ֻ�᲻��
		���ٽ�����Cdec = (Vpeak^2 - Vo^2)/(2*Amax*L)��ֻ�᲻��
		���ٽ�����Ccon = Ccon = C - Cacc - Cdec
		��ʱ���ٽ��������������ڽ���������
		C < Cacc + Cdec ����ԳƵؼ���Cacc��Cdec
		
	*/


	class PLANNER{
		public:
			void Init(); //��ʼ��
			bool Load(double Position[5],float VelocityMax); //װ�ػ���
			bool Launch(STEPPER::_L1 *Pointer); //�������棬���ƶ�L1ָ��
		


//		private:
			struct _L2{ //L2����
				double Position[5]; //Ŀ��λ�þ�������
				double Length[5]; //Ŀ��λ���������(�ƶ�����)
				s64 Step[5]; //����
				s64 SumStep; //�ܽ�����
				float TPluse[5]; //�������
				double VectorSize; //��������
				u64 Accelerate; //���ٽ�����
				u64 ConstSpeed; //���ٽ�����
				u64 Decelerate; //���ٽ�����
				float VelocityIn; //�����ٶ�
				float VelocityOut; //�л��ٶ�
				float VelocityMax; //����ٶ�
				float SubVelocityMax; //����ٶȻ���
//				float DeltaVmax; //���ɱ仯�ٶ�
				bool Full; //������־
				_L2 *Pror; //����ѭ��ָ��
				_L2 *Next; //ѭ��ָ��
			}L2_Cache[L2_SIZE];		
			
			void CaclVelocityAndClock(); //�����ٶ��������
		
			void CaclVelocity(_L2 *Pointer); //���������нǼ����л��ٶ����ֵ
			void CaclVelocityIn(_L2 *Pointer,float &Vmax,float &Vmin); //�����л��ٶȼ��������ٶ����ֵ
			
			void CaclClock(_L2 *Pointer); //���������л��ٶȼ������
			
			u8 L2RestAmount; //L2����ʣ������	
			
			_L2 *PtrLoader; //L2�������
			_L2 *PtrLauncher; //L2�������
			
			float AccelerationMax;
	};

	extern class PLANNER Planner;


#endif

