#include "stm32f10x.h"

#ifndef _INTERFACE_H
	#define _INTERFACE_H


	extern void STEPPER_X_PLUSE(bool S); //设置步进电机X脉冲状态

	extern void STEPPER_Y_PLUSE(bool S); //设置步进电机Y脉冲状态

	extern void STEPPER_Z_PLUSE(bool S); //设置步进电机Z脉冲状态

	extern void STEPPER_A_PLUSE(bool S); //设置步进电机A脉冲状态

	extern void STEPPER_B_PLUSE(bool S); //设置步进电机B脉冲状态



	extern void STEPPER_X_DIRECTION(bool D); //设置步进电机X方向状态

	extern void STEPPER_Y_DIRECTION(bool D); //设置步进电机Y方向状态

	extern void STEPPER_Z_DIRECTION(bool D); //设置步进电机Z方向状态

	extern void STEPPER_A_DIRECTION(bool D); //设置步进电机A方向状态

	extern void STEPPER_B_DIRECTION(bool D); //设置步进电机B方向状态



	extern void STEPPER_ENABLE(bool E); //设置步进电机状态



	extern void PROB_ENABLE(bool E); //设置探针状态

	extern void AUX_ENABLE(bool E); //设置辅助端口状态
	

	extern bool TEST_X_MAX(); //检测X终点限位器状态

	extern bool TEST_Y_MAX(); //检测Y终点限位器状态

	extern bool TEST_Z_MAX(); //检测Z终点限位器状态



	extern bool TEST_X_MIN(); //检测X起点限位器状态

	extern bool TEST_Y_MIN(); //检测Y起点限位器状态

	extern bool TEST_Z_MIN(); //检测Z起点限位器状态


	extern void Delay_Us(u32 n);

	extern void Delay_Ms(u32 n);

#endif

