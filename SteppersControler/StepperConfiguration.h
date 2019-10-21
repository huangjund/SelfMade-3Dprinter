#ifndef _STEPPERCONFIGURATION_H
	#define _STEPPERCONFIGURATION_H
	#include "stm32f10x.h"

	//------------------------限位开关-----------------------//
	//X轴最大坐标限位开关有效电平
	#define AXIS_X_MAX_STOP_LOW 1
	//X轴最小坐标限位开关有效电平
	#define AXIS_X_MIN_STOP_LOW 1

	//Y轴最大坐标限位开关有效电平
	#define AXIS_Y_MAX_STOP_LOW 1
	//Y轴最小坐标限位开关有效电平
	#define AXIS_Y_MIN_STOP_LOW 1

	//Z轴最大坐标限位开关有效电平
	#define AXIS_Z_MAX_STOP_LOW 1
	//Z轴最小坐标限位开关有效电平
	#define AXIS_Z_MIN_STOP_LOW 1

	//A轴最大坐标限位开关有效电平
	#define AXIS_A_MAX_STOP_LOW 1 
	//A轴最小坐标限位开关有效电平
	#define AXIS_A_MIN_STOP_LOW 1

	//B轴最大坐标限位开关有效电平
	#define AXIS_B_MAX_STOP_LOW 1 
	//B轴最小坐标限位开关有效电平
	#define AXIS_B_MIN_STOP_LOW 1
	//-------------------------------------------------------//

	//-------------------------正方向------------------------//
	//X轴正方向对应低电平
	#define AXIS_X_DIRE0_EQU_LOW 1

	//Y轴正方向对应低电平
	#define AXIS_Y_DIRE0_EQU_LOW 1

	//Z轴正方向对应低电平
	#define AXIS_Z_DIRE0_EQU_LOW 1

	//A轴正方向对应低电平
	#define AXIS_A_DIRE0_EQU_LOW 1

	//B轴正方向对应低电平
	#define AXIS_B_DIRE0_EQU_LOW 1
	//-------------------------------------------------------//

	//------------------------归位速度-----------------------//
	#define VELOCITY_HOMING 20.0f //[mm/s]
	//-------------------------------------------------------//


	//------------------------跳变速度-----------------------//
	/*速度可以从0跳变到这个值或反之*/
	#define VELOCITY_START 2.0f //[mm/s]
	//-------------------------------------------------------//

	//-----------------------默认加速度----------------------//
	#define DEFAULT_ACCELERATION 1300.0f //[mm/s^2]
	//-------------------------------------------------------

	//-----------------------脉冲步距比----------------------//
	/*对于某个轴而言，运动1mm需要的脉冲数*/
	//轴向脉冲步距比

	#define X_STEPS_PER_MM 160
	#define X_STEP_LENGTH 0.00625

	#define Y_STEPS_PER_MM 160
	#define Y_STEP_LENGTH 0.00625

	#define Z_STEPS_PER_MM 160
	#define Z_STEP_LENGTH 0.00625

	#define A_STEPS_PER_MM 160
	#define A_STEP_LENGTH 0.00625

	#define B_STEPS_PER_MM 160
	#define B_STEP_LENGTH 0.00625

	#define SYNTHETIC_LENGTH 0.00625

	/*
	由于环向与轴向采用了相同的步长
	这会产生一些偏差，实际计算的时候应当修正
	环向需要增加约6%
	*/
	//-------------------------------------------------------//

	//------------------------极限行程-----------------------//
	#define AXIS_X_MAX_POSITION 210.0
	#define AXIS_X_MIN_POSITION 000.0

	#define AXIS_Y_MAX_POSITION 210.0
	#define AXIS_Y_MIN_POSITION 000.0

	#define AXIS_Z_MAX_POSITION 430.0
	#define AXIS_Z_MIN_POSITION 000.0

	//-------------------------------------------------------//

	//------------------------自动归位-----------------------//
	#define AXIS_X_RETURN_LENGTH 7.50//220.0

	#define AXIS_Y_RETURN_LENGTH 7.50//220.0

	#define AXIS_Z_RETURN_LENGTH 430.0
	
	//探针高度
	#define HIGH_PROB 2.4 //[mm]

	//-------------------------------------------------------//


	//------------------------最大误差-----------------------//
	//基准面允许的最大高度差
	
	#define MAX_DIFFERENCE 0.2 //[mm]

	//-------------------------------------------------------//


	//------------------------脉冲宽度-----------------------//
	#define PLUSE_WIDTH 10 //[us]
	//-------------------------------------------------------//


	//-------------------------L1配置------------------------//
	#define L1_SIZE 4
	//-------------------------------------------------------//

	//-------------------------L2配置------------------------//
	#define L2_SIZE 4
	//-------------------------------------------------------//

#endif

