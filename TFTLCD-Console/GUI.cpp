#include "GUI.h"
#include "LCD.h"
#include "ASCII.h"
#include "ARC.h"
GUI Window;


void GUI::Init(){
	
}

void GUI::DashRound(DashRoundParameter Data){
	u32 AngleBar = Data.Angle*Data.PercentBar/100; //进度条填充角度
	u32 StartAngleBarRest = Data.StartAngle+AngleBar; //进度条非填充起始角度
	u32 AngleBarRest = Data.Angle-AngleBar; //进度条非填充角度
	for(u32 radius=Data.RadiusSmallRim+2;radius<Data.RadiusLargeRim-1;radius+=2){
		DrawArc(Data.Posi,radius,Data.StartAngle,AngleBar,2,Data.ColorBar); //进度条填充部分
		DrawArc(Data.Posi,radius,StartAngleBarRest,AngleBarRest,2,BACKGROUND); //进度条非填充部分
		DrawArc(Data.Posi,radius,Data.StartAngle+Data.Angle-4,2,2,Data.ColorRim);
		DrawArc(Data.Posi,radius,Data.StartAngle,2,2,Data.ColorRim);
	}
	DrawArc(Data.Posi,Data.RadiusLargeRim,Data.StartAngle,Data.Angle,2,Data.ColorRim); //外环
	DrawArc(Data.Posi,Data.RadiusSmallRim,Data.StartAngle,Data.Angle,2,Data.ColorRim); //内环
	
	u32 AnglePointer = Data.Angle*Data.PercentPointer/100;
	u32 StartAnglePointerRest = Data.StartAngle+AnglePointer;
	u32 AnglePointerRest = Data.Angle-AnglePointer;
	DrawArc(Data.Posi,Data.RadiusLargeRim+5,Data.StartAngle,AnglePointer,2,Data.ColorPointer);
	DrawArc(Data.Posi,Data.RadiusLargeRim+5,StartAnglePointerRest,AnglePointerRest,3,BACKGROUND);
	
	u16 PosiTextUp[2] = {
		Data.Posi[0]-Data.RadiusSmallRim+Data.TextZoom*5,
		Data.Posi[1]-Data.TextZoom*9};
	u16 PosiTextDown[2] = {
		Data.Posi[0]-Data.RadiusSmallRim+Data.TextZoom*5,
		Data.Posi[1]};
	DrawString(PosiTextUp,Data.TextUp,Data.TextZoom,Data.ColorText);
	DrawString(PosiTextDown,Data.TextDown,Data.TextZoom,Data.ColorText);
}

void GUI::DashSquare(DashSquareParameter Data){
	DrawRectangle(Data.Posi,Data.Width,Data.ColorRim);
	u16 PosiLine[4] = {Data.Posi[0] + (Data.Posi[2]-Data.Posi[0])*2/5,Data.Posi[1],
										 Data.Posi[0] + (Data.Posi[2]-Data.Posi[0])*2/5,Data.Posi[3]};
	DrawLine(PosiLine,Data.Width,Data.ColorRim);
	u32 NumberRows = Data.Posi[3] - Data.Posi[1] - Data.Width;
	u16 PosiBar[4] = {PosiLine[0] + Data.Width/2 + 1,
									  PosiLine[1] + Data.Width/2 + 1,
										PosiLine[0] + Data.Width/2 + (Data.Posi[2]-PosiLine[0]-Data.Width)*Data.PercentBar/100 + 1,
										PosiLine[1] + Data.Width/2 + 1};
	u16 PosiBarRest[4] = {PosiBar[2] + 1,PosiBar[3],
												Data.Posi[2] - Data.Width/2,PosiBar[3]};
	for(u32 rows=0;rows<NumberRows;rows++){
		DrawLine(PosiBar,1,Data.ColorBar);
		if(Data.PercentBar < 100)
			DrawLine(PosiBarRest,1,BACKGROUND);
		PosiBar[1]++;PosiBar[3]++;
		PosiBarRest[1]++;PosiBarRest[3]++;
	}
	u16 PosiString1[2] = {Data.Posi[0]+Data.Width,Data.Posi[1]+Data.Width};
	u16 PosiString2[2] = {Data.Posi[0]+Data.Width,Data.Posi[1]+Data.Width+9*Data.TextZoom};
	DrawString(PosiString1,Data.Text1,Data.TextZoom,Data.ColorText);
	DrawString(PosiString2,Data.Text2,Data.TextZoom,Data.ColorText);
}

void GUI::BinaryPicture(BinaryPictureParameter Data){
	u16 PosiX = Data.Posi[0];
	u16 PosiY = Data.Posi[1];
	u16 PixelCounter = 0;
	for(u32 row=0;row<Data.NumbleRow;row++){
		for(u32 column=0;column<Data.NumbleColumn;column++){
			if((Data.Picture[PixelCounter/8]<<PixelCounter%8) & 0x80)
				LCDInterface.DrawPoint(PosiX,PosiY,Data.Zoom,Data.Color);
			else
				LCDInterface.DrawPoint(PosiX,PosiY,Data.Zoom,BACKGROUND);
			PosiX += Data.Zoom;
			PixelCounter++;
		}
		PosiX = Data.Posi[0];
		PosiY += Data.Zoom;
	}
}



void GUI::DrawLine(u16 Posi[4],u8 Width,u16 Color){
	s32 DeltaX = Posi[2]-Posi[0];
	s32 DeltaY = Posi[3]-Posi[1];
	s32 SubPosi[4] = {Posi[0],Posi[1],Posi[2],Posi[3]};
	
	if(!DeltaX){ //竖线
		if(SubPosi[1] > SubPosi[3]){
			u16 temp = SubPosi[1];
			SubPosi[1] = SubPosi[3];
			SubPosi[3] = temp;
		}
		for(u32 d=0;d<Width;d++)
			LCDInterface.SetPixelUpright(SubPosi[0]-Width/2+d,SubPosi[1],SubPosi[3],Color);
		return;
	}
	if(!DeltaY){ //横线
		if(SubPosi[0] > SubPosi[2]){
			u16 temp = SubPosi[0];
			SubPosi[0] = SubPosi[2];
			SubPosi[2] = temp;
		}
		for(u32 d=0;d<Width;d++)
			LCDInterface.SetPixelLine(SubPosi[0],SubPosi[1]-Width/2+d,SubPosi[2],Color);
		return;
	}
	
	s32 Delta = Abs(DeltaX);
	if(Abs(DeltaY)>Abs(DeltaX))
		Delta = Abs(DeltaY);
	for(s32 d=0;d<Delta;d++){
		s32 PosiX = SubPosi[0] + DeltaX*d/Delta;
		s32 PosiY = SubPosi[1] + DeltaY*d/Delta;
		LCDInterface.DrawPoint(PosiX,PosiY,Width,Color);
	}
}
void GUI::DrawRectangle(u16 Posi[4],u8 Width,u16 Color){
/*
	  X1          X2
  Y1+-----------+
  	|           |
  	|           |
  Y2+-----------+
	
		X1 - Posi[0]
		Y1 - Posi[1]
		X2 - Posi[2]
		Y2 - Posi[3]
	
*/
	u16 Line[10] = {
		Posi[0],Posi[1],
		Posi[2],Posi[1],
		Posi[2],Posi[3],
		Posi[0],Posi[3],
		Posi[0],Posi[1]};
	
	DrawLine(Line,Width,Color);
	DrawLine(Line+2,Width,Color);
	DrawLine(Line+4,Width,Color);
	DrawLine(Line+6,Width,Color);
}

void GUI::DrawFrame(u16 Posi[4],u16 ColorA,u16 ColorB){
/*
	  X1     1/5    4/5   X2
  Y1+  +====-------     + P4
	    / P1  P2    P3    ||
	   /                 C||D
P10++                   ||
   ||                   ++P5
	A||B                 /
   ||                 /
P9 Y2+    --------====+  +
  	      P8      P7   P6
		X1 - Posi[0]
		Y1 - Posi[1]
		X2 - Posi[2]
		Y2 - Posi[3]
	
*/
	u16 X1 = Posi[0];
	u16 Y1 = Posi[1];
	u16 X2 = Posi[2];
	u16 Y2 = Posi[3];
	u16 LineA[10] = {X1,Y2-5, X1,Y1+10, X1+10,Y1, X1+(X2-X1)/5,Y1, X1+(X2-X1)*4/5,Y1};
	u16 LineB[8] = {X1+5,Y2, X1+5,Y1+13,X1+13,Y1+5,X1+(X2-X1)/5,Y1+5};
	u16 LineC[8] = {X2-5,Y1, X2-5,Y2-13, X2-13,Y2-5, X2-(X2-X1)/5,Y2-5};
	u16 LineD[10] = {X2,Y1+5, X2,Y2-10, X2-10,Y2, X2-(X2-X1)/5,Y2, X2-(X2-X1)*4/5,Y2};
		
	for(u32 counter=0;counter<3;counter++){	 
		DrawLine(LineA+counter*2,2,ColorB);
		DrawLine(LineB+counter*2,4,ColorB);
		DrawLine(LineC+counter*2,4,ColorB);
		DrawLine(LineD+counter*2,2,ColorB);
	}
	DrawLine(LineA+6,2,ColorA);
	DrawLine(LineD+6,2,ColorA);
}

void GUI::DrawCircle(u16 Posi[2],u8 Radius,u8 Width,u16 Color){
	s32 x = 0;
	s32 y = Radius;
	for(x=0;x<=Radius;x++){
		s32 temp = y;
		while(x*x+y*y > Radius*Radius)
			LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-(y--)-1,Width,Color);
		if(temp == y)
			LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-y-1,Width,Color);
	}
	y = 0;
	for(x=Radius;x>=0;x--){
		s32 temp = y;
		while(x*x+y*y < Radius*Radius)
			LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-(y--)-1,Width,Color);
		if(temp == y)
			LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-y-1,Width,Color);			
	}
	y = -Radius;
	for(x=0;x>=-Radius;x--){
		s32 temp = y;
		while(x*x+y*y > Radius*Radius)
			LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-(y++),Width,Color);
		if(temp == y)
			LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-y,Width,Color);			
	}	
	y = 0;
	for(x=-Radius;x<=0;x++){
		s32 temp = y;
		while(x*x+y*y < Radius*Radius)
			LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-(y++),Width,Color);
		if(temp == y)
			LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-y,Width,Color);			
	}
}
void GUI::DrawArc(u16 Posi[2],u8 Radius,u16 StartAngle,u16 Angle,u8 Width,u16 Color){
	if(Angle>=360){
		DrawCircle(Posi,Radius,Width,Color);
		return;
	}
	if(StartAngle>=360) StartAngle %=360;
	u32 Skip = StartAngle*ARC[Radius]/90;
	u32 Step = Angle*ARC[Radius]/90;
	s32 x = 0;
	s32 y = Radius;
	for(x=0;x<=Radius;x++){
		s32 temp = y;
		while(x*x+y*y > Radius*Radius){
			if(Skip){
				Skip--;y--;
			}
			else if(Step){
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-(y--)-1,Width,Color);
				Step--;
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-y-1,Width,Color);
				Step--;
			}
			else return;
		}
	}
	y = 0;
	for(x=Radius;x>=0;x--){
		s32 temp = y;
		while(x*x+y*y < Radius*Radius){
			if(Skip){
				Skip--;
				y--;
			}
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-(y--)-1,Width,Color);
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-y-1,Width,Color);
			}
			else return;
		}			
	}
	y = -Radius;
	for(x=0;x>=-Radius;x--){
		s32 temp = y;
		while(x*x+y*y > Radius*Radius){
			if(Skip){
				Skip--;
				y++;
			}
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-(y++),Width,Color);
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-y,Width,Color);	
			}
			else return;
		}			
	}	
	y = 0;
	for(x=-Radius;x<=0;x++){
		s32 temp = y;
		while(x*x+y*y < Radius*Radius){
			if(Skip){
				Skip--;
				y++;
			}
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-(y++),Width,Color);
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-y,Width,Color);	
			}
			else return;
		}			
	}
	y = Radius;
	for(x=0;x<=Radius;x++){
		s32 temp = y;
		while(x*x+y*y > Radius*Radius){
			if(Skip){
				Skip--;
				y--;
			}
			else if(Step){
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-(y--)-1,Width,Color);
				Step--;
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-y-1,Width,Color);
				Step--;
			}
			else return;
		}
	}
	y = 0;
	for(x=Radius;x>=0;x--){
		s32 temp = y;
		while(x*x+y*y < Radius*Radius){
			if(Skip){
				Skip--;
				y--;
			}
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-(y--)-1,Width,Color);
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-y-1,Width,Color);
			}
			else return;
		}			
	}
	y = -Radius;
	for(x=0;x>=-Radius;x--){
		s32 temp = y;
		while(x*x+y*y > Radius*Radius){
			if(Skip){
				Skip--;
				y++;
			}
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-(y++),Width,Color);
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x+1,Posi[1]-y,Width,Color);	
			}
			else return;
		}			
	}	
	y = 0;
	for(x=-Radius;x<=0;x++){
		s32 temp = y;
		while(x*x+y*y < Radius*Radius){
			if(Skip){
				Skip--;
				y++;
			}
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-(y++),Width,Color);
			}
			else return;
		}
		if(temp == y){
			if(Skip) Skip--;
			else if(Step){
				Step--;
				LCDInterface.DrawPoint(Posi[0]+x,Posi[1]-y,Width,Color);	
			}
			else return;
		}			
	}
}
void GUI::DrawChar(u16 Posi[2],s8 Char,u8 Zoom,u16 Color){
	for(u32 x=0;x<5;x++){
		for(u32 z=0;z<Zoom;z++){
			LCDInterface.DrawNBit(Posi[0]+x*Zoom+z,Posi[1],ASCII[Char][x],8,Zoom,Color);
		}
	}
}
void GUI::DrawString(u16 Posi[2],char String[],u8 Zoom, u16 Color){
	u16 SubPosi[2] = {Posi[0],Posi[1]};
	for(u32 c=0;String[c];c++){
		DrawChar(SubPosi,String[c],Zoom,Color);
		SubPosi[0] += Zoom*6;
	}
}

u32 GUI::Abs(s32 Data){
	if(Data<0) return -Data;
	return Data;
}
