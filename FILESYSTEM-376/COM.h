#ifndef _COM_H
	#define _COM_H
	#include "stm32f10x.h"
	/*
	//�䳤ָ�������^
	//��ʼ��%
	//��ַ��FFF
	+-------------------------------------------------------------------------------------------------+
	|        Operator         |    Sub Operator    |     Return      |            Function            |
	+-------------------------------------------------------------------------------------------------+
	|       FILE_ENUM         |                    |   struct Flat   | Enum All Archived File In Root |
	|-------------------------------------------------------------------------------------------------|
	|    READ_BLOCK_NEXT      |     File Index     |   char *1024    |     Initialization Position    |
	|-------------------------------------------------------------------------------------------------|
	|    READ_BLOCK_AGAIN     |     File Index     |   char *1024    |     Initialization Position    |
	|-------------------------------------------------------------------------------------------------|
	|    READ_BLOCK_PRIOR     |     File Index     |   char *1024    |     Initialization Position    |
	|-------------------------------------------------------------------------------------------------|
	|        Unknown          |                    |      NNN        |                                |
	+-------------------------------------------------------------------------------------------------+
	|         Error           |                    |      RRR        |                                |
	+-------------------------------------------------------------------------------------------------+
	// Instruction M X/Y/Z/A/B/V
		%SSSMXs****.******Ys****.******Zs****.******As******.****Bs******.****V***
			    6            19           32           45           58           71

	// Instruction I X/Y/Z/A/B
		%SSSIXs****.******Ys****.******Zs****.******As******.****Bs******.****
			    6            19           32           45           58

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
		
			void Decoder(); //���벢ִ��
			void Print(char *data); //�����ַ���

			u8 Buffer[128]; //L1����(���ջ���)
			u8 RxData[128]; //L2����(���ݻ���)
			bool RxFinish; //������ɱ�־
			u8 DataCounter; //������
			bool VerifyError; //У������־
			bool TestAddress(); //��ַ����
		
		private:
			void Tx(u8); 

	};

	extern class COM Serial;

#endif
	