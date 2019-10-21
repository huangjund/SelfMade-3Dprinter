#include "Planner.h"


PLANNER Planner;

void PLANNER::Init(){
	//ѭ��ָ���ʼ��
	L2_Cache[L2_SIZE-1].Next = L2_Cache;
	L2_Cache[0].Pror = (L2_Cache+L2_SIZE-1);
	for(u32 i=0;i<L2_SIZE-1;i++)
		(L2_Cache+i)->Next = L2_Cache+i+1;
	for(u32 i=1;i<L2_SIZE;i++)
		(L2_Cache+i)->Pror = L2_Cache+i-1;
	
	//�ų�ʼ��
	PtrLauncher = L2_Cache;
	PtrLoader = L2_Cache;
	
	//�����ٶȳ�ʼ��
	AccelerationMax = DEFAULT_ACCELERATION;
	
	//�������ݳ�ʼ��
	for(u32 i=0;i<L2_SIZE;i++){
		for(u32 c=0;c<5;c++){
			L2_Cache[i].Position[c] = 0;
			L2_Cache[i].Length[c] = 0;
			L2_Cache[i].Step[c] = 0;
			L2_Cache[i].TPluse[c] = 0;
		}
		
		L2_Cache[i].SumStep = 0;
		L2_Cache[i].VectorSize = 0;
		L2_Cache[i].Accelerate = 0;
		L2_Cache[i].ConstSpeed = 0;
		L2_Cache[i].Decelerate = 0;
		L2_Cache[i].VelocityIn = 0;
		L2_Cache[i].VelocityOut = 0;
		L2_Cache[i].VelocityMax = 0;
		L2_Cache[i].Full = 0;
	}
	
	L2RestAmount = L2_SIZE;
	
}

bool PLANNER::Load(double Position[5],float VelocityMax){
	
	if(PtrLoader->Full) return 0; //����������0
	
	if(!Status.TestIfHomed()) return 0; //��δ��λ������0
	
//д������ƶ��ٶ�
	PtrLoader->VelocityMax = VelocityMax;
	PtrLoader->SubVelocityMax = VelocityMax;
	
	if(Position[0] > AXIS_X_MAX_POSITION) Position[0] = AXIS_X_MAX_POSITION; //X�����ƶ�
	if(Position[0] < AXIS_X_MIN_POSITION) Position[0] = AXIS_X_MIN_POSITION; //X�����ƶ�
	if(Position[1] > AXIS_Y_MAX_POSITION) Position[1] = AXIS_Y_MAX_POSITION; //Y�����ƶ�
	if(Position[1] < AXIS_Y_MIN_POSITION) Position[1] = AXIS_Y_MIN_POSITION; //Y�����ƶ�
	if(Position[2] > AXIS_Z_MAX_POSITION) Position[2] = AXIS_Z_MAX_POSITION; //Z�����ƶ�
	if(Position[2] < AXIS_Z_MIN_POSITION) Position[2] = AXIS_Z_MIN_POSITION; //Z�����ƶ�
			
	
//�����ƶ�����
	double LengthX = Position[0]-PtrLoader->Pror->Position[0]; //X���ƶ�����
	double LengthY = Position[1]-PtrLoader->Pror->Position[1]; //Y���ƶ�����
	double LengthZ = Position[2]-PtrLoader->Pror->Position[2]; //Z���ƶ�����
	double LengthA = Position[3]-PtrLoader->Pror->Position[3]; //A���ƶ�����
	double LengthB = Position[4]-PtrLoader->Pror->Position[4]; //B���ƶ�����
	
//���㲽��
	PtrLoader->Step[0] = LengthX/X_STEP_LENGTH; //X�Ჽ��
	PtrLoader->Step[1] = LengthY/Y_STEP_LENGTH; //Y�Ჽ��
	PtrLoader->Step[2] = LengthZ/Z_STEP_LENGTH; //Z�Ჽ��
	PtrLoader->Step[3] = LengthA/A_STEP_LENGTH; //A�Ჽ��
	PtrLoader->Step[4] = LengthB/B_STEP_LENGTH; //B�Ჽ��

	if(!PtrLoader->Step[0] && 
		!PtrLoader->Step[1] && 
		!PtrLoader->Step[2] && 
		!PtrLoader->Step[3] && 
		!PtrLoader->Step[4]) //�����ƶ�
			return 1;
	
	if(PtrLoader->Step[3] && VelocityMax>80.0){
		VelocityMax = 80.0;
		PtrLoader->VelocityMax = 80.0;
		PtrLoader->SubVelocityMax = 80.0;
	}
	
//���¼����ƶ�����(������������)
	LengthX = PtrLoader->Step[0]*X_STEP_LENGTH; //X���ƶ�����
	LengthY = PtrLoader->Step[1]*Y_STEP_LENGTH; //Y���ƶ�����
	LengthZ = PtrLoader->Step[2]*Z_STEP_LENGTH; //Z���ƶ�����
	LengthA = PtrLoader->Step[3]*A_STEP_LENGTH; //A���ƶ�����
	LengthB = PtrLoader->Step[4]*B_STEP_LENGTH; //B���ƶ�����
	
	PtrLoader->Length[0] = LengthX;
	PtrLoader->Length[1] = LengthY;
	PtrLoader->Length[2] = LengthZ;
	PtrLoader->Length[3] = LengthA;
	PtrLoader->Length[4] = LengthB;
	
//�ɲ�������Ŀ���������
	PtrLoader->Position[0] = PtrLoader->Pror->Position[0] + LengthX; //X���������
	PtrLoader->Position[1] = PtrLoader->Pror->Position[1] + LengthY; //Y���������
	PtrLoader->Position[2] = PtrLoader->Pror->Position[2] + LengthZ; //Z���������
	PtrLoader->Position[3] = PtrLoader->Pror->Position[3] + LengthA; //A���������
	PtrLoader->Position[4] = PtrLoader->Pror->Position[4] + LengthB; //B���������

//���������������ܽ�����
	PtrLoader->VectorSize = sqrtl( //������������
		LengthX*LengthX + 
		LengthY*LengthY + 
		LengthZ*LengthZ + 
		LengthA*LengthA + 
		LengthB*LengthB );

	PtrLoader->SumStep = (int)sqrtl( //�����ܽ�����
		PtrLoader->Step[0]*PtrLoader->Step[0] +
		PtrLoader->Step[1]*PtrLoader->Step[1] +
		PtrLoader->Step[2]*PtrLoader->Step[2] +
		PtrLoader->Step[3]*PtrLoader->Step[3] +
		PtrLoader->Step[4]*PtrLoader->Step[4] );
	
	
	for(u32 i=0;i<5;i++){ //��������������
		if(PtrLoader->Step[i]) //��Ҫ�ƶ�
			PtrLoader->TPluse[i] = (float)PtrLoader->SumStep/(float)PtrLoader->Step[i]; //�������
		else
			PtrLoader->TPluse[i] = 0.0f;
		
		if(PtrLoader->TPluse[i] < 0) //ȡ����ֵ
			PtrLoader->TPluse[i] = -PtrLoader->TPluse[i];
	}
	
	PtrLoader->SumStep++; //�����ܽ�����
	
	CaclVelocityAndClock(); //�����ٶ��������
	
	L2RestAmount--; //L2ʣ�໺��������һ
	
	PtrLoader->Full = 1; //��ǰ��������װ��
	PtrLoader = PtrLoader->Next; //ָ����һ������
	return 1;
}

bool PLANNER::Launch(STEPPER::_L1 *Pointer){
	if(!PtrLauncher->Full) return 0;
	if(Pointer->Full) return 0;
	for(u32 i=0;i<5;i++){
		if(PtrLauncher->Step[i] < 0){ //����
			Pointer->Direction[i] = 1;
			Pointer->Step[i] = (u64)(-PtrLauncher->Step[i]);
		}
		else{ //����
			Pointer->Direction[i] = 0;
			Pointer->Step[i] = (u64)PtrLauncher->Step[i];
		}
		Pointer->TPluse[i] = PtrLauncher->TPluse[i];
		Pointer->NextPluseClock[i] = PtrLauncher->TPluse[i];
	}
	Pointer->ClockCounter = 0;
	Pointer->Accelerate = PtrLauncher->Accelerate;
	Pointer->ConstSpeed = PtrLauncher->ConstSpeed;
	Pointer->Decelerate = PtrLauncher->Decelerate;

		
	
//�������״̬���ڱ仯����
	float CycleIn = 56250/PtrLauncher->VelocityIn; //��������
//	float CycleOut = 56250/PtrLauncher->VelocityOut; //�л�����
//	float CycleConst = 56250/PtrLauncher->VelocityMax; //��������
	
	Pointer->VelocityIn = PtrLauncher->VelocityIn; //�����ٶ�
	Pointer->VelocityOut = PtrLauncher->VelocityOut; //�л��ٶ�
	
	Pointer->CycleIn = CycleIn;
	
	
	if(PtrLauncher->Accelerate && !PtrLauncher->ConstSpeed && !PtrLauncher->Decelerate){ //���м���
		if(!PtrLauncher->Accelerate){
			Pointer->Acceleration = 0.0f;
		}
		else{
			Pointer->Acceleration = 
				(PtrLauncher->VelocityOut*PtrLauncher->VelocityOut - PtrLauncher->VelocityIn*PtrLauncher->VelocityIn)
				/2.0f/(float)PtrLauncher->Accelerate/SYNTHETIC_LENGTH;
		}
		Pointer->Deceleration = 0.0f;
	}
	else if(!PtrLauncher->Accelerate && !PtrLauncher->ConstSpeed && PtrLauncher->Decelerate){ //���м���
		Pointer->Acceleration = 0.0f;
		
		if(!PtrLauncher->Decelerate){
			Pointer->Deceleration = 0.0f;
		}
		else{
			Pointer->Deceleration = 
				(PtrLauncher->VelocityOut*PtrLauncher->VelocityOut - PtrLauncher->VelocityIn*PtrLauncher->VelocityIn)
				/2.0f/(float)PtrLauncher->Decelerate/SYNTHETIC_LENGTH;
		}
	}
	else{ //����-(����)-����
		if(!PtrLauncher->Accelerate){
			Pointer->Acceleration = 0.0f;
		}
		else{
			Pointer->Acceleration = 
				(PtrLauncher->VelocityMax*PtrLauncher->VelocityMax - PtrLauncher->VelocityIn*PtrLauncher->VelocityIn)
				/2.0f/(float)PtrLauncher->Accelerate/SYNTHETIC_LENGTH;
		}
		
		if(!PtrLauncher->Decelerate){
			Pointer->Deceleration = 0.0f;
		}
		else{
			Pointer->Deceleration = 
				(PtrLauncher->VelocityOut*PtrLauncher->VelocityOut - PtrLauncher->VelocityMax*PtrLauncher->VelocityMax)
				/2.0f/(float)PtrLauncher->Decelerate/SYNTHETIC_LENGTH;
		}
	}
	
	if(Pointer->Deceleration < 0) Pointer->Deceleration = -Pointer->Deceleration;
	if(Pointer->Acceleration < 0) Pointer->Acceleration = -Pointer->Acceleration;
	
	L2RestAmount++; //L2ʣ�໺��������һ
	
	PtrLauncher->Full = 0; //��ʹ�ã�����ǿձ�־ 
	PtrLauncher = PtrLauncher->Next; //�ƶ�ָ�� 
	
	
	Stepper.L1RestAmount--; //L1ʣ�໺��������һ	
	
	Pointer->Full = 1;
	return 1;
}

void PLANNER::CaclVelocityAndClock(){
	if(PtrLoader == PtrLauncher){ //��һ��
		PtrLoader->VelocityIn = VELOCITY_START; //�����ٶȵ�����ʼ�ٶ�
		PtrLoader->VelocityOut = VELOCITY_START; //�л��ٶȵ�����ʼ�ٶ�
		CaclClock(PtrLoader); //���������
	}
	else if(Status.TestIfIDP()){ //���ö�̬�滮���ǵ�һ�Σ����Կ�ʼ����
		PtrLoader->VelocityOut = VELOCITY_START; //�л��ٶȵ�����ʼ�ٶ�
	/*�������Ҫ���������ϵĻ��ݣ�ʹ�����������滻��һ���̶��ϻή���ȶ���
		CaclVelocity(PtrLoader->Pror);
		CaclClock(PtrLoader->Pror);
		CaclClock(PtrLoader);
	*/
//---------------------------------����--------------------------------//
		for(_L2 *Travel=PtrLauncher;Travel!=PtrLoader;Travel=Travel->Next){ //ȫ�����¼����л��ٶ�
			CaclVelocity(Travel); //�����л��ٶ�		
		}
		
		float Vmax = 0.0f; //����ٶ�
		float Vmin = 0.0f; //��С�ٶ�
		for(_L2 *Travel=PtrLoader;Travel!=PtrLauncher;Travel=Travel->Pror){ //����
			CaclVelocityIn(Travel,Vmax,Vmin);		
			if(Travel->VelocityIn < Vmax*.999f){ //�ٶ�����Ҫ��
				continue; //����
			}
			else{ //�ٶȲ�����Ҫ��(�ں�ǿ������)
				Travel->VelocityIn = Vmax; //���������ٶ�
				Travel->Pror->VelocityOut = Vmax; //����ǰһ���л��ٶ�
			}
		}

		for(_L2 *Travel=PtrLoader;true;Travel=Travel->Pror){ //ȫ�����¼�������� 
			CaclClock(Travel); //���������
			if(Travel == PtrLauncher) break;
		}	
//---------------------------------------------------------------------//
	}
	else{ //���ö�̬�滮���ǵ�һ�Σ�ͬ��һ��һ��
		PtrLoader->VelocityIn = VELOCITY_START; //�����ٶȵ�����ʼ�ٶ�
		PtrLoader->VelocityOut = VELOCITY_START; //�л��ٶȵ�����ʼ�ٶ�
		CaclClock(PtrLoader); //���������
	}
}

void PLANNER::CaclVelocity(_L2 *Pointer){
	float DotProduct =  //���
		Pointer->Length[0]*Pointer->Next->Length[0] +
		Pointer->Length[1]*Pointer->Next->Length[1] +
		Pointer->Length[2]*Pointer->Next->Length[2] +
		Pointer->Length[3]*Pointer->Next->Length[3] +
		Pointer->Length[4]*Pointer->Next->Length[4];
	
	float CosTheta = DotProduct / Pointer->VectorSize / Pointer->Next->VectorSize; //����
		
	if(CosTheta >= 0.99f) //���򲻱�
		Pointer->VelocityOut = Pointer->VelocityMax; //����ٶ�
	else{ //����ı�
	
		Pointer->VelocityOut = sqrtf(SYNTHETIC_LENGTH*AccelerationMax/sqrtf(2.0f-2.0f*CosTheta));
		if(Pointer->VelocityOut < VELOCITY_START) //�ǶȽϴ�
			Pointer->VelocityOut = VELOCITY_START; 
		else if(Pointer->VelocityOut > Pointer->VelocityMax) //�ǶȽ�С
			Pointer->VelocityOut = Pointer->VelocityMax;
	}
	
	Pointer->Next->VelocityIn = Pointer->VelocityOut;
}

void PLANNER::CaclVelocityIn(_L2 *Pointer,float &Vmax,float &Vmin){
	Vmax = sqrtf(
		Pointer->VelocityOut*Pointer->VelocityOut +
		2.0f*AccelerationMax*(float)Pointer->VectorSize);
	/*ע�͵������Ǽ�����С�ٶȣ��ڻ���������
	Vmin = sqrtf(
		Pointer->VelocityOut*Pointer->VelocityOut -
		2.0f*AccelerationMax*(float)Pointer->VectorSize);
	*/
	Vmin = 0.0f;
}


void PLANNER::CaclClock(_L2 *Pointer){
	float Vpeak = sqrtf( //��������ٶ�/���۷�ֵ�ٶ�
		(2.0f*AccelerationMax*Pointer->VectorSize +
		Pointer->VelocityIn*Pointer->VelocityIn +
	  Pointer->VelocityOut*Pointer->VelocityOut)/2.0f);
	
	Pointer->VelocityMax = Pointer->SubVelocityMax; //�ָ�����ٶ�(�����ڻ����б�����)
	
	if(Pointer->VelocityOut>Pointer->VelocityIn && Vpeak<Pointer->VelocityOut*1.001f){ //һֱ����
		Pointer->VelocityMax = Pointer->VelocityOut; //����ٶ�=�л��ٶ�
		Pointer->Accelerate = Pointer->SumStep; //���в���ȫ�����ڼ���
		Pointer->ConstSpeed = 0;
		Pointer->Decelerate = 0;
	}
	else if(Pointer->VelocityOut<Pointer->VelocityIn && Vpeak<Pointer->VelocityIn*1.001f){ //һֱ����
		Pointer->VelocityMax = Pointer->VelocityIn; //����ٶ�=�����ٶ�
		Pointer->Accelerate = 0;
		Pointer->ConstSpeed = 0;
		Pointer->Decelerate = Pointer->SumStep; //���в���ȫ�����ڼ���
	}
	else if(Pointer->VelocityMax>Vpeak){ //����-����(���۷�ֵ�ٶȽϸߣ������ٽ׶�)
		Pointer->VelocityMax = Vpeak; //����ٶ�=���۷�ֵ�ٶ�
		Pointer->Accelerate = (u64)( 
			(Pointer->VelocityMax*Pointer->VelocityMax - Pointer->VelocityIn*Pointer->VelocityIn)*Pointer->SumStep/2.0/AccelerationMax/Pointer->VectorSize);
		Pointer->Decelerate = (u64)(
			(Pointer->VelocityMax*Pointer->VelocityMax - Pointer->VelocityOut*Pointer->VelocityOut)*Pointer->SumStep/2.0/AccelerationMax/Pointer->VectorSize);
		while(Pointer->Accelerate+Pointer->Decelerate > Pointer->SumStep){
				Pointer->Accelerate--;
				Pointer->Decelerate--;
		}
		Pointer->ConstSpeed = Pointer->SumStep - Pointer->Accelerate - Pointer->Decelerate;
	}
	else{ //����-����-����
		Pointer->Accelerate = (u64)(
			(Pointer->VelocityMax*Pointer->VelocityMax - Pointer->VelocityIn*Pointer->VelocityIn)*Pointer->SumStep/2.0/AccelerationMax/Pointer->VectorSize);
		Pointer->Decelerate = (u64)(
			(Pointer->VelocityMax*Pointer->VelocityMax - Pointer->VelocityOut*Pointer->VelocityOut)*Pointer->SumStep/2.0/AccelerationMax/Pointer->VectorSize);
		while(Pointer->Accelerate+Pointer->Decelerate > Pointer->SumStep){
				Pointer->Accelerate--;
				Pointer->Decelerate--;
		}
		Pointer->ConstSpeed = Pointer->SumStep - Pointer->Accelerate - Pointer->Decelerate;
	}
}

