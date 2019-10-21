#include "Status.h"

class STATUS Status;

extern"C"void TIM3_IRQHandler(void){
	if(TIM3->SR&0x01){
		if(Status.CursorStatus == 'I'){ //插入型光标
			Status.CursorCounter++; //改变状态
			if(Status.CursorCounter>1) //状态溢出
				Status.CursorCounter = 0;	
			if(Status.CursorCounter) //显示状态
				Status.ShowCursor(); //显示光标
			else  //隐藏状态
				Status.HideCursor(); //隐藏光标
		}
		else{ //运转型光标
			Status.CursorCounter++; //改变状态
			if(Status.CursorCounter>3) //状态溢出
				Status.CursorCounter = 0;
			Status.ShowCursor(); //改变光标
		}
		TIM3->SR&=~0x01;
	}
}


void STATUS::Init(){
	static u16 RectanglePosi1[4] = {1,1,478,34};
	static u16 RectanglePosi2[4] = {1,38,478,318};
	Window.DrawFrame(RectanglePosi1,0xC618,0x8430);
	Window.DrawFrame(RectanglePosi2,0xC618,0x8430);
//状态栏内容初始化
	for(u32 i=0;i<38;i++)
		State[i] = ' ';
	State[37] = 0;
	
//显示缓冲区初始化
	for(u32 i=0;i<14;i++){
		for(u32 k=0;k<37;k++){
			DispBuffer[i][k] = 0;
		}
	}
//光标初始化
	Cursor[0] = 0;
	Cursor[1] = 0;
	
//光标状态初始化
	CursorStatus = 'I';//插入状态
	
//光标刷新
	RCC->APB1ENR|=0x01<<1;
  TIM3->ARR=0;//预装载0
  TIM3->PSC=7199;//分频
  TIM3->CR1|=0x01<<4;
  TIM3->DIER|=0x01<<0;
  NVIC->IP[29]=0x20;//优先级2
  NVIC->ISER[0]|=0x01<<29;
  TIM3->CR1|=0x01<<0;//使能
  
}

void STATUS::ShowStatus(){
	static u16 Posi[2] = {17,10};
	ShowLine(Posi,State,0xC618);
}

void STATUS::ShowData(){
	u16 Posi[2] = {17,47};
	u32 Index = 0;
	while(Index < 14){
		ShowLine(Posi,DispBuffer[Index],0xC618);
		Posi[1] += 18;
		Index++;	
	}
}

void STATUS::ShowChar(u16 Curs[2],char Data){
//	DisableCursor();
	DispBuffer[Curs[1]][Curs[0]] = Data;
	u16 Position[2];
	Position[0] = 17 + Curs[0]*12;
	Position[1] = 47 + Curs[1]*18;
	Window.DrawChar(Position,Data,2,0xC618);
//	EnableCursor();
}

void STATUS::ShowLine(u16 Posi[2],char Data[],u16 Color){
//	DisableCursor();	
	u16 Position[2] = {Posi[0],Posi[1]};
	u32 Index = 0;
	while(Index<37 && Data[Index]){
		Window.DrawChar(Position,Data[Index],2,Color);
		Position[0] += 12;
		Index++;
	}
	while(Index<37){
		Window.DrawChar(Position,' ',2,Color);
		Position[0] += 12;
		Index++;
	}
//	EnableCursor();	
}

void STATUS::AddChar(char Data){
	switch(Data){
		case '\b':
			if(Cursor[0] == 0){ //行首
				if(Cursor[1] == 0) return; //无可删除
				Cursor[0] = 36; //切换到前一行
				Cursor[1]--;
				for(u32 i=36;i>0;i--){
					if(DispBuffer[Cursor[1]][i] == 0)
					Cursor[0]--;
				}
				if(Cursor[0]){
					ShowChar(Cursor,0);
				}
				return;
			}
			else{
				Cursor[0]--;
				ShowChar(Cursor,0);
				return;
			}
		case '\n':case '\r':
			if(Cursor[1] > 12){
				for(u32 i=0;i<13;i++){
					for(u32 k=0;k<37;k++){
						DispBuffer[i][k] = DispBuffer[i+1][k];
					}
				}
				for(u32 i=0;i<37;i++){
					DispBuffer[13][i] = 0;
				}
				Cursor[0] = 0;
				Cursor[1] = 13;
				ShowData();
				return;
			}
			else{
				Cursor[0] = 0;
				Cursor[1]++;
				return;
			}
		default:
			if(Data>=32 && Data<=127){
				if(Cursor[0] == 36){ //行尾
					if(Cursor[1] == 13){ //最后一行
						DispBuffer[13][36] = Data;
						for(u32 i=0;i<13;i++){
							for(u32 k=0;k<37;k++){
								DispBuffer[i][k] = DispBuffer[i+1][k];
							}
						}
						for(u32 i=0;i<37;i++){
							DispBuffer[13][i] = 0;
						}
						Cursor[0] = 0;
						Cursor[1] = 13;
						ShowData();
					}
					else{
						ShowChar(Cursor,Data);
						Cursor[0] = 0;
						Cursor[1]++;
					}
				}
				else{				
					ShowChar(Cursor,Data);
					Cursor[0]++;
				}
			}
	}
}

void STATUS::Clear(){
	for(u32 i=0;i<14;i++){
		for(u32 k=0;k<37;k++){
			DispBuffer[i][k] = 0;
		}
	}
	Cursor[0] = 0;
	Cursor[1] = 0;
	ShowData();
}

void STATUS::SetState(char Data[37]){
	for(u32 i=0;i<37;i++){
		State[i] = Data[i];
	}
	State[37] = 0;
	ShowStatus();
}

void STATUS::EnableCursor(){
	CursorCounter = 0;
	TIM3->ARR = 4999; //0.5S
}

void STATUS::DisableCursor(){
	CursorCounter = 0;
	TIM3->ARR = 0;
	u16 Position[2];
	Position[0] = 17 + Cursor[0]*12;
	Position[1] = 47 + Cursor[1]*18;
	char Data = DispBuffer[Cursor[1]][Cursor[0]];
	Window.DrawChar(Position,Data,2,0xC618);
}

void STATUS::SetCursorStatus(char Stat){
	switch(Stat){
		case 'I':break;
		case 'B':break;
		default:return;
	}
	CursorStatus = Stat;
	CursorCounter = 0;
}

void STATUS::SetCursorPosition(u32 PosiX,u32 PosiY){
	HideCursor();
	if(PosiX>36) Cursor[0] = 36;
	else Cursor[0] = PosiX;
	if(PosiY>13) Cursor[1] = 13;
	else Cursor[1] = PosiY;
	ShowCursor();
}

void STATUS::ShowCursor(){
	u16 Position[2];
	Position[0] = 17 + Cursor[0]*12;
	Position[1] = 47 + Cursor[1]*18;
	if(CursorStatus == 'I')
		Window.DrawChar(Position,29,2,0xC618);
	else{
		switch(CursorCounter){
			case 0:Window.DrawChar(Position,'|',2,0xC618);break;
			case 1:Window.DrawChar(Position,'/',2,0xC618);break;
			case 2:Window.DrawChar(Position,'-',2,0xC618);break;
			case 3:Window.DrawChar(Position,'\\',2,0xC618);break;
			default:Window.DrawChar(Position,'|',2,0xC618);break;
		}
	}
}

void STATUS::HideCursor(){
	u16 Position[2];
	Position[0] = 17 + Cursor[0]*12;
	Position[1] = 47 + Cursor[1]*18;
	char Data = DispBuffer[Cursor[1]][Cursor[0]];
	Window.DrawChar(Position,Data,2,0xC618);
}
