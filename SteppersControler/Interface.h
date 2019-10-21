#include "stm32f10x.h"

#ifndef _INTERFACE_H
	#define _INTERFACE_H


	extern void STEPPER_X_PLUSE(bool S); //���ò������X����״̬

	extern void STEPPER_Y_PLUSE(bool S); //���ò������Y����״̬

	extern void STEPPER_Z_PLUSE(bool S); //���ò������Z����״̬

	extern void STEPPER_A_PLUSE(bool S); //���ò������A����״̬

	extern void STEPPER_B_PLUSE(bool S); //���ò������B����״̬



	extern void STEPPER_X_DIRECTION(bool D); //���ò������X����״̬

	extern void STEPPER_Y_DIRECTION(bool D); //���ò������Y����״̬

	extern void STEPPER_Z_DIRECTION(bool D); //���ò������Z����״̬

	extern void STEPPER_A_DIRECTION(bool D); //���ò������A����״̬

	extern void STEPPER_B_DIRECTION(bool D); //���ò������B����״̬



	extern void STEPPER_ENABLE(bool E); //���ò������״̬



	extern void PROB_ENABLE(bool E); //����̽��״̬

	extern void AUX_ENABLE(bool E); //���ø����˿�״̬
	

	extern bool TEST_X_MAX(); //���X�յ���λ��״̬

	extern bool TEST_Y_MAX(); //���Y�յ���λ��״̬

	extern bool TEST_Z_MAX(); //���Z�յ���λ��״̬



	extern bool TEST_X_MIN(); //���X�����λ��״̬

	extern bool TEST_Y_MIN(); //���Y�����λ��״̬

	extern bool TEST_Z_MIN(); //���Z�����λ��״̬


	extern void Delay_Us(u32 n);

	extern void Delay_Ms(u32 n);

#endif

