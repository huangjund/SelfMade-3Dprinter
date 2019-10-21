#include "Stepper.h"

STEPPER Stepper;

inline double GetMax(double Data[],u32 Length){ //���ֵ
	double Temp = Data[0];
	for(u32 i=1;i<Length;i++)
		if(Data[i] > Temp) Temp = Data[i];
	return Temp;
}
inline double GetMin(double Data[],u32 Length){ //��Сֵ
	double Temp = Data[0];
	for(u32 i=1;i<Length;i++)
		if(Data[i] < Temp) Temp = Data[i];
	return Temp;
}

extern"C"void TIM2_IRQHandler(){
  if(TIM2->SR&0x01){
		if(Stepper.FAStatus == 'I'){ //����״̬
			if(Stepper.PtrLauncher->Full){ //����ǿ�
				Stepper.SetAllDirection(Stepper.PtrLauncher->Direction); //���÷���
				TIM2->ARR = (u16)Stepper.PtrLauncher->CycleIn; //���ý�������
				Stepper.FAStatus = 'W'; //��������״̬           
			}
			else{ //�����
			TIM2->ARR = 5625;
				Stepper.FAStatus = 'I'; //���ֿ���״̬
			}
		}
		
		else if(Stepper.FAStatus == 'W'){ //����״̬
			if(Stepper.PtrLauncher->Full){ //����ǿ�
				Stepper.PtrLauncher->ClockCounter++; //��ǰʱ��+1
				
			//�����ٶ�
				if(Stepper.PtrLauncher->Accelerate){ //����
					Stepper.PtrLauncher->Accelerate--;
					
					float VelocityPresent = sqrtf(
						Stepper.PtrLauncher->VelocityIn*Stepper.PtrLauncher->VelocityIn +
						2.0f*Stepper.PtrLauncher->Acceleration*Stepper.PtrLauncher->ClockCounter*SYNTHETIC_LENGTH);
					
					TIM2->ARR = (u16)(56250/VelocityPresent); //���ý�������
				}
				else if(Stepper.PtrLauncher->ConstSpeed){ //����
					Stepper.PtrLauncher->ConstSpeed--;
				}
				else if(Stepper.PtrLauncher->Decelerate){ //����
					Stepper.PtrLauncher->Decelerate--;
					float VelocityPresent = sqrtf(
						Stepper.PtrLauncher->VelocityOut*Stepper.PtrLauncher->VelocityOut +
						2.0f*Stepper.PtrLauncher->Deceleration*Stepper.PtrLauncher->Decelerate*SYNTHETIC_LENGTH);
					
					TIM2->ARR = (u16)(56250/VelocityPresent); //���ý�������
				}
				
			//��������
				u8 PluseSelection = 0x00; //��¼
				for(u32 i=0;i<5;i++){
					if(Stepper.PtrLauncher->NextPluseClock[i]<Stepper.PtrLauncher->ClockCounter){ //��������ʱ��
						if(Stepper.PtrLauncher->Step[i]){ //���в���ʣ��
							Stepper.PtrLauncher->NextPluseClock[i] += Stepper.PtrLauncher->TPluse[i]; //������һ������ʱ��	
							PluseSelection |= 0x01<<i; //��¼����
							Stepper.PtrLauncher->Step[i]--; //ʣ�ಽ����һ
						}
					}
				}
				Stepper.GeneratePluse(PluseSelection); //��������	
				TIM3->ARR = PLUSE_WIDTH; 
				
			//���ʣ�ಽ��
				for(u32 i=0;i<6;i++){
					if(i==5){ //û��ʣ��
						Stepper.PtrLauncher->Full = 0; //�����������־
						Stepper.L1RestAmount++; //ʣ�໺��������һ 
						Stepper.FAStatus = 'S'; //�����л�״̬
						TIM2->ARR = 2; //�����л�
						break;
					}
					if(Stepper.PtrLauncher->Step[i]) break; //����ǿ�
				}
			}
			
			else{ //�����
				Stepper.FAStatus = 'S'; //�����л�״̬
				TIM2->ARR = 2; //�����л�
			}
		}
		
		else if(Stepper.FAStatus == 'S'){ //�л�����״̬
			Stepper.PtrLauncher = Stepper.PtrLauncher->Next; //�л�����
			
			if(Stepper.PtrLauncher->Full){ //����ǿ�
				Stepper.SetAllDirection(Stepper.PtrLauncher->Direction); //���÷���
				TIM2->ARR = (u16)Stepper.PtrLauncher->CycleIn; //���ý�������
				Stepper.FAStatus = 'W'; // ��������״̬
			}
			else{ //�����
				Stepper.FAStatus = 'I'; //�������״̬
				TIM2->ARR = 5625;
			}
		}
		
		else{ //״̬����
			Stepper.FAStatus = 'I'; //�������״̬
			TIM2->ARR = 5625;
		}
  	TIM2->SR&=~0x01;
  }
}

extern"C"void TIM3_IRQHandler(void){
//��ʱ��3�����ж�
//��������������������½���
//����ʧ�ܶ�ʱ��3�ȴ���һ������	
	if(TIM3->SR&0x01){
		
		Stepper.GeneratePluse(0x00); //���������½���
		TIM3->ARR = 0; //ʧ�ܶ�ʱ��

		TIM3->SR&=~0x01;
	}
}

void STEPPER::Init(){
	//ѭ��ָ���ʼ��
	L1_Cache[L1_SIZE-1].Next = L1_Cache;
	L1_Cache[0].Pror = (L1_Cache+L1_SIZE-1);
	for(u32 i=0;i<L1_SIZE-1;i++)
		(L1_Cache+i)->Next = L1_Cache+i+1;
	for(u32 i=1;i<L1_SIZE;i++)
		(L1_Cache+i)->Pror = L1_Cache+i-1;
	
	//�ų�ʼ��
	PtrLauncher = L1_Cache;
	PtrLoader = L1_Cache;
	
	//�������ݳ�ʼ��
	for(u32 i=0;i<L1_SIZE;i++){
		for(u32 c=0;c<5;c++){
			L1_Cache[i].Direction[c] = 0;
			L1_Cache[i].Step[c] = 0;
			L1_Cache[i].NextPluseClock[c] = 0;
			L1_Cache[i].TPluse[c] = 0;
		}
		
		L1_Cache[i].ClockCounter = 0;
		L1_Cache[i].Accelerate = 0;
		L1_Cache[i].ConstSpeed = 0;
		L1_Cache[i].Decelerate = 0;
		L1_Cache[i].CycleIn = 0;
		L1_Cache[i].Acceleration = 0;
		L1_Cache[i].Deceleration = 0;
		L1_Cache[i].VelocityIn = 0;
		L1_Cache[i].VelocityOut = 0;
		L1_Cache[i].Full = 0;
	}
	
	//����������ʼ��
	L1RestAmount = L1_SIZE;
	
	//�Զ���״̬��ʼ��
	FAStatus = 'I';
	
	VirtualStep[0] = 0;
	VirtualStep[1] = 0;
	VirtualStep[2] = 0;
	
	RCC->APB2ENR |= 0x07<<2;
	GPIOA->CRL &= 0xF000FFFF;
	GPIOA->CRL |= 0x03330000;
	GPIOA->CRH &= 0x0FF00000;
	GPIOA->CRH |= 0x30033333;
	GPIOB->CRL &= 0x00000FFF;
	GPIOB->CRL |= 0x88887000;
	GPIOB->CRH &= 0x000FFF00;
	GPIOB->CRH |= 0x33300088;
	GPIOB->BSRR |= 0x03F0;	
	GPIOC->CRH &= 0x000FFFFF;
	GPIOC->CRH |= 0x33300000;
	
	/*��ʱ��2��ʼ��
		��ʱ��2���������Զ�������
		��������ǿɱ�ģ�ȡ���ڵ�ǰ�Ĳ�������ٶ�
		����������ٶȽϿ�ʱ������Ҳ�Ͽ�
		��������ٶȽ���ʱ������Ҳ��֮����
		�������������ֵ�����Ӧ�Ų�������������ٶ�
		
		ÿ�����ĵ���ʱ�������ڶ�ʱ��2�ĸ����ж��и����Զ���״̬
		��ȻҲ�����������ɲ���������������������
		��(��)�ı䲽��������ٶȣ�Ҳ���Ǹı��������
	*/
	RCC->APB1ENR |= 0x01;
  TIM2->ARR = 5625;//Ԥװ��
  TIM2->PSC = 7;//8��Ƶ
  TIM2->CR1 |= 0x01<<4;
  TIM2->DIER |= 0x01<<0;
  NVIC->IP[28] = 0x10;//���ȼ�1
  NVIC->ISER[0] |= 0x01<<28;
  TIM2->CR1 |= 0x01<<0;//ʹ��
	
	/*��ʱ��3��ʼ��
	��ʱ��3���Ը�����ʱ��2���ɲ��������������
	����ʱ��2���ɲ��������������������ص�ͬʱ����������ʱ��3
	����һ��ʱ��(һ����10��΢��)�󣬶�ʱ��3����������¼�
	�ڶ�ʱ��3�ĸ����ж������ɲ����������������½���
	*/
  RCC->APB1ENR |= 0x01<<1;
  TIM3->ARR = 0;//Ԥװ��
  TIM3->PSC = 71;//72��Ƶ
  TIM3->CR1 |= 0x01<<4;
  TIM3->DIER |= 0x01<<0;
  NVIC->IP[29] = 0x10;//���ȼ�1
  NVIC->ISER[0] |= 0x01<<29;
  TIM3->CR1 |= 0x01<<0;//ʹ��
	
}

bool STEPPER::AutoHoming(){
	if(!Status.TestIfEnable()) //δʹ��
		EnableStepper();
	
	TestMove(2,0,AXIS_Z_MAX_POSITION*1.2);
	TestMove(1,1,AXIS_Y_MAX_POSITION*1.2);
	TestMove(0,1,AXIS_X_MAX_POSITION*1.2);
	

	PROB_ENABLE(1);
	Delay_Ms(2000);
	
	TestMove(0,0,AXIS_X_RETURN_LENGTH); //X����΢�ƶ�һЩ���ͷſ���
	TestMove(1,0,AXIS_Y_RETURN_LENGTH); //Y����΢�ƶ�һЩ���ͷſ���
	
	TestMove(0,0,AXIS_X_MAX_POSITION/2.0 - 30.0); //X���ƶ����м�
	TestMove(1,0,AXIS_Y_MAX_POSITION/2.0 - 30.0); //Y���ƶ����м�
	TestMove(2,1,AXIS_Z_RETURN_LENGTH); //Z�ᷴ���ƶ�������̽��
	
	TestMove(2,0,10.0); //Z�����10mm
	
	TestMove(0,1,AXIS_X_MAX_POSITION/2.0 - 30.0); //X���ƶ������
	TestMove(1,1,AXIS_Y_MAX_POSITION/2.0 - 30.0); //Y���ƶ������
	
	Status.SetPosition(0.0,0); //X������Ϊ0mm
	Status.SetPosition(0.0,1); //Y������Ϊ0mm
	Status.SetPosition(0.0,2); //Z������Ϊ0mm
	Status.SetPosition(0.0,3); //A������Ϊ0mm
	Status.SetPosition(0.0,4); //B������Ϊ0mm
	
// y A
//	 |
// PD|- - - -PC 
//	 |       |
//	 |       
// PA|       PB
//   +----------->
//	            x
							
	//����XY�ᶼλ�����
	double TestPoint[5] = {0.0, 0.0, 0.0, 0.0, 10.0}; //5������������
	//���Կ�ʼ���Ը߶�
	//���Ե�A (X,Y) = (0,0)
	TestMove(2,1,15.0); //Z�ᷴ���ƶ�15mm������̽��
	TestPoint[0] = fabsl(Status.GetPosition(2)); //��ȡA��λ����Ϣ
	TestMove(2,0,TestPoint[0]); //Z��ص�ԭ���ĸ߶�
	
	//����XY��λ��A
	//���Ե�B (X,Y) = (Max,0)
	TestMove(0,0,AXIS_X_MAX_POSITION - 30.0); //X���ƶ����յ�
	TestMove(2,1,15.0); //Z�ᷴ���ƶ�15mm������̽��
	TestPoint[1] = fabsl(Status.GetPosition(2)); //��ȡB��λ����Ϣ
	TestMove(2,0,TestPoint[1]); //Z��ص�ԭ���ĸ߶�
//	TestMove(0,1,AXIS_X_MAX_POSITION - 30.0); //X���ƶ������
	
	//����XY��λ��B
	//���Ե�C (X,Y) = (0,Max)
	TestMove(1,0,AXIS_Y_MAX_POSITION - 30.0); //Y���ƶ����յ�
	TestMove(2,1,15.0); //Z�ᷴ���ƶ�15mm������̽��
	TestPoint[2] = fabsl(Status.GetPosition(2)); //��ȡC��λ����Ϣ
	TestMove(2,0,TestPoint[2]); //Z��ص�ԭ���ĸ߶�
//	TestMove(1,1,AXIS_Y_MAX_POSITION - 30.0); //Y���ƶ������
	
	//����XY��λ��C
	//���Ե�D (X,Y) = (Max,Max)
	TestMove(0,1,AXIS_X_MAX_POSITION - 30.0); //X���ƶ�����
//	TestMove(1,0,AXIS_Y_MAX_POSITION - 30.0); //Y���ƶ����յ�
	TestMove(2,1,15.0); //Z�ᷴ���ƶ�15mm������̽��
	TestPoint[3] = fabsl(Status.GetPosition(2)); //��ȡD��λ����Ϣ
	TestMove(2,0,TestPoint[3]); //Z��ص�ԭ���ĸ߶�
//	TestMove(0,1,AXIS_X_MAX_POSITION - 30.0); //X���ƶ������
	TestMove(1,1,AXIS_Y_MAX_POSITION - 30.0); //Y���ƶ������
	
	//����XY�ᶼλ�����
	
	double Min = GetMin(TestPoint,5);
	for(u32 i=0;i<5;i++)
		TestPoint[i] -= Min;
	
	double Max = GetMax(TestPoint,5);
	if(Max < MAX_DIFFERENCE) //��𲻴���Ϊ�Ѿ���ƽ
		Status.Leveling = 1;
	else //û�е�ƽ
		Status.Leveling = 0;
	
	for(u32 i=0;i<5;i++) //��¼��ƽ״��
		Status.TestPoint[i] = TestPoint[i];
	
	Delay_Ms(1000); //�ӳ�һ��
	PROB_ENABLE(0); //�ջ�̽��
	Delay_Ms(2000); //�ȴ�̽���ջ�
	
	TestMove(2,1,Min+Max+HIGH_PROB); //Z��ص����߶�
	
	double Posi[5] = {0.0 , 0.0 , 0.0 , 0.0 , 0.0};
	if(Status.InitPosition(Posi)) //�������꼰�ѹ�λ��־
		return 1;
	
	return 0;
}

void STEPPER::SectionMove(double Position,u8 Selection){	
	if(Selection > 4) return;
	double Delta = 0;
	bool Direction = 0;
	switch(Selection){
		case 0:
			if(Position > AXIS_X_MAX_POSITION) Position = AXIS_X_MAX_POSITION;
			if(Position < AXIS_X_MIN_POSITION) Position = AXIS_X_MIN_POSITION;
			Delta = Position - Status.Position[0];
			Direction = 0;
			if(Delta < 0){
				Direction = 1;
				Delta = -Delta;
			}
			TestMove(0,Direction,Delta);
			break;
		case 1:
			if(Position > AXIS_Y_MAX_POSITION) Position = AXIS_Y_MAX_POSITION;
			if(Position < AXIS_Y_MIN_POSITION) Position = AXIS_Y_MIN_POSITION;
			Delta = Position - Status.Position[1];
			Direction = 0;
			if(Delta < 0){
				Direction = 1;
				Delta = -Delta;
			}
			TestMove(1,Direction,Delta);
			break;		
		case 2:
			if(Position > AXIS_Z_MAX_POSITION) Position = AXIS_Z_MAX_POSITION;
			if(Position < AXIS_Z_MIN_POSITION) Position = AXIS_Z_MIN_POSITION;
			Delta = Position - Status.Position[2];
			Direction = 0;
			if(Delta < 0){
				Direction = 1;
				Delta = -Delta;
			}
			TestMove(2,Direction,Delta);
			break;
		case 3:
			Delta = Position - Status.Position[3];
			if(Delta > 200.0) Delta = 200.0;
			if(Delta < -200.0) Delta = -200.0;
			Direction = 0;
			if(Delta < 0){
				Direction = 1;
				Delta = -Delta;
			}
			TestMove(3,Direction,Delta);
			break;
		case 4:
			Delta = Position - Status.Position[4];
			if(Delta > 200.0) Delta = 200.0;
			if(Delta < -200.0) Delta = -200.0;
			Direction = 0;
			if(Delta < 0){
				Direction = 1;
				Delta = -Delta;
			}
			TestMove(4,Direction,Delta);
			break;
	}
}

void STEPPER::TestMove(u8 Selection,bool Direction,double Length){
	if(!Status.TestIfEnable()) //δʹ��
		EnableStepper();
	
	u32 HALFPLUSEWIDTH = 3125/(u32)VELOCITY_HOMING; //[us] �ɹ�λ�ٶ�ȷ���������
	if(Length < 0)
		Length = -Length;

	if(Selection == 0){ //X��
		u32 Steps = Length/X_STEP_LENGTH; //���㲽��
		STEPPER_X_DIRECTION(Direction); //���÷���
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //�ƶ�
			if(Status.TestMinEndPointX()&&Direction) break;
			if(Status.TestMaxEndPointX()&&!Direction) break;
			
			STEPPER_X_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_X_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToX(Direction);
		}
	}
	else if(Selection == 1){ //Y��
		u32 Steps = Length/Y_STEP_LENGTH; //���㲽��
		STEPPER_Y_DIRECTION(Direction); //���÷���
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //�ƶ�
			if(Status.TestMinEndPointY()&&Direction) break;
			if(Status.TestMaxEndPointY()&&!Direction) break;
			
			STEPPER_Y_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_Y_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToY(Direction);
		}
	}
	else if(Selection == 2){ //Z��
		u32 Steps = Length/Z_STEP_LENGTH; //���㲽��
		STEPPER_Z_DIRECTION(Direction); //���÷���
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //�ƶ�
			if(Status.TestMinEndPointZ()&&Direction) break;
			if(Status.TestMaxEndPointZ()&&!Direction) break;
			
			STEPPER_Z_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_Z_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToZ(Direction);
		}
	}
	else if(Selection == 3){ //A��
		if(Length > 200)
			Length = 200;
		u32 Steps = Length/A_STEP_LENGTH; //���㲽��
		STEPPER_A_DIRECTION(Direction); //���÷���
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //�ƶ�
			STEPPER_A_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_A_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToA(Direction);
		}
	}
	else if(Selection == 4){ //B��
		if(Length > 200)
			Length = 200;
		u32 Steps = Length/B_STEP_LENGTH; //���㲽��
		STEPPER_B_DIRECTION(Direction); //���÷���
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //�ƶ�
			STEPPER_B_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_B_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToB(Direction);
		}
	}
}

void STEPPER::EnableStepper(){
	/*ʹ�����в������
		��ʹ�ܲ���������ٺ�������Z��ʧ���ƶ�
		Ҳ����˵������̻�������뼶���ӳ�
		һ����ڿ���ʱ���ô˹���
	*/
	STEPPER_ENABLE(1); //ʹ�ܲ������
	Delay_Ms(10);
//	LOCK_ENABLE(0); //����ƶ�
//	Delay_Ms(200);
	Status.SetEnable(1); //����ʹ�ܱ�־
}

bool STEPPER::DisableStepper(){
	/*ʧ�����в������
		����Z��ʧ���ƶ��������ٺ�����ʧ�ܲ������
		Ҳ����˵�˹��̻�������뼶�ӳ�
		һ����ڹػ�ǰ���ô˹���
	*/
	if(Planner.L2RestAmount != L2_SIZE) //L2����ǿղ��ɹر�
		return 0;
	if(Stepper.L1RestAmount != L1_SIZE) //L1����ǿղ��ɹر�
		return 0;
	if(Stepper.FAStatus != 'I') //�����в��ɹر�
		return 0;
	
//	LOCK_ENABLE(1);
//	Delay_Ms(200);
	STEPPER_ENABLE(0);
	Delay_Ms(10);
	Status.SetEnable(0); //���ʹ�ܱ�־
	return 1;
	
}

void STEPPER::SetAllDirection(bool Direction[5]){
	/*���÷���
		����Direction
	  MSB 7 6 5 4 3 2 1 0 LSB
	    \ | | | | | | | | /
		  	N N N B A Z Y X
	  N - ����/δʹ��
	  B - B��
		A - A��
	  Z - Z��
	  Y - Y��
	  X - X��
		�ڶ�Ӧ�����Selection������Ч(==True)ʱ
	  ĳһBit - 0/1<=>(��Ӧ����)����Ϊ����0/����1��ʵ�ʷ����ɻ������þ���	
	*/
	STEPPER_X_DIRECTION(Direction[0]);
	
	STEPPER_Y_DIRECTION(Direction[1]);
	
	STEPPER_Z_DIRECTION(Direction[2]);
	
	STEPPER_A_DIRECTION(Direction[3]);
	
	STEPPER_B_DIRECTION(Direction[4]);
}

void STEPPER::GeneratePluse(u8 Selection){
	/*�����������ɿ�������
	
		����Selection
	  MSB 7 6 5 4 3 2 1 0 LSB
	    \ | | | | | | | | /
		  	N N N B A Z Y X
	  N - ����/δʹ��
	  B - B��
		A - A��
	  Z - Z��
	  Y - Y��
	  X - X��
	  ��һBit - 0/1<=>(��Ӧ����)����������/��������
	
		�Ӳ���Selection�н�������Ҫ�����������
		�������������ز�������ʱ��2
		(��ʱ��3�ĸ����ж��в��������½���)
	*/
	if(Selection & 0x01){ //X��
		if(!PtrLauncher->Direction[0] && !Status.TestMaxEndPointX()){ //������δ������λ
			if(VirtualStep[0] >= 0){ //û����λ��
				Status.AddOneStepToX(Stepper.PtrLauncher->Direction[0]);
				STEPPER_X_PLUSE(1);
			}
			else //������λ��
				if(VirtualStep[0] < 0) VirtualStep[0]++;
		}
		else if(!PtrLauncher->Direction[0] && Status.TestMaxEndPointX()){ //�����ҵ�����λ
			VirtualStep[0]++; //������λ��
		}
		else if(PtrLauncher->Direction[0] && !Status.TestMinEndPointX()){ //������δ������λ
			if(VirtualStep[0] <= 0){ //û����λ��
				Status.AddOneStepToX(Stepper.PtrLauncher->Direction[0]);
				STEPPER_X_PLUSE(1);
			}
			else //������λ��
				if(VirtualStep[0] > 0) VirtualStep[0]--;
		}
		else if(PtrLauncher->Direction[0] && Status.TestMinEndPointX()){ //�����ҵ�����λ
			VirtualStep[0]--; //������λ��
		}
	}
	else{
			STEPPER_X_PLUSE(0);
	}
	
	if(Selection & 0x02){ //Y��
		if(!PtrLauncher->Direction[1] && !Status.TestMaxEndPointY()){ //������δ������λ
			if(VirtualStep[1] >= 0){ //û����λ��
				Status.AddOneStepToY(Stepper.PtrLauncher->Direction[1]);
				STEPPER_Y_PLUSE(1);
			}
			else //������λ��
				if(VirtualStep[1] < 0) VirtualStep[1]++;
		}
		else if(!PtrLauncher->Direction[1] && Status.TestMaxEndPointY()){ //�����ҵ�����λ
			VirtualStep[1]++; //������λ��
		}
		else if(PtrLauncher->Direction[1] && !Status.TestMinEndPointY()){ //������δ������λ
			if(VirtualStep[1] <= 0){ //û����λ��
				Status.AddOneStepToY(Stepper.PtrLauncher->Direction[1]);
				STEPPER_Y_PLUSE(1);
			}
			else //������λ��
				if(VirtualStep[1] > 0) VirtualStep[1]--;
		}
		else if(PtrLauncher->Direction[1] && Status.TestMinEndPointY()){ //�����ҵ�����λ
			VirtualStep[1]--; //������λ��
		}
	}
	else{
			STEPPER_Y_PLUSE(0);
	}
	
	if(Selection & 0x04){ //Z��
		if(!PtrLauncher->Direction[2] && !Status.TestMaxEndPointZ()){ //������δ������λ
			if(VirtualStep[2] >= 0){ //û����λ��
				Status.AddOneStepToZ(Stepper.PtrLauncher->Direction[2]);
				STEPPER_Z_PLUSE(1);
			}
			else //������λ��
				if(VirtualStep[2] < 0) VirtualStep[2]++;
		}
		else if(!PtrLauncher->Direction[2] && Status.TestMaxEndPointZ()){ //�����ҵ�����λ
			VirtualStep[2]++; //������λ��
		}
		else if(PtrLauncher->Direction[2] && !Status.TestMinEndPointZ()){ //������δ������λ
			if(VirtualStep[2] <= 0){ //û����λ��
				Status.AddOneStepToZ(Stepper.PtrLauncher->Direction[2]);
				STEPPER_Z_PLUSE(1);
			}
			else //������λ��
				if(VirtualStep[2] > 0) VirtualStep[2]--;
		}
		else if(PtrLauncher->Direction[2] && Status.TestMinEndPointZ()){ //�����ҵ�����λ
			VirtualStep[2]--; //������λ��
		}
	}
	else{
			STEPPER_Z_PLUSE(0);
	}

	
/*ע�͵�������û����λ�������ƶ��������Ҫ�˹��ܿ��Խ����滻
	if(Selection & 0x01){ //X��
		Status.AddOneStepToX(Stepper.PtrLauncher->Direction[0]);
		STEPPER_X_PLUSE(1);
	}
	else{
		STEPPER_X_PLUSE(0);
	}
	
	if(Selection & 0x02){ //Y��
		Status.AddOneStepToY(Stepper.PtrLauncher->Direction[1]);
		STEPPER_Y_PLUSE(1);
	}
	else{
		STEPPER_Y_PLUSE(0);
	}
	
	if(Selection & 0x04){ //Z��
		Status.AddOneStepToZ(Stepper.PtrLauncher->Direction[2]);
		STEPPER_Z_PLUSE(1);
	}
	else{
		STEPPER_Z_PLUSE(0);
	}*/
	
	if(Selection & 0x08){ //A��
		Status.AddOneStepToA(Stepper.PtrLauncher->Direction[3]);
		STEPPER_A_PLUSE(1);
	}
	else{
		STEPPER_A_PLUSE(0);
	}
	
	if(Selection & 0x10){ //B��
		Status.AddOneStepToB(Stepper.PtrLauncher->Direction[4]);
		STEPPER_B_PLUSE(1);
	}
	else{
		STEPPER_B_PLUSE(0);
	}
	
}



