#include "Status.h"

STATUS Status;

void STATUS::Init(){
	
	Position[0] = 0.0;
	Position[1] = 0.0;
	Position[2] = 0.0;
	Position[3] = 0.0;
	Position[4] = 0.0;
	Planner.PtrLauncher->Pror->Position[0] = Position[0];
	Planner.PtrLauncher->Pror->Position[1] = Position[1];
	Planner.PtrLauncher->Pror->Position[2] = Position[2];
	Planner.PtrLauncher->Pror->Position[3] = Position[3];
	Planner.PtrLauncher->Pror->Position[4] = Position[4];
	Homed = 0;
	Enable = 1;
	IDP = 0;
}

bool STATUS::InitPosition(double Posi[5]){
	if(Planner.L2RestAmount != L2_SIZE) //L2缓存非空不可更改
		return 0;
	if(Stepper.L1RestAmount != L1_SIZE) //L1缓存非空不可更改
		return 0;
	if(Stepper.FAStatus != 'I') //运行中不可更改
		return 0;
	
	if(Posi[0] > AXIS_X_MAX_POSITION) Posi[0] = AXIS_X_MAX_POSITION;
	if(Posi[0] < AXIS_X_MIN_POSITION) Posi[0] = AXIS_X_MIN_POSITION;
	if(Posi[1] > AXIS_Y_MAX_POSITION) Posi[1] = AXIS_Y_MAX_POSITION;
	if(Posi[1] < AXIS_Y_MIN_POSITION) Posi[1] = AXIS_Y_MIN_POSITION;
	if(Posi[2] > AXIS_Z_MAX_POSITION) Posi[2] = AXIS_Z_MAX_POSITION;
	if(Posi[2] < AXIS_Z_MIN_POSITION) Posi[2] = AXIS_Z_MIN_POSITION;
	
	Position[0] = Posi[0];
	Position[1] = Posi[1];
	Position[2] = Posi[2];
	Position[3] = Posi[3];
	Position[4] = Posi[4];
	
	Planner.PtrLauncher->Pror->Position[0] = Position[0];
	Planner.PtrLauncher->Pror->Position[1] = Position[1];
	Planner.PtrLauncher->Pror->Position[2] = Position[2];
	Planner.PtrLauncher->Pror->Position[3] = Position[3];
	Planner.PtrLauncher->Pror->Position[4] = Position[4];
	
	Homed = 1; //设置已归位标志
	
	return 1;
}

bool STATUS::TestMaxEndPointX(){
	return TEST_X_MAX();
}

bool STATUS::TestMaxEndPointY(){
	return TEST_Y_MAX();
}

bool STATUS::TestMaxEndPointZ(){
	return TEST_Z_MAX();
}

bool STATUS::TestMinEndPointX(){
	return TEST_X_MIN();
}

bool STATUS::TestMinEndPointY(){
	return TEST_Y_MIN();
}

bool STATUS::TestMinEndPointZ(){
	return TEST_Z_MIN();
}



void STATUS::AddOneStepToX(bool Direction){
	if(Direction) 
		Position[0] -= X_STEP_LENGTH;
	else
		Position[0] += X_STEP_LENGTH;
}

void STATUS::AddOneStepToY(bool Direction){
	if(Direction) 
		Position[1] -= Y_STEP_LENGTH;
	else
		Position[1] += Y_STEP_LENGTH;
}

void STATUS::AddOneStepToZ(bool Direction){
	if(Direction) 
		Position[2] -= Z_STEP_LENGTH;
	else
		Position[2] += Z_STEP_LENGTH;
}

void STATUS::AddOneStepToA(bool Direction){
	if(Direction) 
		Position[3] -= A_STEP_LENGTH;
	else
		Position[3] += A_STEP_LENGTH;
}

void STATUS::AddOneStepToB(bool Direction){
	if(Direction) 
		Position[4] -= B_STEP_LENGTH;
	else
		Position[4] += B_STEP_LENGTH;
}


void STATUS::SetPosition(double Posi, u8 Selection){
	if(Selection > 4) return;
	Position[Selection] = Posi;
}


double STATUS::GetPosition(u8 Selection){
	if(Selection > 4) return 0;
	return Position[Selection];
}

bool STATUS::TestIfLevel(){
	return Leveling;
}

bool STATUS::TestIfHomed(){
	return Homed;
}

bool STATUS::TestIfEnable(){
	return Enable;
}

bool STATUS::TestIfIDP(){
	return IDP;
}

void STATUS::SetEnable(bool S){
	Enable = S;
}

void STATUS::SetIDP(bool S){
	IDP = S;
}
