#include "Planner.h"


PLANNER Planner;

void PLANNER::Init(){
	//循环指针初始化
	L2_Cache[L2_SIZE-1].Next = L2_Cache;
	L2_Cache[0].Pror = (L2_Cache+L2_SIZE-1);
	for(u32 i=0;i<L2_SIZE-1;i++)
		(L2_Cache+i)->Next = L2_Cache+i+1;
	for(u32 i=1;i<L2_SIZE;i++)
		(L2_Cache+i)->Pror = L2_Cache+i-1;
	
	//门初始化
	PtrLauncher = L2_Cache;
	PtrLoader = L2_Cache;
	
	//最大加速度初始化
	AccelerationMax = DEFAULT_ACCELERATION;
	
	//缓存内容初始化
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
	
	if(PtrLoader->Full) return 0; //已满，返回0
	
	if(!Status.TestIfHomed()) return 0; //尚未归位，返回0
	
//写入最大移动速度
	PtrLoader->VelocityMax = VelocityMax;
	PtrLoader->SubVelocityMax = VelocityMax;
	
	if(Position[0] > AXIS_X_MAX_POSITION) Position[0] = AXIS_X_MAX_POSITION; //X饱和移动
	if(Position[0] < AXIS_X_MIN_POSITION) Position[0] = AXIS_X_MIN_POSITION; //X饱和移动
	if(Position[1] > AXIS_Y_MAX_POSITION) Position[1] = AXIS_Y_MAX_POSITION; //Y饱和移动
	if(Position[1] < AXIS_Y_MIN_POSITION) Position[1] = AXIS_Y_MIN_POSITION; //Y饱和移动
	if(Position[2] > AXIS_Z_MAX_POSITION) Position[2] = AXIS_Z_MAX_POSITION; //Z饱和移动
	if(Position[2] < AXIS_Z_MIN_POSITION) Position[2] = AXIS_Z_MIN_POSITION; //Z饱和移动
			
	
//计算移动距离
	double LengthX = Position[0]-PtrLoader->Pror->Position[0]; //X轴移动距离
	double LengthY = Position[1]-PtrLoader->Pror->Position[1]; //Y轴移动距离
	double LengthZ = Position[2]-PtrLoader->Pror->Position[2]; //Z轴移动距离
	double LengthA = Position[3]-PtrLoader->Pror->Position[3]; //A轴移动距离
	double LengthB = Position[4]-PtrLoader->Pror->Position[4]; //B轴移动距离
	
//计算步数
	PtrLoader->Step[0] = LengthX/X_STEP_LENGTH; //X轴步数
	PtrLoader->Step[1] = LengthY/Y_STEP_LENGTH; //Y轴步数
	PtrLoader->Step[2] = LengthZ/Z_STEP_LENGTH; //Z轴步数
	PtrLoader->Step[3] = LengthA/A_STEP_LENGTH; //A轴步数
	PtrLoader->Step[4] = LengthB/B_STEP_LENGTH; //B轴步数

	if(!PtrLoader->Step[0] && 
		!PtrLoader->Step[1] && 
		!PtrLoader->Step[2] && 
		!PtrLoader->Step[3] && 
		!PtrLoader->Step[4]) //无需移动
			return 1;
	
	if(PtrLoader->Step[3] && VelocityMax>80.0){
		VelocityMax = 80.0;
		PtrLoader->VelocityMax = 80.0;
		PtrLoader->SubVelocityMax = 80.0;
	}
	
//重新计算移动距离(步长的整数倍)
	LengthX = PtrLoader->Step[0]*X_STEP_LENGTH; //X轴移动距离
	LengthY = PtrLoader->Step[1]*Y_STEP_LENGTH; //Y轴移动距离
	LengthZ = PtrLoader->Step[2]*Z_STEP_LENGTH; //Z轴移动距离
	LengthA = PtrLoader->Step[3]*A_STEP_LENGTH; //A轴移动距离
	LengthB = PtrLoader->Step[4]*B_STEP_LENGTH; //B轴移动距离
	
	PtrLoader->Length[0] = LengthX;
	PtrLoader->Length[1] = LengthY;
	PtrLoader->Length[2] = LengthZ;
	PtrLoader->Length[3] = LengthA;
	PtrLoader->Length[4] = LengthB;
	
//由步数计算目标绝对坐标
	PtrLoader->Position[0] = PtrLoader->Pror->Position[0] + LengthX; //X轴绝对坐标
	PtrLoader->Position[1] = PtrLoader->Pror->Position[1] + LengthY; //Y轴绝对坐标
	PtrLoader->Position[2] = PtrLoader->Pror->Position[2] + LengthZ; //Z轴绝对坐标
	PtrLoader->Position[3] = PtrLoader->Pror->Position[3] + LengthA; //A轴绝对坐标
	PtrLoader->Position[4] = PtrLoader->Pror->Position[4] + LengthB; //B轴绝对坐标

//计算向量长度与总节拍数
	PtrLoader->VectorSize = sqrtl( //计算向量长度
		LengthX*LengthX + 
		LengthY*LengthY + 
		LengthZ*LengthZ + 
		LengthA*LengthA + 
		LengthB*LengthB );

	PtrLoader->SumStep = (int)sqrtl( //计算总节拍数
		PtrLoader->Step[0]*PtrLoader->Step[0] +
		PtrLoader->Step[1]*PtrLoader->Step[1] +
		PtrLoader->Step[2]*PtrLoader->Step[2] +
		PtrLoader->Step[3]*PtrLoader->Step[3] +
		PtrLoader->Step[4]*PtrLoader->Step[4] );
	
	
	for(u32 i=0;i<5;i++){ //计算各轴脉冲节拍
		if(PtrLoader->Step[i]) //需要移动
			PtrLoader->TPluse[i] = (float)PtrLoader->SumStep/(float)PtrLoader->Step[i]; //计算节拍
		else
			PtrLoader->TPluse[i] = 0.0f;
		
		if(PtrLoader->TPluse[i] < 0) //取绝对值
			PtrLoader->TPluse[i] = -PtrLoader->TPluse[i];
	}
	
	PtrLoader->SumStep++; //调整总节拍数
	
	CaclVelocityAndClock(); //计算速度与节拍数
	
	L2RestAmount--; //L2剩余缓存容量减一
	
	PtrLoader->Full = 1; //当前缓存体已装填
	PtrLoader = PtrLoader->Next; //指向下一缓存体
	return 1;
}

bool PLANNER::Launch(STEPPER::_L1 *Pointer){
	if(!PtrLauncher->Full) return 0;
	if(Pointer->Full) return 0;
	for(u32 i=0;i<5;i++){
		if(PtrLauncher->Step[i] < 0){ //反向
			Pointer->Direction[i] = 1;
			Pointer->Step[i] = (u64)(-PtrLauncher->Step[i]);
		}
		else{ //正向
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

		
	
//计算加速状态周期变化步长
	float CycleIn = 56250/PtrLauncher->VelocityIn; //切入周期
//	float CycleOut = 56250/PtrLauncher->VelocityOut; //切换周期
//	float CycleConst = 56250/PtrLauncher->VelocityMax; //匀速周期
	
	Pointer->VelocityIn = PtrLauncher->VelocityIn; //切入速度
	Pointer->VelocityOut = PtrLauncher->VelocityOut; //切换速度
	
	Pointer->CycleIn = CycleIn;
	
	
	if(PtrLauncher->Accelerate && !PtrLauncher->ConstSpeed && !PtrLauncher->Decelerate){ //仅有加速
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
	else if(!PtrLauncher->Accelerate && !PtrLauncher->ConstSpeed && PtrLauncher->Decelerate){ //仅有减速
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
	else{ //加速-(匀速)-减速
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
	
	L2RestAmount++; //L2剩余缓存容量加一
	
	PtrLauncher->Full = 0; //已使用，清除非空标志 
	PtrLauncher = PtrLauncher->Next; //移动指针 
	
	
	Stepper.L1RestAmount--; //L1剩余缓存容量减一	
	
	Pointer->Full = 1;
	return 1;
}

void PLANNER::CaclVelocityAndClock(){
	if(PtrLoader == PtrLauncher){ //第一段
		PtrLoader->VelocityIn = VELOCITY_START; //切入速度等于起始速度
		PtrLoader->VelocityOut = VELOCITY_START; //切换速度等于起始速度
		CaclClock(PtrLoader); //计算节拍数
	}
	else if(Status.TestIfIDP()){ //启用动态规划，非第一段，可以开始回溯
		PtrLoader->VelocityOut = VELOCITY_START; //切换速度等于起始速度
	/*如果不需要真正意义上的回溯，使用以下内容替换，一定程度上会降低稳定性
		CaclVelocity(PtrLoader->Pror);
		CaclClock(PtrLoader->Pror);
		CaclClock(PtrLoader);
	*/
//---------------------------------回溯--------------------------------//
		for(_L2 *Travel=PtrLauncher;Travel!=PtrLoader;Travel=Travel->Next){ //全部重新计算切换速度
			CaclVelocity(Travel); //计算切换速度		
		}
		
		float Vmax = 0.0f; //最大速度
		float Vmin = 0.0f; //最小速度
		for(_L2 *Travel=PtrLoader;Travel!=PtrLauncher;Travel=Travel->Pror){ //回溯
			CaclVelocityIn(Travel,Vmax,Vmin);		
			if(Travel->VelocityIn < Vmax*.999f){ //速度满足要求
				continue; //跳过
			}
			else{ //速度不满足要求，(内含强制收敛)
				Travel->VelocityIn = Vmax; //降低切入速度
				Travel->Pror->VelocityOut = Vmax; //降低前一段切换速度
			}
		}

		for(_L2 *Travel=PtrLoader;true;Travel=Travel->Pror){ //全部重新计算节拍数 
			CaclClock(Travel); //计算节拍数
			if(Travel == PtrLauncher) break;
		}	
//---------------------------------------------------------------------//
	}
	else{ //禁用动态规划，非第一段，同第一段一样
		PtrLoader->VelocityIn = VELOCITY_START; //切入速度等于起始速度
		PtrLoader->VelocityOut = VELOCITY_START; //切换速度等于起始速度
		CaclClock(PtrLoader); //计算节拍数
	}
}

void PLANNER::CaclVelocity(_L2 *Pointer){
	float DotProduct =  //点积
		Pointer->Length[0]*Pointer->Next->Length[0] +
		Pointer->Length[1]*Pointer->Next->Length[1] +
		Pointer->Length[2]*Pointer->Next->Length[2] +
		Pointer->Length[3]*Pointer->Next->Length[3] +
		Pointer->Length[4]*Pointer->Next->Length[4];
	
	float CosTheta = DotProduct / Pointer->VectorSize / Pointer->Next->VectorSize; //余弦
		
	if(CosTheta >= 0.99f) //方向不变
		Pointer->VelocityOut = Pointer->VelocityMax; //最大速度
	else{ //方向改变
	
		Pointer->VelocityOut = sqrtf(SYNTHETIC_LENGTH*AccelerationMax/sqrtf(2.0f-2.0f*CosTheta));
		if(Pointer->VelocityOut < VELOCITY_START) //角度较大
			Pointer->VelocityOut = VELOCITY_START; 
		else if(Pointer->VelocityOut > Pointer->VelocityMax) //角度较小
			Pointer->VelocityOut = Pointer->VelocityMax;
	}
	
	Pointer->Next->VelocityIn = Pointer->VelocityOut;
}

void PLANNER::CaclVelocityIn(_L2 *Pointer,float &Vmax,float &Vmin){
	Vmax = sqrtf(
		Pointer->VelocityOut*Pointer->VelocityOut +
		2.0f*AccelerationMax*(float)Pointer->VectorSize);
	/*注释的内容是计算最小速度，在回溯中无用
	Vmin = sqrtf(
		Pointer->VelocityOut*Pointer->VelocityOut -
		2.0f*AccelerationMax*(float)Pointer->VectorSize);
	*/
	Vmin = 0.0f;
}


void PLANNER::CaclClock(_L2 *Pointer){
	float Vpeak = sqrtf( //计算最大速度/理论峰值速度
		(2.0f*AccelerationMax*Pointer->VectorSize +
		Pointer->VelocityIn*Pointer->VelocityIn +
	  Pointer->VelocityOut*Pointer->VelocityOut)/2.0f);
	
	Pointer->VelocityMax = Pointer->SubVelocityMax; //恢复最大速度(可能在回溯中被更改)
	
	if(Pointer->VelocityOut>Pointer->VelocityIn && Vpeak<Pointer->VelocityOut*1.001f){ //一直加速
		Pointer->VelocityMax = Pointer->VelocityOut; //最大速度=切换速度
		Pointer->Accelerate = Pointer->SumStep; //所有步数全部用于加速
		Pointer->ConstSpeed = 0;
		Pointer->Decelerate = 0;
	}
	else if(Pointer->VelocityOut<Pointer->VelocityIn && Vpeak<Pointer->VelocityIn*1.001f){ //一直减速
		Pointer->VelocityMax = Pointer->VelocityIn; //最大速度=切入速度
		Pointer->Accelerate = 0;
		Pointer->ConstSpeed = 0;
		Pointer->Decelerate = Pointer->SumStep; //所有步数全部用于减速
	}
	else if(Pointer->VelocityMax>Vpeak){ //加速-减速(理论峰值速度较高，无匀速阶段)
		Pointer->VelocityMax = Vpeak; //最大速度=理论峰值速度
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
	else{ //加速-匀速-减速
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

