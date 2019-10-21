#include "Status.h"

class STATUS Status;

char *STRING_OFF = "OFF ";
char *STRING_HOLDING = "HOLD";
char *STRING_COOLING = "COOL";
char *STRING_HEATING = "HEAT";

void STATUS::GenerateDashRound1_H(DashRoundParameter *Data){
	Data->PercentBar = 100*TempExtruderA/320;
	Data->PercentPointer = 100*TempExtruderASet/320;
	char *TempPtr = Data->TextUp;
	if(TempExtruderA >= 100){
		*(TempPtr++) = TempExtruderA%1000/100 + '0';
		*(TempPtr++) = TempExtruderA%100/10 + '0';
		*(TempPtr++) = TempExtruderA%10 + '0';
	}
	else if(TempExtruderA >= 10){
		*(TempPtr++) = TempExtruderA%100/10 + '0';
		*(TempPtr++) = TempExtruderA%10 + '0';
	}
	else
		*(TempPtr++) = TempExtruderA + '0';
	*(TempPtr++) = '/';
	if(TempExtruderASet >= 100){
		*(TempPtr++) = TempExtruderASet%1000/100 + '0';
		*(TempPtr++) = TempExtruderASet%100/10 + '0';
		*(TempPtr++) = TempExtruderASet%10 + '0';
	}
	else if(TempExtruderASet >= 10){
		*(TempPtr++) = TempExtruderASet%100/10 + '0';
		*(TempPtr++) = TempExtruderASet%10 + '0';
	}
	else
		*(TempPtr++) = TempExtruderASet + '0';
	*(TempPtr++) = 30;
	*(TempPtr++) = 'C';
	
	while(TempPtr-Data->TextUp < 9)
		*(TempPtr++) = ' ';
	*(TempPtr++) = 0;
}


void STATUS::GenerateDashRound2_H(DashRoundParameter *Data){
	Data->PercentBar = 100*TempExtruderB/320;
	Data->PercentPointer = 100*TempExtruderBSet/320;
	char *TempPtr = Data->TextUp;
	if(TempExtruderB >= 100){
		*(TempPtr++) = TempExtruderB%1000/100 + '0';
		*(TempPtr++) = TempExtruderB%100/10 + '0';
		*(TempPtr++) = TempExtruderB%10 + '0';
	}
	else if(TempExtruderB >= 10){
		*(TempPtr++) = TempExtruderB%100/10 + '0';
		*(TempPtr++) = TempExtruderB%10 + '0';
	}
	else
		*(TempPtr++) = TempExtruderB + '0';
	*(TempPtr++) = '/';
	if(TempExtruderBSet >= 100){
		*(TempPtr++) = TempExtruderBSet%1000/100 + '0';
		*(TempPtr++) = TempExtruderBSet%100/10 + '0';
		*(TempPtr++) = TempExtruderBSet%10 + '0';
	}
	else if(TempExtruderBSet >= 10){
		*(TempPtr++) = TempExtruderBSet%100/10 + '0';
		*(TempPtr++) = TempExtruderBSet%10 + '0';
	}
	else
		*(TempPtr++) = TempExtruderBSet + '0';
	*(TempPtr++) = 30;
	*(TempPtr++) = 'C';
	
	while(TempPtr-Data->TextUp < 9)
		*(TempPtr++) = ' ';
	*(TempPtr++) = 0;
}


void STATUS::GenetateDashRound3_H(DashRoundParameter *Data){
	Data->PercentBar = 100*TempBasePlane/150;
	Data->PercentPointer = 100*TempBasePlaneSet/150;
	char *TempPtr = Data->TextUp;
	if(TempBasePlane >= 100){
		*(TempPtr++) = TempBasePlane%1000/100 + '0';
		*(TempPtr++) = TempBasePlane%100/10 + '0';
		*(TempPtr++) = TempBasePlane%10 + '0';
	}
	else if(TempBasePlane >= 10){
		*(TempPtr++) = TempBasePlane%100/10 + '0';
		*(TempPtr++) = TempBasePlane%10 + '0';
	}
	else
		*(TempPtr++) = TempBasePlane + '0';
	*(TempPtr++) = '/';
	if(TempBasePlaneSet >= 100){
		*(TempPtr++) = TempBasePlaneSet%1000/100 + '0';
		*(TempPtr++) = TempBasePlaneSet%100/10 + '0';
		*(TempPtr++) = TempBasePlaneSet%10 + '0';
	}
	else if(TempBasePlaneSet >= 10){
		*(TempPtr++) = TempBasePlaneSet%100/10 + '0';
		*(TempPtr++) = TempBasePlaneSet%10 + '0';
	}
	else
		*(TempPtr++) = TempBasePlaneSet + '0';
	*(TempPtr++) = 30;
	*(TempPtr++) = 'C';
	
	while(TempPtr-Data->TextUp < 9)
		*(TempPtr++) = ' ';		
	*(TempPtr++) = 0;
}


void STATUS::GenerateDashSquare1A_H(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	*(TempPtr++) = ' ';
	if(PowerHeaterExtruderA%1000/100)
		*(TempPtr++) = PowerHeaterExtruderA%1000/100+'0';
	else
		*(TempPtr++) = ' ';

	if(PowerHeaterExtruderA%100/10 || PowerHeaterExtruderA%1000/100)
		*(TempPtr++) = PowerHeaterExtruderA%100/10+'0';
	else
		*(TempPtr++) = ' ';	
	
	*(TempPtr++) = PowerHeaterExtruderA%10+'0';
	*(TempPtr++) = '%';
	*(TempPtr++) = 0;
	Data->PercentBar = PowerHeaterExtruderA;
}


void STATUS::GenerateDashSquare1B_H(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	*(TempPtr++) = ' ';
	if(PowerCoolerExtruderA%1000/100)
		*(TempPtr++) = PowerCoolerExtruderA%1000/100+'0';
	else
		*(TempPtr++) = ' ';

	if(PowerCoolerExtruderA%100/10 || PowerCoolerExtruderA%1000/100)
		*(TempPtr++) = PowerCoolerExtruderA%100/10+'0';
	else
		*(TempPtr++) = ' ';	
	
	*(TempPtr++) = PowerCoolerExtruderA%10+'0';
	*(TempPtr++) = '%';
	*(TempPtr++) = 0;
	Data->PercentBar = PowerCoolerExtruderA;
}


void STATUS::GenerateDashSquare2A_H(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	*(TempPtr++) = ' ';
	if(PowerHeaterExtruderB%1000/100)
		*(TempPtr++) = PowerHeaterExtruderB%1000/100+'0';
	else
		*(TempPtr++) = ' ';

	if(PowerHeaterExtruderB%100/10 || PowerHeaterExtruderB%1000/100)
		*(TempPtr++) = PowerHeaterExtruderB%100/10+'0';
	else
		*(TempPtr++) = ' ';	
	
	*(TempPtr++) = PowerHeaterExtruderB%10+'0';
	*(TempPtr++) = '%';
	*(TempPtr++) = 0;
	Data->PercentBar = PowerHeaterExtruderB;
}


void STATUS::GenerateDashSquare2B_H(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	*(TempPtr++) = ' ';
	if(PowerCoolerExtruderB%1000/100)
		*(TempPtr++) = PowerCoolerExtruderB%1000/100+'0';
	else
		*(TempPtr++) = ' ';

	if(PowerCoolerExtruderB%100/10 || PowerCoolerExtruderB%1000/100)
		*(TempPtr++) = PowerCoolerExtruderB%100/10+'0';
	else
		*(TempPtr++) = ' ';	
	
	*(TempPtr++) = PowerCoolerExtruderB%10+'0';
	*(TempPtr++) = '%';
	*(TempPtr++) = 0;
	Data->PercentBar = PowerCoolerExtruderB;
}


void STATUS::GenerateDashSquare3A_H(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	*(TempPtr++) = ' ';
	if(PowerHeaterBasePlane%1000/100)
		*(TempPtr++) = PowerHeaterBasePlane%1000/100+'0';
	else
		*(TempPtr++) = ' ';

	if(PowerHeaterBasePlane%100/10 || PowerHeaterBasePlane%1000/100)
		*(TempPtr++) = PowerHeaterBasePlane%100/10+'0';
	else
		*(TempPtr++) = ' ';	
	
	*(TempPtr++) = PowerHeaterBasePlane%10+'0';
	*(TempPtr++) = '%';
	*(TempPtr++) = 0;
	Data->PercentBar = PowerHeaterBasePlane;
}


void STATUS::GenerateDashSquare3B_H(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	*(TempPtr++) = ' ';
	if(TempAir%100/10)
		*(TempPtr++) = TempAir%100/10+'0';
	else
		*(TempPtr++) = ' ';	
	
	*(TempPtr++) = TempAir%10+'0';
	*(TempPtr++) = 30;
	*(TempPtr++) = 'C';
	*(TempPtr++) = 0;
	Data->PercentBar = TempAir;
}


void STATUS::GeneratePicture1_H(BinaryPictureParameter *Data){
	if(StateExtruderA == 'H')
		Data->Color = COLOR_HEATING;
	else if(StateExtruderA == 'C')
		Data->Color = COLOR_COOLING;
	else if(StateExtruderA == 'M')
		Data->Color = COLOR_HOLDING;
	else 
		Data->Color = COLOR_OFF;
}


void STATUS::GeneratePicture2_H(BinaryPictureParameter *Data){
	if(StateExtruderB == 'H')
		Data->Color = COLOR_HEATING;
	else if(StateExtruderB == 'C')
		Data->Color = COLOR_COOLING;
	else if(StateExtruderB == 'M')
		Data->Color = COLOR_HOLDING;
	else 
		Data->Color = COLOR_OFF;
}


void STATUS::GeneratePicture3_H(BinaryPictureParameter *Data){
	if(StateBasePlane == 'H')
		Data->Color = COLOR_HEATING;
	else if(StateBasePlane == 'C')
		Data->Color = COLOR_COOLING;
	else if(StateBasePlane == 'M')
		Data->Color = COLOR_HOLDING;
	else 
		Data->Color = COLOR_OFF;
}

	
void STATUS::GenerateTextStatus1_H(){
	static u16 PosiStatus1[] = {95,78};
	if(StateExtruderA == 'H')
		Window.DrawString(PosiStatus1,STRING_HEATING,2,COLOR_HEATING);
	else if(StateExtruderA == 'C')
		Window.DrawString(PosiStatus1,STRING_COOLING,2,COLOR_COOLING);
	else if(StateExtruderA == 'M')
		Window.DrawString(PosiStatus1,STRING_HOLDING,2,COLOR_HOLDING);
	else 
		Window.DrawString(PosiStatus1,STRING_OFF,2,COLOR_OFF);
}


void STATUS::GenerateTextStatus2_H(){
	static u16 PosiStatus2[] = {95,187};
	if(StateExtruderB == 'H')
		Window.DrawString(PosiStatus2,STRING_HEATING,2,COLOR_HEATING);
	else if(StateExtruderB == 'C')
		Window.DrawString(PosiStatus2,STRING_COOLING,2,COLOR_COOLING);
	else if(StateExtruderB == 'M')
		Window.DrawString(PosiStatus2,STRING_HOLDING,2,COLOR_HOLDING);
	else 
		Window.DrawString(PosiStatus2,STRING_OFF,2,COLOR_OFF);
}


void STATUS::GenerateTextStatus3_H(){
	static u16 PosiStatus3[] = {95,296};
	if(StateBasePlane == 'H')
		Window.DrawString(PosiStatus3,STRING_HEATING,2,COLOR_HEATING);
	else if(StateBasePlane == 'C')
		Window.DrawString(PosiStatus3,STRING_COOLING,2,COLOR_COOLING);
	else if(StateBasePlane == 'M')
		Window.DrawString(PosiStatus3,STRING_HOLDING,2,COLOR_HOLDING);
	else 
		Window.DrawString(PosiStatus3,STRING_OFF,2,COLOR_OFF);
}


//--------------------------------------------------------//


char *STRING_POWERDOWN = "STOP";
char *STRING_NOTHOMING = "OFF ";
char *STRING_WORKING =   "RUN ";
char *STRING_IDLE =      "IDLE";

char *STRING_ACTICATED =   "ON  ";
char *STRING_DEACTIVATED = "OFF ";


void STATUS::GenerateDashSquare1_S(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	if(PosiX >= 100){
		*(TempPtr++) = PosiX%1000/100 + '0';
		*(TempPtr++) = PosiX%100/10 + '0';
		*(TempPtr++) = PosiX%10 + '0';
	}
	else if(PosiX >= 10){
		*(TempPtr++) = PosiX%100/10 + '0';
		*(TempPtr++) = PosiX%10 + '0';
	}
	else
		*(TempPtr++) = PosiX + '0';
	
	*(TempPtr++) = '/';
	
	if(PosiXMax >= 100){
		*(TempPtr++) = PosiXMax%1000/100 + '0';
		*(TempPtr++) = PosiXMax%100/10 + '0';
		*(TempPtr++) = PosiXMax%10 + '0';
	}
	else if(PosiXMax >= 10){
		*(TempPtr++) = PosiXMax%100/10 + '0';
		*(TempPtr++) = PosiXMax%10 + '0';
	}
	else
		*(TempPtr++) = PosiXMax + '0';
	
	*(TempPtr++) = 'm';
	*(TempPtr++) = 'm';
	while(TempPtr-Data->Text2 < 9)
		*(TempPtr++) = ' ';
	*(TempPtr++) = 0;
	Data->PercentBar = 100*PosiX/PosiXMax;
}


void STATUS::GenerateDashSquare2_S(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	if(PosiY >= 100){
		*(TempPtr++) = PosiY%1000/100 + '0';
		*(TempPtr++) = PosiY%100/10 + '0';
		*(TempPtr++) = PosiY%10 + '0';
	}
	else if(PosiY >= 10){
		*(TempPtr++) = PosiY%100/10 + '0';
		*(TempPtr++) = PosiY%10 + '0';
	}
	else
		*(TempPtr++) = PosiY + '0';
	
	*(TempPtr++) = '/';
	
	if(PosiYMax >= 100){
		*(TempPtr++) = PosiYMax%1000/100 + '0';
		*(TempPtr++) = PosiYMax%100/10 + '0';
		*(TempPtr++) = PosiYMax%10 + '0';
	}
	else if(PosiYMax >= 10){
		*(TempPtr++) = PosiYMax%100/10 + '0';
		*(TempPtr++) = PosiYMax%10 + '0';
	}
	else
		*(TempPtr++) = PosiYMax + '0';
	
	*(TempPtr++) = 'm';
	*(TempPtr++) = 'm';
	while(TempPtr-Data->Text2 < 9)
		*(TempPtr++) = ' ';
	*(TempPtr++) = 0;
	Data->PercentBar = 100*PosiY/PosiYMax;
}


void STATUS::GenerateDashSquare3_S(DashSquareParameter *Data){
	char *TempPtr = Data->Text2;
	if(PosiZ >= 100){
		*(TempPtr++) = PosiZ%1000/100 + '0';
		*(TempPtr++) = PosiZ%100/10 + '0';
		*(TempPtr++) = PosiZ%10 + '0';
	}
	else if(PosiZ >= 10){
		*(TempPtr++) = PosiZ%100/10 + '0';
		*(TempPtr++) = PosiZ%10 + '0';
	}
	else
		*(TempPtr++) = PosiZ + '0';
	
	*(TempPtr++) = '/';
	
	if(PosiZMax >= 100){
		*(TempPtr++) = PosiZMax%1000/100 + '0';
		*(TempPtr++) = PosiZMax%100/10 + '0';
		*(TempPtr++) = PosiZMax%10 + '0';
	}
	else if(PosiZMax >= 10){
		*(TempPtr++) = PosiZMax%100/10 + '0';
		*(TempPtr++) = PosiZMax%10 + '0';
	}
	else
		*(TempPtr++) = PosiZMax + '0';
	
	*(TempPtr++) = 'm';
	*(TempPtr++) = 'm';
	while(TempPtr-Data->Text2 < 9)
		*(TempPtr++) = ' ';
	*(TempPtr++) = 0;
	Data->PercentBar = 100*PosiZ/PosiZMax;
}

void STATUS::GenerateTextLevel1_S(){
	static u8 BinPicCache[] = {
		0x00,0x03,0xFC,
		0x3F,0xC3,0x04,
		0x38,0x43,0x04,
		0x38,0x43,0x04,
		0x38,0x43,0x04,
		0x3F,0xC0,0x00
	};
	
	static BinaryPictureParameter BinPicture = { //二值图片参数
		/* Posi[2]        */ {296,247}, //CacheL1坐标
		/* *Picture       */ BinPicCache, //图片 行优先
		/* NumbleRow      */ 12, //行数
		/* NumbleColumn   */ 12, //列数
		/* Zoom           */ 2, //缩放
		/* Color          */ 0xC618 //颜色
	};	
	
	if(L1Used == L1Max) 
		BinPicture.Color = 0xC618;
	else
		BinPicture.Color = 0x6C7B;
	
	Window.BinaryPicture(BinPicture);
	
	static char TempText[9] = {};
	static char *LEVEL1 = "L1Cache";
	static u16 PosiLevelA[] = {296,229};
	static u16 PosiLevelB[] = {316,251};
	
	char *TempPtr = TempText;
	
	if(L1Used >= 10){
		*(TempPtr++) = L1Used%100/10 + '0';
		*(TempPtr++) = L1Used%10 + '0';
	}
	else
		*(TempPtr++) = L1Used + '0';
	
	*(TempPtr++) = '/';
	
	if(L1Max >= 10){
		*(TempPtr++) = L1Max%100/10 + '0';
		*(TempPtr++) = L1Max%10 + '0';
	}
	else
		*(TempPtr++) = L1Max + '0';
	
	while(TempPtr-TempText < 5)
		*(TempPtr++) = ' ';
	*(TempPtr++) = 0;
	
	Window.DrawString(PosiLevelA,LEVEL1,2,0xC618);
	Window.DrawString(PosiLevelB,TempText,2,0xC618);
}


void STATUS::GenerateTextLevel2_S(){
	static u8 BinPicCache[] = {
		0x00,0x03,0xFC,
		0x3F,0xC3,0x04,
		0x38,0x43,0x04,
		0x38,0x43,0x04,
		0x38,0x43,0x04,
		0x3F,0xC0,0x00
	};
	
	static BinaryPictureParameter BinPicture = { //二值图片参数
		/* Posi[2]        */ {296,290}, //CacheL2坐标
		/* *Picture       */ BinPicCache, //图片 行优先
		/* NumbleRow      */ 12, //行数
		/* NumbleColumn   */ 12, //列数
		/* Zoom           */ 2, //缩放
		/* Color          */ 0xC618 //颜色
	};	

	if(L2Used == L2Max) 
		BinPicture.Color = 0xC618;
	else
		BinPicture.Color = 0x6C7B;
	
	Window.BinaryPicture(BinPicture);
	
	static char TempText[9] = {};
	static char *LEVEL2 = "L2Cache";
	static u16 PosiLevelA[] = {296,272};
	static u16 PosiLevelB[] = {316,294};
	
	char *TempPtr = TempText;
	
	if(L2Used >= 10){
		*(TempPtr++) = L2Used%100/10 + '0';
		*(TempPtr++) = L2Used%10 + '0';
	}
	else
		*(TempPtr++) = L2Used + '0';
	
	*(TempPtr++) = '/';
	
	if(L2Max >= 10){
		*(TempPtr++) = L2Max%100/10 + '0';
		*(TempPtr++) = L2Max%10 + '0';
	}
	else
		*(TempPtr++) = L2Max + '0';
	
	while(TempPtr-TempText < 5)
		*(TempPtr++) = ' ';
	*(TempPtr++) = 0;
	
	Window.DrawString(PosiLevelA,LEVEL2,2,0xC618);
	Window.DrawString(PosiLevelB,TempText,2,0xC618);
}



void STATUS::GeneratePicture1_S(BinaryPictureParameter *Data){
	static u8 BinPicAutomate_D[] = {
		0x07,0xFF,0xE0,
		0x0F,0xFF,0xF0,
		0x0C,0x03,0xB0,
		0x0C,0x07,0x30,
		0x0C,0x0E,0x30,
		0x0C,0x1C,0x30,
		0x0C,0x38,0x30,
		0x0C,0x70,0x30,
		0x0C,0xE0,0x30,
		0x0D,0xC0,0x30,
		0x0F,0xFF,0xF0,
		0x07,0xFF,0xE0
	};
	static u8 BinPicAutomate_N[] = {
		0x01,0x81,0x80,
		0x03,0x00,0xC0,
		0x06,0x00,0x60,
		0x0C,0x00,0x30,
		0x1F,0xF8,0x18,
		0x3F,0xF8,0x0C,
		0x30,0x1F,0xFC,
		0x18,0x1F,0xF8,
		0x0C,0x00,0x30,
		0x06,0x00,0x60,
		0x03,0x00,0xC0,
		0x01,0x81,0x80
	};
	static u8 BinPicAutomate_W[] = {
		0x07,0xFF,0xE0,
		0x0F,0xFF,0xF0,
		0x0C,0x00,0x30,
		0x00,0x00,0x30,
		0x0C,0x00,0x30,
		0x1E,0x00,0xFC,
		0x3F,0x00,0x78,
		0x0C,0x00,0x30,
		0x0C,0x00,0x00,
		0x0C,0x00,0x30,
		0x0F,0xFF,0xF0,
		0x07,0xFF,0xE0
	};
/*	static u8 BinPicAutomate_S[] = {
		0x11,0xFF,0x88,
		0x33,0xFF,0xCC,
		0x3F,0x00,0xCC,
		0x3F,0x00,0xCC,
		0x33,0x00,0xFC,
		0x33,0x00,0xFC,
		0x3F,0x00,0xCC,
		0x3F,0x00,0xCC,
		0x33,0x00,0xFC,
		0x33,0x00,0xFC,
		0x33,0xFF,0xCC,
		0x11,0xFF,0x88
	};*/
	static u8 BinPicAutomate_I[] = {
		0x07,0xFF,0xE0,
		0x0F,0xFF,0xF0,
		0x0C,0x00,0x30,
		0x0C,0x00,0x30,
		0x0C,0x00,0x30,
		0x0C,0x00,0xFC,
		0x0C,0x00,0x78,
		0x0C,0x00,0x30,
		0x0C,0x00,0x00,
		0x0C,0x00,0x30,
		0x0F,0xFF,0xF0,
		0x07,0xFF,0xE0
	};
		
	if(StateFA == 'N'){
		Data->Picture = BinPicAutomate_N;
		Data->Color = COLOR_NOTHOME;
	}
/*	else if(StateFA == 'S'){
		Data->Picture = BinPicAutomate_S;
		Data->Color = COLOR_SWITCHED;
	}*/
	else if(StateFA == 'W'){
		Data->Picture = BinPicAutomate_W;
		Data->Color = COLOR_WORKING;
	}
	else if(StateFA == 'I'){
		Data->Picture = BinPicAutomate_I;
		Data->Color = COLOR_IDLEING;
	}
	else{
		Data->Picture = BinPicAutomate_D;
		Data->Color = COLOR_DISABLED;
	}
}


void STATUS::GeneratePicture2_S(BinaryPictureParameter *Data){
	static u8 BinPicIDP_E[] = {
		0x0C,0x00,0x30,
		0x18,0x00,0x18,
		0x30,0x80,0x0C,
		0x31,0xC0,0x0C,
		0x33,0x60,0x0C,
		0x36,0x30,0x0C,
		0x7C,0x18,0x3E,
		0x30,0x0C,0x6C,
		0x30,0x06,0xCC,
		0x30,0x03,0x8C,
		0x18,0x01,0x18,
		0x0C,0x00,0x30
	};
	static u8 BinPicIDP_D[] = {
		0x00,0x00,0x00,
		0x00,0x00,0x00,
		0x00,0x80,0x00,
		0x01,0xC0,0x00,
		0x03,0x60,0x00,
		0x06,0x30,0x00,
		0x7C,0x18,0x3E,
		0x00,0x0C,0x60,
		0x00,0x06,0xC0,
		0x00,0x03,0x80,
		0x00,0x01,0x00,
		0x00,0x00,0x00
	};
	if(StateIDP == 'E'){
		Data->Picture = BinPicIDP_E;
		Data->Color = COLOR_ENABLE;
	}
	else{
		Data->Picture = BinPicIDP_D;
		Data->Color = COLOR_DISABLED;
	}
}

	
void STATUS::GenerateTextStatus1_S(){
	static u16 PosiStatus1[] = {415,251};
	if(StateFA == 'N')
		Window.DrawString(PosiStatus1,STRING_NOTHOMING,2,COLOR_NOTHOME);
	else if(StateFA == 'W')
		Window.DrawString(PosiStatus1,STRING_WORKING,2,COLOR_WORKING);
	else if(StateFA == 'I')
		Window.DrawString(PosiStatus1,STRING_IDLE,2,COLOR_IDLEING);	
	else 
		Window.DrawString(PosiStatus1,STRING_POWERDOWN,2,COLOR_DISABLED);
}


void STATUS::GenerateTextStatus2_S(){
	static u16 PosiStatus2[] = {415,294};
	if(StateIDP == 'E')
		Window.DrawString(PosiStatus2,STRING_ACTICATED,2,COLOR_ENABLE);
	else 
		Window.DrawString(PosiStatus2,STRING_DEACTIVATED,2,COLOR_DISABLED);
}

/*Cache
□□□□□□□□□□□□       0000 0000 0000 000
□□■■■■■■■■□□       0011 1111 1100 3FC,
□□■■■■■■■■□□       0011 1111 1100 3FC
□□■■□□□□□■□□       0011 0000 0100 304,
□□■■■□□□□■□□       0011 1000 0100 384
□□■■□□□□□■□□       0011 0000 0100 304,
□□■■■□□□□■□□       0011 1000 0100 384
□□■■□□□□□■□□       0011 0000 0100 304,
□□■■■□□□□■□□       0011 1000 0100 384
□□■■□□□□□■□□       0011 0000 0100 304,
□□■■■■■■■■□□       0011 1111 1100 3FC
□□□□□□□□□□□□       0000 0000 0000 000,
*/ 


