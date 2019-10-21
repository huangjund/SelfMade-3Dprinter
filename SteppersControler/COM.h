/*
#ifndef _INCLUDE_STM32F10X_H
	#define _INCLUDE_STM32F10X_H
	#include "stm32f10x.h"
#endif

#ifndef _INCLUDE_PLANNER_H
	#define _INCLUDE_PLANNER_H
	#include "Planner.h"
#endif

#ifndef _INCLUDE_STATUS_H
	#define _INCLUDE_STATUS_H
	#include "Status.h"
#endif

#ifndef _INCLUDE_STEPPER_H
	#define _INCLUDE_STEPPER_H
	#include "Stepper.h"
#endif

#ifndef _INCLUDE_STEPPERCONFIGURATION_H
	#define _INCLUDE_STEPPERCONFIGURATION_H
	#include "StepperConfiguration.h"
#endif
*/


#ifndef _COM_H
	#define _COM_H
	#include "stm32f10x.h"
	#include "Planner.h"
	#include "Status.h"
	#include "Stepper.h"
	#include "StepperConfiguration.h"
	/*
	//变长指令，结束符\n
	//起始符%
	//地址符SSS
	+--------------------------------------------------------------------------------------------------------------------------------------------------------+
	|   Operator    |  SubOperator  |        Operand        |   Instruction Length   |      Return      |   Return Length   |            Function            |
	+--------------------------------------------------------------------------------------------------------------------------------------------------------+
	|      M        |  X/Y/Z/A/B/V  | destination/velocity  |           75           |    DDD or FFF    |         3         |  Set Destination and Buffered  |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      I        |   X/Y/Z/A/B   |       position        |           71           |    DDD or KKK    |         3         |     Initialization Position    |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      H        |    HOMEXYZ    |                       |           12           |    DDD or KKK    |         3         |      AutoHoming Axis X,Y,Z     |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      G        |X,Y,Z,A,B,O,P,Q|                       |           6            |     position     |         12        |   Get Position of Given Axis   |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      G        |    S,I,E,H    |                       |           6            |      status      |         1         |     Get Status of Automate     |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      G        |       1       |                       |           6            |    rest amount   |         3         |   Get Rest Amount of L1 Cache  |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      G        |       2       |                       |           6            |    rest amount   |         3         |   Get Rest Amount of L2 Cache  |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      G        |       M       |                       |           6            |       size       |         3         |      Get Size of L1 Cache      |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      G        |       N       |                       |           6            |       size       |         3         |      Get Size of L2 Cache      |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      P        |   X,Y,Z,A,B   |       position        |           18           |       DDD        |         3         |   Set Position of Given Axis   |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      S        |       E       |                       |           6            |       DDD        |         3         |         Enable Steppers        |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      S        |       D       |                       |           6            |    DDD or KKK    |         3         |         Disable Steppers       |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      D        |       E       |                       |           6            |       DDD        |         3         |          Activate IDP          |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      D        |       D       |                       |           6            |       DDD        |         3         |         Deactivate IDP         |
	|--------------------------------------------------------------------------------------------------------------------------------------------------------|
	|      T        |       *       |                       |           1            |       DDD        |         3         |       Communication Test       |
	+--------------------------------------------------------------------------------------------------------------------------------------------------------+
	|   Unknown     |    Unknown    |        Unknown        |           ?            |       NNN        |         3         |                                |
	+--------------------------------------------------------------------------------------------------------------------------------------------------------+
	|    Error      |     Error     |         Error         |           ?            |       RRR        |         3         |                                |
	+--------------------------------------------------------------------------------------------------------------------------------------------------------+
	// Instruction M X/Y/Z/A/B/V
		%SSSMXs****.******Ys****.******Zs****.******As******.******Bs******.******V***
			    6            19           32           45             60             75

	// Instruction I X/Y/Z/A/B
		%SSSIXs****.******Ys****.******Zs****.******As******.******Bs******.******
			    6            19           32           45             60

	// Tnstruction G X,Y,Z,A,B,S,1,2,M,N
		%SSS + GX , GY , GZ , GA , GB , GS , G1 , G2 , GM , GN 

	// Instruction P X,Y,Z,A,B
		%SSS + PXs****.****** , PYs****.****** , PZs****.****** , PAs******.**** , PBs******.****
			       6                6                6                6                6

	// Instruction S E,D
		%SSS + SE , SD

	// Instruction T
		%SSST
	*/

	class COM{
		public:
			void Init();
		
			void Decoder(); //译码并执行
			void Print(char *data); //发送字符串
			void Tx_Decimal(double Data); //发送ASCII编码s6.6有理数
			void Tx_Integer(u32 Data); //发送ASCII编码3位整数
		
			u8 Buffer[128]; //接收缓存
			u8 RxData[128]; //辅助接受缓存
			bool RxFinish; //接收完成标志
			u8 DataCounter; //数据量
			u8 DataNumber; //总数据量
			bool VerifyError; //校验错误标志
			bool TestAddress(); //地址检验
		private:
			void Tx(u8); 
			double C2D(u8 *Data); //ASCII-s4.6 -> Double
			double LC2D(u8 *Data); //ASCII-s6.6 -> Double
			float C2F(u8 *Data); //ASCII-3 -> Float
		
//			double Temp[5];
//			float V;
		
	};

	extern class COM Serial;

#endif

	
