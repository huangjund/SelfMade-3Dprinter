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
	运动规划器
	速度控制

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

	启动速度Vs的设定：
		步进电机启动时转速不能太高，否则会失步
		一般情况下，电机的手册中都会涉及这个速度
		这里将最高启动转速对应的速度记为Vjerk
		
		因为这里的Vs也是切换过程的最小切换速度
		考虑到速度方向变化为180°的情况，应当有Vs < Vjerk/2
		即|ΔV| < Vjerk ，稳定起见，最大可取 Vs = 0.4*Vjerk

	在V1、V2 ≠ 0 时，最大切换/切入速度计算：

												A
											 / \
										V2/   \ΔV 
										 /     \
										/θ     \
									 ----------> 
													 V1
													V1·V2
							 cosθ = ------------ 
												|V1|*|V2|
												 ______________
							 sinθ = √ 1 - (cosθ)^2
						 ____________________________________
	 |ΔV| = √(|V2|*sinθ)^2 + (|V2|*cosθ-|V1|)^2

	 |ΔV| = 2*|V1|*sinθ/2 = 2*|V2|*sinθ/2 ，V1 = V2时

		当运动方向改变时速度可能发生跳变，这个跳变不能太大
		满足跳变要求的最大速度称为最大切换/切入速度(分别对应跳变前/后)
		为了确保跳变满足要求，可能需要事先减速到最大切换速度

		首先通过余弦定理计算出两段位移方向矢量夹角的余弦值
		再通过余弦值计算出位移方向矢量夹角的正弦值
		这里所说的位移并不包括AB两轴的位移

		为了得到最大切换/切入速度V1V2，考虑ΔVmax
		一般情况下ΔVmax是在一个自动机节拍内能变化的最大速度
		也就是说 ΔVmax = T*Amax，T是自动机节拍周期、Amax是最大加速度
		当然，T是和V1有关的，在XYZ轴的步长都是L的情况下 T = L/V1
		ΔVmax = L*Amax/V1，不妨令|V| = |V1| = |V2|，也就是等速率切换
		此时令|ΔVmax| = |ΔV|，得到|ΔVmax| = 2*|V1|*sinθ/2
															 ____________________
		于是有|V| = |V1| = |V2| = √ L*Amax / 2*sinθ/2

		考虑三种情况：
		1.|V| > |Vmax| ，此时可以直接切换无需减速
		2.|V|∈ [|Vs|,|Vmax|] ，此时可能需要先减速到|V|
		 注：在某些情况下加速不完全，速度可能低于V
		3.|V| < |Vs| ，需要减速到启动速度，这种情况一般在折返或启停时出现

	加、减速与匀速节拍数计算：
		先计算出X、Y、Z的步数Sx、Sy、Sz
		在计算步数时只舍不入      ___________________
		由步数计算出合成步数S = √ Sx^2 + Sy^2 + Sz^2
		由合成步数计算出总节拍数C = S/L ，L是XYZ轴的步长
		在计算总节拍数时只入不舍
		
		取得上一段位移的切换速度Vi与此段位移的切换速度Vo
		若Vo^2 -Vi^2 > 2*Amax*S*L ，则说明Vi、Vo不合理
																				___________________
		若Vi < Vo ，则应当减小Vo ，使得Vo = √ Vi^2 + 2*Amax*S*L
		此时所有节拍均为加速节拍
																							___________________
		反之，若Vi > Vo ，则应当减小Vi ，使得Vi = √ Vo^2 - 2*Amax*S*L
		此时所有节拍均为减速节拍

		有一点必须注意：改变Vi意味着上一段位移的速度曲线需要重新计算
		这样可能导致上一段位移的Vi也变得不合理 ，以此类推......
		因此算法需要有足够的前瞻性，鉴于处理器性能较强，可以预处理16段甚至更多
		当然，若时预处理用尽也无法收敛(无法使得所有的切换速度都合理)
		这时会采取强制收敛措施，根据各段位移之和∑S适当增加最大加速度Amax
		这会在一定程度上降低产品的质量，不过在这样强的前瞻性下这种情况非常罕见
		
		当Vi、Vo均为合理值时，计算加、减速节拍数
		加速节拍数Cacc = (Vmax^2 - Vi^2)*Csum/(2*Amax*L) ，只舍不入
		减速节拍数Cdec = (Vmax^2 - Vo^2)*Csum/(2*Amax*L) ，只舍不入
		
		若总节拍数C ≥ Cacc + Cdec 时 ，有匀速节拍数Ccon = C - Cacc - Cdec
		
		当总节拍数C < Cacc + Cdec 时，处于不完全加速状态
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
		Vpeak = √(2*Amax*S + Vi^2 + Vo^2)/2
		加速节拍数Cacc = (Vpeak^2 - Vi^2)/(2*Amax*L)，只舍不入
		减速节拍数Cdec = (Vpeak^2 - Vo^2)/(2*Amax*L)，只舍不入
		匀速节拍数Ccon = Ccon = C - Cacc - Cdec
		此时匀速节拍数的意义在于节拍数补偿
		C < Cacc + Cdec ，则对称地减少Cacc与Cdec
		
	*/


	class PLANNER{
		public:
			void Init(); //初始化
			bool Load(double Position[5],float VelocityMax); //装载缓存
			bool Launch(STEPPER::_L1 *Pointer); //弹出缓存，不移动L1指针
		


//		private:
			struct _L2{ //L2缓存
				double Position[5]; //目标位置绝对坐标
				double Length[5]; //目标位置相对坐标(移动距离)
				s64 Step[5]; //步数
				s64 SumStep; //总节拍数
				float TPluse[5]; //脉冲节拍
				double VectorSize; //向量长度
				u64 Accelerate; //加速节拍数
				u64 ConstSpeed; //匀速节拍数
				u64 Decelerate; //减速节拍数
				float VelocityIn; //切入速度
				float VelocityOut; //切换速度
				float VelocityMax; //最大速度
				float SubVelocityMax; //最大速度缓存
//				float DeltaVmax; //最大可变化速度
				bool Full; //已填充标志
				_L2 *Pror; //逆向循环指针
				_L2 *Next; //循环指针
			}L2_Cache[L2_SIZE];		
			
			void CaclVelocityAndClock(); //计算速度与节拍数
		
			void CaclVelocity(_L2 *Pointer); //根据向量夹角计算切换速度最大值
			void CaclVelocityIn(_L2 *Pointer,float &Vmax,float &Vmin); //根据切换速度计算切入速度最大值
			
			void CaclClock(_L2 *Pointer); //根据切入切换速度计算节拍
			
			u8 L2RestAmount; //L2缓存剩余容量	
			
			_L2 *PtrLoader; //L2缓存入口
			_L2 *PtrLauncher; //L2缓存出口
			
			float AccelerationMax;
	};

	extern class PLANNER Planner;


#endif

