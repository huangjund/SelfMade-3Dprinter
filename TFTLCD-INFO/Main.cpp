#include "stm32f10x.h"
#include "LCD.h"
#include "GUI.h"
#include "Status.h"
#include "COM.h"
/*
Grey: 0xC618,0x8430
Orange: 0xFBA0,0xFB20
Blue: 0x7D7C,0x6C7B
*/
#define COLORA 0x6C7B

void Refresh(); //刷屏

int main(){
	RCC->APB2ENR |= 0x01;
  AFIO->MAPR &= 0xF0FFFFFF;
  AFIO->MAPR |= 0x02000000;
	LCDInterface.Init();
	Serial.Init();
	Refresh();
	while(1){
		if(Serial.RxFinish){
			Serial.Decoder();
			Serial.Tx('D');			
			Refresh();
			Serial.RxFinish = 0;
		}
	}
}

void Refresh(){
	static u16 RectanglePosi1_H[4] = {1,1,280,104};
	static u16 RectanglePosi2_H[4] = {1,110,280,210};
	static u16 RectanglePosi3_H[4] = {1,216,280,318};
	Window.DrawFrame(RectanglePosi1_H,0xC618,0x8430);
	Window.DrawFrame(RectanglePosi2_H,0xC618,0x8430);
	Window.DrawFrame(RectanglePosi3_H,0xC618,0x8430);
	
	static char TextRound1_H[12] = {'3','2','0','/','3','2','0',30,'C',0};
	static char TextRound2_H[12] = {'3','2','0','/','3','2','0',30,'C',0};
	static char TextRound3_H[12] = {'1','5','0','/','1','5','0',30,'C',0};
	DashRoundParameter DashRound1_H = {
		/*Posi[2]         */  {60,56}, //中心坐标XY
		/*RadiusLargeRim  */  40, //大外环半径
		/*RadiusSmallRim  */  30, //小外环半径
		/*ColorRim        */  COLORA, //外环颜色
		/*ColorPointer    */  COLORA, //指针颜色
		/*ColorBar        */  0xC618, //进度条颜色
		/*ColorText       */  0xC618, //文本颜色
		/*PercentBar      */  100, //进度条百分比
		/*PercentPointer  */  100, //指针百分比
		/*TextUp          */  TextRound1_H, //第一行文本
		/*TextDown        */  "ExtruderA", //第二行文本
		/*TextZoom        */  2, //文本缩放
		/*StartAngle      */  180, //起始角度
		/*Angle           */  225 //总角度
	};
	DashRoundParameter DashRound2_H = {
		/*Posi[2]         */  {60,165}, //中心坐标XY
		/*RadiusLargeRim  */  40, //大外环半径
		/*RadiusSmallRim  */  30, //小外环半径
		/*ColorRim        */  COLORA, //外环颜色
		/*ColorPointer    */  COLORA, //指针颜色
		/*ColorBar        */  0xC618, //进度条颜色
		/*ColorText       */  0xC618, //文本颜色
		/*PercentBar      */  100, //进度条百分比
		/*PercentPointer  */  100, //指针百分比
		/*TextUp          */  TextRound2_H, //第一行文本
		/*TextDown        */  "ExtruderB", //第二行文本
		/*TextZoom        */  2, //文本缩放
		/*StartAngle      */  180, //起始角度
		/*Angle           */  225 //总角度
	};
	DashRoundParameter DashRound3_H = {
		/*Posi[2]         */  {60,271}, //中心坐标XY
		/*RadiusLargeRim  */  40, //大外环半径
		/*RadiusSmallRim  */  30, //小外环半径
		/*ColorRim        */  COLORA, //外环颜色
		/*ColorPointer    */  COLORA, //指针颜色
		/*ColorBar        */  0xC618, //进度条颜色
		/*ColorText       */  0xC618, //文本颜色
		/*PercentBar      */  100, //进度条百分比
		/*PercentPointer  */  100, //指针百分比
		/*TextUp          */  TextRound3_H, //第一行文本
		/*TextDown        */  "BasePlane", //第二行文本
		/*TextZoom        */  2, //文本缩放
		/*StartAngle      */  180, //起始角度
		/*Angle           */  225 //总角度
	};

	Status.GenerateDashRound1_H(&DashRound1_H);
	Status.GenerateDashRound2_H(&DashRound2_H);
	Status.GenetateDashRound3_H(&DashRound3_H);
	Window.DashRound(DashRound1_H);
	Window.DashRound(DashRound2_H);
	Window.DashRound(DashRound3_H);

	static char TextSquare1A_H[6] = {' ','4','0','%',0};
	static char TextSquare1B_H[6] = {' ','1','0','0','%',0};
	DashSquareParameter DashSquare1A_H = { //方仪表盘参数
		/* Posi[4]      */ {150,11,265,50}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ COLORA,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 40, //进度条百分比
		/* *Text1       */ "Heater", //第一行文本
		/* *Text2       */ TextSquare1A_H, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	DashSquareParameter DashSquare1B_H = { //方仪表盘参数
		/* Posi[4]      */ {150,51,265,90}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ COLORA,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 100, //进度条百分比
		/* *Text1       */ "Cooler", //第一行文本
		/* *Text2       */ TextSquare1B_H, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	Status.GenerateDashSquare1A_H(&DashSquare1A_H);
	Status.GenerateDashSquare1B_H(&DashSquare1B_H);
	Window.DashSquare(DashSquare1A_H);
	Window.DashSquare(DashSquare1B_H);
	
	static char TextSquare2A_H[6] = {' ','4','0','%',0};
	static char TextSquare2B_H[6] = {' ','1','0','0','%',0};
	DashSquareParameter DashSquare2A_H = { //方仪表盘参数
		/* Posi[4]      */ {150,120,265,159}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ COLORA,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 40, //进度条百分比
		/* *Text1       */ "Heater", //第一行文本
		/* *Text2       */ TextSquare2A_H, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	DashSquareParameter DashSquare2B_H = { //方仪表盘参数
		/* Posi[4]      */ {150,160,265,199}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ COLORA,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 100, //进度条百分比
		/* *Text1       */ "Cooler", //第一行文本
		/* *Text2       */ TextSquare2B_H, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	Status.GenerateDashSquare2A_H(&DashSquare2A_H);
	Status.GenerateDashSquare2B_H(&DashSquare2B_H);
	Window.DashSquare(DashSquare2A_H);
	Window.DashSquare(DashSquare2B_H);
	
	static char TextSquare3A_H[6] = {' ','4','0','%',0};
	static char TextSquare3B_H[6] = {' ','4','0',30,'C',0};
	DashSquareParameter DashSquare3A_H = { //方仪表盘参数
		/* Posi[4]      */ {150,229,265,268}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ COLORA,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 40, //进度条百分比
		/* *Text1       */ "Heater", //第一行文本
		/* *Text2       */ TextSquare3A_H, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	DashSquareParameter DashSquare3B_H = { //方仪表盘参数
		/* Posi[4]      */ {150,269,265,308}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ COLORA,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 40, //进度条百分比
		/* *Text1       */ " Tair", //第一行文本
		/* *Text2       */ TextSquare3B_H, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	Status.GenerateDashSquare3A_H(&DashSquare3A_H);
	Status.GenerateDashSquare3B_H(&DashSquare3B_H);
	Window.DashSquare(DashSquare3A_H);
	Window.DashSquare(DashSquare3B_H);

	static u8 BinPic1_H[] = {
		0x00,0x04,0x70,0x4F,
		0x85,0xFC,0x4F,0x85,
		0xFC,0x4F,0x85,0xFC,
		0x4F,0x87,0xF0,0x02,
		0x00,0x00
	};
	BinaryPictureParameter BinPicture1_H = { //二值图片参数
		/* Posi[2]        */ {70,75}, //左上角坐标
		/* *Picture       */ BinPic1_H, //图片 行优先
		/* NumbleRow      */ 12, //行数
		/* NumbleColumn   */ 12, //列数
		/* Zoom           */ 2, //缩放
		/* Color          */ 0xC618 //颜色
	};	
	Status.GeneratePicture1_H(&BinPicture1_H);
	Window.BinaryPicture(BinPicture1_H);

	static u8 BinPic2_H[] = {
		0x00,0x04,0x70,0x4F,
		0x85,0xFC,0x4F,0x85,
		0xFC,0x4F,0x85,0xFC,
		0x4F,0x87,0xF0,0x02,
		0x00,0x00
	};
	BinaryPictureParameter BinPicture2_H = { //二值图片参数
		/* Posi[2]        */ {70,184}, //左上角坐标
		/* *Picture       */ BinPic2_H, //图片 行优先
		/* NumbleRow      */ 12, //行数
		/* NumbleColumn   */ 12, //列数
		/* Zoom           */ 2, //缩放
		/* Color          */ 0xC618 //颜色
	};	
	Status.GeneratePicture2_H(&BinPicture2_H);
	Window.BinaryPicture(BinPicture2_H);
	
	static u8 BinPic3_H[] = {
		0x00,0x04,0x44,0x22,
		0x22,0x22,0x22,0x24,
		0x44,0x44,0x44,0x44,
		0x22,0x20,0x00,0xFF,
		0xF0,0x00
	};
	BinaryPictureParameter BinPicture3_H = { //二值图片参数
		/* Posi[2]        */ {70,293}, //左上角坐标
		/* *Picture       */ BinPic3_H, //图片 行优先
		/* NumbleRow      */ 12, //行数
		/* NumbleColumn   */ 12, //列数
		/* Zoom           */ 2, //缩放
		/* Color          */ 0xC618 //颜色
	};	
	Status.GeneratePicture3_H(&BinPicture3_H);
	Window.BinaryPicture(BinPicture3_H);

	Status.GenerateTextStatus1_H();
	Status.GenerateTextStatus2_H();
	Status.GenerateTextStatus3_H();
	
	//----------------------------S------------------------------//
	static u16 RectanglePosi1_S[4] = {285,1,478,66};	
	static u16 RectanglePosi2_S[4] = {285,72,478,138};
	static u16 RectanglePosi3_S[4] = {285,144,478,210};	
	static u16 RectanglePosi4_S[4] = {285,216,478,318};
	
	Window.DrawFrame(RectanglePosi1_S,0xC618,0x8430);
	Window.DrawFrame(RectanglePosi2_S,0xC618,0x8430);	
	Window.DrawFrame(RectanglePosi3_S,0xC618,0x8430);
	Window.DrawFrame(RectanglePosi4_S,0xC618,0x8430);	
	

	static char TextSquare1_S[10] = {'0','/','0','m','m',0};
	static char TextSquare2_S[10] = {'0','/','0','m','m',0};
	static char TextSquare3_S[10] = {'0','/','0','m','m',0};
	
	DashSquareParameter DashSquare1_S = { //方仪表盘参数
		/* Posi[4]      */ {296,15,467,53}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ 0x6C7B,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 40, //进度条百分比
		/* *Text1       */ "PositionX", //第一行文本
		/* *Text2       */ TextSquare1_S, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	DashSquareParameter DashSquare2_S = { //方仪表盘参数
		/* Posi[4]      */ {296,86,467,124}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ 0x6C7B,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 100, //进度条百分比
		/* *Text1       */ "PositionY", //第一行文本
		/* *Text2       */ TextSquare2_S, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	DashSquareParameter DashSquare3_S = { //方仪表盘参数
		/* Posi[4]      */ {296,158,467,196}, //顶点坐标
		/* Width        */ 3, //线宽
		/* ColorRim     */ 0x6C7B,  //外环颜色
		/* ColorBar     */ 0xC618, //进度条颜色
		/* ColorText    */ 0xC618, //文本颜色
		/* PercentBar   */ 100, //进度条百分比
		/* *Text1       */ "PositionZ", //第一行文本
		/* *Text2       */ TextSquare3_S, //第二行文本
		/* TextZoom     */ 2 //文本缩放
	};
	Status.GenerateDashSquare1_S(&DashSquare1_S);
	Status.GenerateDashSquare2_S(&DashSquare2_S);
	Status.GenerateDashSquare3_S(&DashSquare3_S);
	Window.DashSquare(DashSquare1_S);
	Window.DashSquare(DashSquare2_S);
	Window.DashSquare(DashSquare3_S);

	Status.GenerateTextLevel1_S();
	Status.GenerateTextLevel2_S();
	
	
	static u16 Text1Position_S[2] = {390,229};
	static u16 Text2Position_S[2] = {390,272};
	Window.DrawString(Text1Position_S,"Status",2,0xC618);
	Window.DrawString(Text2Position_S,"I.D.P.",2,0xC618);


	static u8 BinPicAutomate_D_S[] = {
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
	static u8 BinPicIDP_D_S[] = {
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
	BinaryPictureParameter BinPicture1_S = { //二值图片参数
		/* Posi[2]        */ {390,252}, //Automate坐标
		/* *Picture       */ BinPicAutomate_D_S, //图片 行优先
		/* NumbleRow      */ 12, //行数
		/* NumbleColumn   */ 24, //列数
		/* Zoom           */ 1, //缩放
		/* Color          */ 0xC618 //颜色
	};	
	Status.GeneratePicture1_S(&BinPicture1_S);
	Window.BinaryPicture(BinPicture1_S);

	BinaryPictureParameter BinPicture2_S = { //二值图片参数
		/* Posi[2]        */ {390,295}, //左上角坐标
		/* *Picture       */ BinPicIDP_D_S, //图片 行优先
		/* NumbleRow      */ 12, //行数
		/* NumbleColumn   */ 24, //列数
		/* Zoom           */ 1, //缩放
		/* Color          */ 0xC618 //颜色
	};	
	Status.GeneratePicture2_S(&BinPicture2_S);
	Window.BinaryPicture(BinPicture2_S);
//	
	Status.GenerateTextStatus1_S();
	Status.GenerateTextStatus2_S();
	
	static u16 PosiLine[4] = {384,229,384,310};
	Window.DrawLine(PosiLine,2,0xC618);
}

/*Extruder
□□□□□□□□□□□□       0000 0000 0000 000
□■□□□■■■□□□□       0100 0111 0000 470
□■□□■■■■■□□□       0100 1111 1000 4F8
□■□■■■■■■■□□       0101 1111 1100 5FC
□■□□■■■■■□□□       0100 1111 1000 4F8
□■□■■■■■■■□□       0101 1111 1100 5FC
□■□□■■■■■□□□       0100 1111 1000 4F8
□■□■■■■■■■□□       0101 1111 1100 5FC
□■□□■■■■■□□□       0100 1111 1000 4F8
□■■■■■■■□□□□       0111 1111 0000 7F0
□□□□□□■□□□□□       0000 0010 0000 020
□□□□□□□□□□□□       0000 0000 0000 000
*/ 

/*BasePlane
□□□□□□□□□□□□       0000 0000 0000 000
□■□□□■□□□■□□       0100 0100 0100 444
□□■□□□■□□□■□       0010 0010 0010 222
□□■□□□■□□□■□       0010 0010 0010 222
□□■□□□■□□□■□       0010 0010 0010 222
□■□□□■□□□■□□       0100 0100 0100 444
□■□□□■□□□■□□       0100 0100 0100 444
□■□□□■□□□■□□       0100 0100 0100 444
□□■□□□■□□□■□       0010 0010 0010 222
□□□□□□□□□□□□       0000 0000 0000 000
■■■■■■■■■■■■       1111 1111 1111 FFF
□□□□□□□□□□□□       0000 0000 0000 000
*/

//void Refresh(){


//}

/*Automate-Disabled
□□□□□■■■■■■■■■■■■■■□□□□□       0000 0111 1111 1111 1110 0000 07FFE0
□□□□■■■■■■■■■■■■■■■■□□□□       0000 1111 1111 1111 1111 0000 0FFFF0
□□□□■■□□□□□□□□■■■□■■□□□□       0000 1100 0000 0011 1011 0000 0C03B0
□□□□■■□□□□□□□■■■□□■■□□□□       0000 1100 0000 0111 0011 0000 0C0730
□□□□■■□□□□□□■■■□□□■■□□□□       0000 1100 0000 1110 0011 0000 0C0E30
□□□□■■□□□□□■■■□□□□■■□□□□       0000 1100 0001 1100 0011 0000 0C1C30
□□□□■■□□□□■■■□□□□□■■□□□□       0000 1100 0011 1000 0011 0000 0C3830
□□□□■■□□□■■■□□□□□□■■□□□□       0000 1100 0111 0000 0011 0000 0C7030
□□□□■■□□■■■□□□□□□□■■□□□□       0000 1100 1110 0000 0011 0000 0CE030
□□□□■■□■■■□□□□□□□□■■□□□□       0000 1101 1100 0000 0011 0000 0DC030
□□□□■■■■■■■■■■■■■■■■□□□□       0000 1111 1111 1111 1111 0000 0FFFF0
□□□□□■■■■■■■■■■■■■■□□□□□       0000 0111 1111 1111 1110 0000 07FFE0
*/


/*Automate-NotHome
□□□□□□□■■□□□□□□■■□□□□□□□       0000 0001 1000 0001 1000 0000 018180
□□□□□□■■□□□□□□□□■■□□□□□□       0000 0011 0000 0000 1100 0000 0300C0
□□□□□■■□□□□□□□□□□■■□□□□□       0000 0110 0000 0000 0110 0000 060060
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□■■■■■■■■■■□□□□□□■■□□□       0001 1111 1111 1000 0001 1000 1FF818
□□■■■■■■■■■■■□□□□□□■■■□□       0011 1111 1111 1000 0000 1100 3FF80C
□□■■■□□□□□□■■■■■■■■■■■□□       0011 0000 0001 1111 1111 1100 301FFC
□□□■■□□□□□□■■■■■■■■■■□□□       0001 1000 0001 1111 1111 1000 181FF8
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□□■■□□□□□□□□□□■■□□□□□       0000 0110 0000 0000 0110 0000 060060
□□□□□□■■□□□□□□□□■■□□□□□□       0000 0011 0000 0000 1100 0000 0300C0
□□□□□□□■■□□□□□□■■□□□□□□□       0000 0001 1000 0001 1000 0000 018180
*/


/*Automate-Switching
□□□□□■■■■■■■■■■■■■■□□□□□       0000 0111 1111 1111 1110 0000 07FFE0
□□□□■■■■■■■■■■■■■■■■□□□□       0000 1111 1111 1111 1111 0000 0FFFF0
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□□□□□□□□□□□□□□□■■□□□□       0000 0000 0000 0000 0011 0000 000030
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□■■■■□□□□□□□□□■■■■■■□□       0001 1110 0000 0000 1111 1100 1E00FC
□□■■■■■■□□□□□□□□□■■■■□□□       0011 1111 0000 0000 0111 1000 3F0078
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□■■□□□□□□□□□□□□□□□□□□       0000 1100 0000 0000 0000 0000 0C0000
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□■■■■■■■■■■■■■■■■□□□□       0000 1111 1111 1111 1111 0000 0FFFF0
□□□□□■■■■■■■■■■■■■■□□□□□       0000 0111 1111 1111 1110 0000 07FFE0
*/

/*Automate-Working
□□□■□□□■■■■■■■■■■□□□■□□□       0001 0001 1111 1111 1000 1000 11FF88
□□■■□□■■■■■■■■■■■■□□■■□□       0011 0011 1111 1111 1100 1100 33FFCC
□□■■■■■■□□□□□□□□■■□□■■□□       0011 1111 0000 0000 1100 1100 3F00CC
□□■■■■■■□□□□□□□□■■□□■■□□       0011 1111 0000 0000 1100 1100 3F00CC
□□■■□□■■□□□□□□□□■■■■■■□□       0011 0011 0000 0000 1111 1100 3300FC
□□■■□□■■□□□□□□□□■■■■■■□□       0011 0011 0000 0000 1111 1100 3300FC
□□■■■■■■□□□□□□□□■■□□■■□□       0011 1111 0000 0000 1100 1100 3F00CC
□□■■■■■■□□□□□□□□■■□□■■□□       0011 1111 0000 0000 1100 1100 3F00CC
□□■■□□■■□□□□□□□□■■■■■■□□       0011 0011 0000 0000 1111 1100 3300FC
□□■■□□■■□□□□□□□□■■■■■■□□       0011 0011 0000 0000 1111 1100 3300FC
□□■■□□■■■■■■■■■■■■□□■■□□       0011 0011 1111 1111 1100 1100 33FFCC
□□□■□□□■■■■■■■■■■□□□■□□□       0001 0001 1111 1111 1000 1000 11FF88
*/

/*Automate-Idel
□□□□□■■■■■■■■■■■■■■□□□□□       0000 0111 1111 1111 1110 0000 07FFE0
□□□□■■■■■■■■■■■■■■■■□□□□       0000 1111 1111 1111 1111 0000 0FFFF0
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□■■□□□□□□□□□□■■■■■■□□       0000 1100 0000 0000 1111 1100 0C00FC
□□□□■■□□□□□□□□□□□■■■■□□□       0000 1100 0000 0000 0111 1000 0C0078
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□■■□□□□□□□□□□□□□□□□□□       0000 1100 0000 0000 0000 0000 0C0000
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□□■■■■■■■■■■■■■■■■□□□□       0000 1111 1111 1111 1111 0000 0FFFF0
□□□□□■■■■■■■■■■■■■■□□□□□       0000 0111 1111 1111 1110 0000 07FFE0
*/

/*IDP-Activated
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
□□□■■□□□□□□□□□□□□□□■■□□□       0001 1000 0000 0000 0001 1000 180018
□□■■□□□□■□□□□□□□□□□□■■□□       0011 0000 1000 0000 0000 1100 30800C
□□■■□□□■■■□□□□□□□□□□■■□□       0011 0001 1100 0000 0000 1100 31C00C
□□■■□□■■□■■□□□□□□□□□■■□□       0011 0011 0110 0000 0000 1100 33600C
□□■■□■■□□□■■□□□□□□□□■■□□       0011 0110 0011 0000 0000 1100 36300C
□■■■■■□□□□□■■□□□□□■■■■■□       0111 1100 0001 1000 0011 1110 7C183E
□□■■□□□□□□□□■■□□□■■□■■□□       0011 0000 0000 1100 0110 1100 300C6C
□□■■□□□□□□□□□■■□■■□□■■□□       0011 0000 0000 0110 1100 1100 3006CC
□□■■□□□□□□□□□□■■■□□□■■□□       0011 0000 0000 0011 1000 1100 30038C
□□□■■□□□□□□□□□□■□□□■■□□□       0001 1000 0000 0001 0001 1000 180118
□□□□■■□□□□□□□□□□□□■■□□□□       0000 1100 0000 0000 0011 0000 0C0030
*/

/*IDP-Deactivated
□□□□□□□□□□□□□□□□□□□□□□□□       0000 0000 0000 0000 0000 0000 000000
□□□□□□□□□□□□□□□□□□□□□□□□       0000 0000 0000 0000 0000 0000 000000
□□□□□□□□■□□□□□□□□□□□□□□□       0000 0000 1000 0000 0000 0000 008000
□□□□□□□■■■□□□□□□□□□□□□□□       0000 0001 1100 0000 0000 0000 01C000
□□□□□□■■□■■□□□□□□□□□□□□□       0000 0011 0110 0000 0000 0000 036000
□□□□□■■□□□■■□□□□□□□□□□□□       0000 0110 0011 0000 0000 0000 063000
□■■■■■□□□□□■■□□□□□■■■■■□       0111 1100 0001 1000 0011 1110 7C183E
□□□□□□□□□□□□■■□□□■■□□□□□       0000 0000 0000 1100 0110 0000 000C60
□□□□□□□□□□□□□■■□■■□□□□□□       0000 0000 0000 0110 1100 0000 0006C0
□□□□□□□□□□□□□□■■■□□□□□□□       0000 0000 0000 0011 1000 0000 000380
□□□□□□□□□□□□□□□■□□□□□□□□       0000 0000 0000 0001 0000 0000 000100
□□□□□□□□□□□□□□□□□□□□□□□□       0000 0000 0000 0000 0000 0000 000000
*/


