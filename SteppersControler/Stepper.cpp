#include "Stepper.h"

STEPPER Stepper;

inline double GetMax(double Data[],u32 Length){ //最大值
	double Temp = Data[0];
	for(u32 i=1;i<Length;i++)
		if(Data[i] > Temp) Temp = Data[i];
	return Temp;
}
inline double GetMin(double Data[],u32 Length){ //最小值
	double Temp = Data[0];
	for(u32 i=1;i<Length;i++)
		if(Data[i] < Temp) Temp = Data[i];
	return Temp;
}

extern"C"void TIM2_IRQHandler(){
  if(TIM2->SR&0x01){
		if(Stepper.FAStatus == 'I'){ //空闲状态
			if(Stepper.PtrLauncher->Full){ //缓存非空
				Stepper.SetAllDirection(Stepper.PtrLauncher->Direction); //设置方向
				TIM2->ARR = (u16)Stepper.PtrLauncher->CycleIn; //设置节拍周期
				Stepper.FAStatus = 'W'; //进入运行状态           
			}
			else{ //缓存空
			TIM2->ARR = 5625;
				Stepper.FAStatus = 'I'; //保持空闲状态
			}
		}
		
		else if(Stepper.FAStatus == 'W'){ //运行状态
			if(Stepper.PtrLauncher->Full){ //缓存非空
				Stepper.PtrLauncher->ClockCounter++; //当前时刻+1
				
			//调节速度
				if(Stepper.PtrLauncher->Accelerate){ //加速
					Stepper.PtrLauncher->Accelerate--;
					
					float VelocityPresent = sqrtf(
						Stepper.PtrLauncher->VelocityIn*Stepper.PtrLauncher->VelocityIn +
						2.0f*Stepper.PtrLauncher->Acceleration*Stepper.PtrLauncher->ClockCounter*SYNTHETIC_LENGTH);
					
					TIM2->ARR = (u16)(56250/VelocityPresent); //设置节拍周期
				}
				else if(Stepper.PtrLauncher->ConstSpeed){ //匀速
					Stepper.PtrLauncher->ConstSpeed--;
				}
				else if(Stepper.PtrLauncher->Decelerate){ //减速
					Stepper.PtrLauncher->Decelerate--;
					float VelocityPresent = sqrtf(
						Stepper.PtrLauncher->VelocityOut*Stepper.PtrLauncher->VelocityOut +
						2.0f*Stepper.PtrLauncher->Deceleration*Stepper.PtrLauncher->Decelerate*SYNTHETIC_LENGTH);
					
					TIM2->ARR = (u16)(56250/VelocityPresent); //设置节拍周期
				}
				
			//产生脉冲
				u8 PluseSelection = 0x00; //记录
				for(u32 i=0;i<5;i++){
					if(Stepper.PtrLauncher->NextPluseClock[i]<Stepper.PtrLauncher->ClockCounter){ //到达脉冲时刻
						if(Stepper.PtrLauncher->Step[i]){ //仍有步数剩余
							Stepper.PtrLauncher->NextPluseClock[i] += Stepper.PtrLauncher->TPluse[i]; //计算下一次脉冲时刻	
							PluseSelection |= 0x01<<i; //记录脉冲
							Stepper.PtrLauncher->Step[i]--; //剩余步数减一
						}
					}
				}
				Stepper.GeneratePluse(PluseSelection); //产生脉冲	
				TIM3->ARR = PLUSE_WIDTH; 
				
			//检测剩余步数
				for(u32 i=0;i<6;i++){
					if(i==5){ //没有剩余
						Stepper.PtrLauncher->Full = 0; //清除缓存满标志
						Stepper.L1RestAmount++; //剩余缓存容量加一 
						Stepper.FAStatus = 'S'; //进入切换状态
						TIM2->ARR = 2; //加速切换
						break;
					}
					if(Stepper.PtrLauncher->Step[i]) break; //缓存非空
				}
			}
			
			else{ //缓存空
				Stepper.FAStatus = 'S'; //进入切换状态
				TIM2->ARR = 2; //加速切换
			}
		}
		
		else if(Stepper.FAStatus == 'S'){ //切换缓存状态
			Stepper.PtrLauncher = Stepper.PtrLauncher->Next; //切换缓存
			
			if(Stepper.PtrLauncher->Full){ //缓存非空
				Stepper.SetAllDirection(Stepper.PtrLauncher->Direction); //设置方向
				TIM2->ARR = (u16)Stepper.PtrLauncher->CycleIn; //设置节拍周期
				Stepper.FAStatus = 'W'; // 进入运行状态
			}
			else{ //缓存空
				Stepper.FAStatus = 'I'; //进入空闲状态
				TIM2->ARR = 5625;
			}
		}
		
		else{ //状态错误
			Stepper.FAStatus = 'I'; //进入空闲状态
			TIM2->ARR = 5625;
		}
  	TIM2->SR&=~0x01;
  }
}

extern"C"void TIM3_IRQHandler(void){
//定时器3更新中断
//产生步进电机控制脉冲下降沿
//重新失能定时器3等待下一个脉冲	
	if(TIM3->SR&0x01){
		
		Stepper.GeneratePluse(0x00); //产生脉冲下降沿
		TIM3->ARR = 0; //失能定时器

		TIM3->SR&=~0x01;
	}
}

void STEPPER::Init(){
	//循环指针初始化
	L1_Cache[L1_SIZE-1].Next = L1_Cache;
	L1_Cache[0].Pror = (L1_Cache+L1_SIZE-1);
	for(u32 i=0;i<L1_SIZE-1;i++)
		(L1_Cache+i)->Next = L1_Cache+i+1;
	for(u32 i=1;i<L1_SIZE;i++)
		(L1_Cache+i)->Pror = L1_Cache+i-1;
	
	//门初始化
	PtrLauncher = L1_Cache;
	PtrLoader = L1_Cache;
	
	//缓存内容初始化
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
	
	//缓存容量初始化
	L1RestAmount = L1_SIZE;
	
	//自动机状态初始化
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
	
	/*定时器2初始化
		定时器2用于生成自动机节拍
		这个节拍是可变的，取决于当前的步进电机速度
		当步进电机速度较快时，节拍也较快
		步进电机速度较慢时，节拍也随之变慢
		节拍周期有最大值，这对应着步进电机的启动速度
		
		每个节拍到来时，都会在定时器2的更新中断中更新自动机状态
		当然也会根据情况生成步进电机控制脉冲的上升沿
		或(和)改变步进电机的速度，也就是改变节拍周期
	*/
	RCC->APB1ENR |= 0x01;
  TIM2->ARR = 5625;//预装载
  TIM2->PSC = 7;//8分频
  TIM2->CR1 |= 0x01<<4;
  TIM2->DIER |= 0x01<<0;
  NVIC->IP[28] = 0x10;//优先级1
  NVIC->ISER[0] |= 0x01<<28;
  TIM2->CR1 |= 0x01<<0;//使能
	
	/*定时器3初始化
	定时器3用以辅助定时器2生成步进电机控制脉冲
	当定时器2生成步进电机控制脉冲的上升沿的同时，会启动定时器3
	经过一定时间(一般是10个微秒)后，定时器3会产生更新事件
	在定时器3的更新中断中生成步进电机控制脉冲的下降沿
	*/
  RCC->APB1ENR |= 0x01<<1;
  TIM3->ARR = 0;//预装载
  TIM3->PSC = 71;//72分频
  TIM3->CR1 |= 0x01<<4;
  TIM3->DIER |= 0x01<<0;
  NVIC->IP[29] = 0x10;//优先级1
  NVIC->ISER[0] |= 0x01<<29;
  TIM3->CR1 |= 0x01<<0;//使能
	
}

bool STEPPER::AutoHoming(){
	if(!Status.TestIfEnable()) //未使能
		EnableStepper();
	
	TestMove(2,0,AXIS_Z_MAX_POSITION*1.2);
	TestMove(1,1,AXIS_Y_MAX_POSITION*1.2);
	TestMove(0,1,AXIS_X_MAX_POSITION*1.2);
	

	PROB_ENABLE(1);
	Delay_Ms(2000);
	
	TestMove(0,0,AXIS_X_RETURN_LENGTH); //X轴稍微移动一些，释放开关
	TestMove(1,0,AXIS_Y_RETURN_LENGTH); //Y轴稍微移动一些，释放开关
	
	TestMove(0,0,AXIS_X_MAX_POSITION/2.0 - 30.0); //X轴移动到中间
	TestMove(1,0,AXIS_Y_MAX_POSITION/2.0 - 30.0); //Y轴移动到中间
	TestMove(2,1,AXIS_Z_RETURN_LENGTH); //Z轴反向移动，触发探针
	
	TestMove(2,0,10.0); //Z轴回退10mm
	
	TestMove(0,1,AXIS_X_MAX_POSITION/2.0 - 30.0); //X轴移动到起点
	TestMove(1,1,AXIS_Y_MAX_POSITION/2.0 - 30.0); //Y轴移动到起点
	
	Status.SetPosition(0.0,0); //X轴设置为0mm
	Status.SetPosition(0.0,1); //Y轴设置为0mm
	Status.SetPosition(0.0,2); //Z轴设置为0mm
	Status.SetPosition(0.0,3); //A轴设置为0mm
	Status.SetPosition(0.0,4); //B轴设置为0mm
	
// y A
//	 |
// PD|- - - -PC 
//	 |       |
//	 |       
// PA|       PB
//   +----------->
//	            x
							
	//现在XY轴都位于起点
	double TestPoint[5] = {0.0, 0.0, 0.0, 0.0, 10.0}; //5个测量点数据
	//可以开始测试高度
	//测试点A (X,Y) = (0,0)
	TestMove(2,1,15.0); //Z轴反向移动15mm，触发探针
	TestPoint[0] = fabsl(Status.GetPosition(2)); //获取A点位置信息
	TestMove(2,0,TestPoint[0]); //Z轴回到原来的高度
	
	//现在XY轴位于A
	//测试点B (X,Y) = (Max,0)
	TestMove(0,0,AXIS_X_MAX_POSITION - 30.0); //X轴移动到终点
	TestMove(2,1,15.0); //Z轴反向移动15mm，触发探针
	TestPoint[1] = fabsl(Status.GetPosition(2)); //获取B点位置信息
	TestMove(2,0,TestPoint[1]); //Z轴回到原来的高度
//	TestMove(0,1,AXIS_X_MAX_POSITION - 30.0); //X轴移动到起点
	
	//现在XY轴位于B
	//测试点C (X,Y) = (0,Max)
	TestMove(1,0,AXIS_Y_MAX_POSITION - 30.0); //Y轴移动到终点
	TestMove(2,1,15.0); //Z轴反向移动15mm，触发探针
	TestPoint[2] = fabsl(Status.GetPosition(2)); //获取C点位置信息
	TestMove(2,0,TestPoint[2]); //Z轴回到原来的高度
//	TestMove(1,1,AXIS_Y_MAX_POSITION - 30.0); //Y轴移动到起点
	
	//现在XY轴位于C
	//测试点D (X,Y) = (Max,Max)
	TestMove(0,1,AXIS_X_MAX_POSITION - 30.0); //X轴移动到起
//	TestMove(1,0,AXIS_Y_MAX_POSITION - 30.0); //Y轴移动到终点
	TestMove(2,1,15.0); //Z轴反向移动15mm，触发探针
	TestPoint[3] = fabsl(Status.GetPosition(2)); //获取D点位置信息
	TestMove(2,0,TestPoint[3]); //Z轴回到原来的高度
//	TestMove(0,1,AXIS_X_MAX_POSITION - 30.0); //X轴移动到起点
	TestMove(1,1,AXIS_Y_MAX_POSITION - 30.0); //Y轴移动到起点
	
	//现在XY轴都位于起点
	
	double Min = GetMin(TestPoint,5);
	for(u32 i=0;i<5;i++)
		TestPoint[i] -= Min;
	
	double Max = GetMax(TestPoint,5);
	if(Max < MAX_DIFFERENCE) //差别不大，认为已经调平
		Status.Leveling = 1;
	else //没有调平
		Status.Leveling = 0;
	
	for(u32 i=0;i<5;i++) //记录调平状况
		Status.TestPoint[i] = TestPoint[i];
	
	Delay_Ms(1000); //延迟一会
	PROB_ENABLE(0); //收回探针
	Delay_Ms(2000); //等待探针收回
	
	TestMove(2,1,Min+Max+HIGH_PROB); //Z轴回到最大高度
	
	double Posi[5] = {0.0 , 0.0 , 0.0 , 0.0 , 0.0};
	if(Status.InitPosition(Posi)) //设置坐标及已归位标志
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
	if(!Status.TestIfEnable()) //未使能
		EnableStepper();
	
	u32 HALFPLUSEWIDTH = 3125/(u32)VELOCITY_HOMING; //[us] 由归位速度确定脉冲半宽度
	if(Length < 0)
		Length = -Length;

	if(Selection == 0){ //X轴
		u32 Steps = Length/X_STEP_LENGTH; //计算步数
		STEPPER_X_DIRECTION(Direction); //设置方向
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //移动
			if(Status.TestMinEndPointX()&&Direction) break;
			if(Status.TestMaxEndPointX()&&!Direction) break;
			
			STEPPER_X_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_X_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToX(Direction);
		}
	}
	else if(Selection == 1){ //Y轴
		u32 Steps = Length/Y_STEP_LENGTH; //计算步数
		STEPPER_Y_DIRECTION(Direction); //设置方向
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //移动
			if(Status.TestMinEndPointY()&&Direction) break;
			if(Status.TestMaxEndPointY()&&!Direction) break;
			
			STEPPER_Y_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_Y_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToY(Direction);
		}
	}
	else if(Selection == 2){ //Z轴
		u32 Steps = Length/Z_STEP_LENGTH; //计算步数
		STEPPER_Z_DIRECTION(Direction); //设置方向
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //移动
			if(Status.TestMinEndPointZ()&&Direction) break;
			if(Status.TestMaxEndPointZ()&&!Direction) break;
			
			STEPPER_Z_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_Z_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToZ(Direction);
		}
	}
	else if(Selection == 3){ //A轴
		if(Length > 200)
			Length = 200;
		u32 Steps = Length/A_STEP_LENGTH; //计算步数
		STEPPER_A_DIRECTION(Direction); //设置方向
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //移动
			STEPPER_A_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_A_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToA(Direction);
		}
	}
	else if(Selection == 4){ //B轴
		if(Length > 200)
			Length = 200;
		u32 Steps = Length/B_STEP_LENGTH; //计算步数
		STEPPER_B_DIRECTION(Direction); //设置方向
		Delay_Ms(1);
		for(u32 i=0;i<Steps;i++){ //移动
			STEPPER_B_PLUSE(1);
			Delay_Us(HALFPLUSEWIDTH);
			STEPPER_B_PLUSE(0);
			Delay_Us(HALFPLUSEWIDTH);
			Status.AddOneStepToB(Direction);
		}
	}
}

void STEPPER::EnableStepper(){
	/*使能所有步进电机
		在使能步进电机数百毫秒后会解除Z轴失电制动
		也就是说这个过程会引入毫秒级的延迟
		一般仅在开机时调用此过程
	*/
	STEPPER_ENABLE(1); //使能步进电机
	Delay_Ms(10);
//	LOCK_ENABLE(0); //解除制动
//	Delay_Ms(200);
	Status.SetEnable(1); //设置使能标志
}

bool STEPPER::DisableStepper(){
	/*失能所有步进电机
		启动Z轴失电制动器后数百毫秒再失能步进电机
		也就是说此过程会引入毫秒级延迟
		一般仅在关机前调用此过程
	*/
	if(Planner.L2RestAmount != L2_SIZE) //L2缓存非空不可关闭
		return 0;
	if(Stepper.L1RestAmount != L1_SIZE) //L1缓存非空不可关闭
		return 0;
	if(Stepper.FAStatus != 'I') //运行中不可关闭
		return 0;
	
//	LOCK_ENABLE(1);
//	Delay_Ms(200);
	STEPPER_ENABLE(0);
	Delay_Ms(10);
	Status.SetEnable(0); //清除使能标志
	return 1;
	
}

void STEPPER::SetAllDirection(bool Direction[5]){
	/*设置方向
		参数Direction
	  MSB 7 6 5 4 3 2 1 0 LSB
	    \ | | | | | | | | /
		  	N N N B A Z Y X
	  N - 保留/未使用
	  B - B轴
		A - A轴
	  Z - Z轴
	  Y - Y轴
	  X - X轴
		在对应的轴的Selection参数有效(==True)时
	  某一Bit - 0/1<=>(对应的轴)设置为方向0/方向1，实际方向由机器设置决定	
	*/
	STEPPER_X_DIRECTION(Direction[0]);
	
	STEPPER_Y_DIRECTION(Direction[1]);
	
	STEPPER_Z_DIRECTION(Direction[2]);
	
	STEPPER_A_DIRECTION(Direction[3]);
	
	STEPPER_B_DIRECTION(Direction[4]);
}

void STEPPER::GeneratePluse(u8 Selection){
	/*根据需求生成控制脉冲
	
		参数Selection
	  MSB 7 6 5 4 3 2 1 0 LSB
	    \ | | | | | | | | /
		  	N N N B A Z Y X
	  N - 保留/未使用
	  B - B轴
		A - A轴
	  Z - Z轴
	  Y - Y轴
	  X - X轴
	  任一Bit - 0/1<=>(对应的轴)不产生脉冲/产生脉冲
	
		从参数Selection中解析出需要产生脉冲的轴
		产生脉冲上升沿并启动定时器2
		(定时器3的更新中断中产生脉冲下降沿)
	*/
	if(Selection & 0x01){ //X轴
		if(!PtrLauncher->Direction[0] && !Status.TestMaxEndPointX()){ //正向且未到达限位
			if(VirtualStep[0] >= 0){ //没有虚位移
				Status.AddOneStepToX(Stepper.PtrLauncher->Direction[0]);
				STEPPER_X_PLUSE(1);
			}
			else //消除虚位移
				if(VirtualStep[0] < 0) VirtualStep[0]++;
		}
		else if(!PtrLauncher->Direction[0] && Status.TestMaxEndPointX()){ //正向且到达限位
			VirtualStep[0]++; //进行虚位移
		}
		else if(PtrLauncher->Direction[0] && !Status.TestMinEndPointX()){ //反向且未到达限位
			if(VirtualStep[0] <= 0){ //没有虚位移
				Status.AddOneStepToX(Stepper.PtrLauncher->Direction[0]);
				STEPPER_X_PLUSE(1);
			}
			else //消除虚位移
				if(VirtualStep[0] > 0) VirtualStep[0]--;
		}
		else if(PtrLauncher->Direction[0] && Status.TestMinEndPointX()){ //反向且到达限位
			VirtualStep[0]--; //进行虚位移
		}
	}
	else{
			STEPPER_X_PLUSE(0);
	}
	
	if(Selection & 0x02){ //Y轴
		if(!PtrLauncher->Direction[1] && !Status.TestMaxEndPointY()){ //正向且未到达限位
			if(VirtualStep[1] >= 0){ //没有虚位移
				Status.AddOneStepToY(Stepper.PtrLauncher->Direction[1]);
				STEPPER_Y_PLUSE(1);
			}
			else //消除虚位移
				if(VirtualStep[1] < 0) VirtualStep[1]++;
		}
		else if(!PtrLauncher->Direction[1] && Status.TestMaxEndPointY()){ //正向且到达限位
			VirtualStep[1]++; //进行虚位移
		}
		else if(PtrLauncher->Direction[1] && !Status.TestMinEndPointY()){ //反向且未到达限位
			if(VirtualStep[1] <= 0){ //没有虚位移
				Status.AddOneStepToY(Stepper.PtrLauncher->Direction[1]);
				STEPPER_Y_PLUSE(1);
			}
			else //消除虚位移
				if(VirtualStep[1] > 0) VirtualStep[1]--;
		}
		else if(PtrLauncher->Direction[1] && Status.TestMinEndPointY()){ //反向且到达限位
			VirtualStep[1]--; //进行虚位移
		}
	}
	else{
			STEPPER_Y_PLUSE(0);
	}
	
	if(Selection & 0x04){ //Z轴
		if(!PtrLauncher->Direction[2] && !Status.TestMaxEndPointZ()){ //正向且未到达限位
			if(VirtualStep[2] >= 0){ //没有虚位移
				Status.AddOneStepToZ(Stepper.PtrLauncher->Direction[2]);
				STEPPER_Z_PLUSE(1);
			}
			else //消除虚位移
				if(VirtualStep[2] < 0) VirtualStep[2]++;
		}
		else if(!PtrLauncher->Direction[2] && Status.TestMaxEndPointZ()){ //正向且到达限位
			VirtualStep[2]++; //进行虚位移
		}
		else if(PtrLauncher->Direction[2] && !Status.TestMinEndPointZ()){ //反向且未到达限位
			if(VirtualStep[2] <= 0){ //没有虚位移
				Status.AddOneStepToZ(Stepper.PtrLauncher->Direction[2]);
				STEPPER_Z_PLUSE(1);
			}
			else //消除虚位移
				if(VirtualStep[2] > 0) VirtualStep[2]--;
		}
		else if(PtrLauncher->Direction[2] && Status.TestMinEndPointZ()){ //反向且到达限位
			VirtualStep[2]--; //进行虚位移
		}
	}
	else{
			STEPPER_Z_PLUSE(0);
	}

	
/*注释的内容是没有限位测量的移动，如果需要此功能可以进行替换
	if(Selection & 0x01){ //X轴
		Status.AddOneStepToX(Stepper.PtrLauncher->Direction[0]);
		STEPPER_X_PLUSE(1);
	}
	else{
		STEPPER_X_PLUSE(0);
	}
	
	if(Selection & 0x02){ //Y轴
		Status.AddOneStepToY(Stepper.PtrLauncher->Direction[1]);
		STEPPER_Y_PLUSE(1);
	}
	else{
		STEPPER_Y_PLUSE(0);
	}
	
	if(Selection & 0x04){ //Z轴
		Status.AddOneStepToZ(Stepper.PtrLauncher->Direction[2]);
		STEPPER_Z_PLUSE(1);
	}
	else{
		STEPPER_Z_PLUSE(0);
	}*/
	
	if(Selection & 0x08){ //A轴
		Status.AddOneStepToA(Stepper.PtrLauncher->Direction[3]);
		STEPPER_A_PLUSE(1);
	}
	else{
		STEPPER_A_PLUSE(0);
	}
	
	if(Selection & 0x10){ //B轴
		Status.AddOneStepToB(Stepper.PtrLauncher->Direction[4]);
		STEPPER_B_PLUSE(1);
	}
	else{
		STEPPER_B_PLUSE(0);
	}
	
}



