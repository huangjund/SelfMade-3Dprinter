#include "LCD.h"

LCD LCDInterface;

inline void LCD_DATA(u8 value){
	//DATA PA7:0
	GPIOA->ODR &= 0xFF00;
	GPIOA->ODR |= value;
}
inline void LCD_RST(bool value){
	//RST  PB9 /Reset
	if(value) GPIOB->BSRR |= 0x01<<9;
	else GPIOB->BRR |= 0x01<<9;
}
inline void LCD_CS(bool value){
	//CS   PB8 /Chip Select
	if(value) GPIOB->BSRR |= 0x01<<8;
	else GPIOB->BRR |= 0x01<<8;
}
inline void LCD_RS(bool value){
	//RS   PB7 /Register SRAM
	if(value) GPIOB->BSRR |= 0x01<<7;
	else GPIOB->BRR |= 0x01<<7;
}
inline void LCD_WR(bool value){
	//WR   PB6 /Write
	if(value) GPIOB->BSRR |= 0x01<<6;
	else GPIOB->BRR |= 0x01<<6;
}
inline void LCD_RD(bool value){
	//RD   PB5 /Read
	if(value) GPIOB->BSRR |= 0x01<<5;
	else GPIOB->BRR |= 0x01<<5;
}

void LCD::Init(){
	RCC->APB2ENR |= 0x01<<2; //GPIOA
	RCC->APB2ENR |= 0x01<<3; //GPIOB
	GPIOA->CRL = 0x33333333; //GPIOAL - PP
	GPIOB->CRH &= 0xFFFFFF00;
	GPIOB->CRH |= 0x00000033; //GPIOB9:8 - PP
	GPIOB->CRL &= 0x000FFFFF;
	GPIOB->CRL |= 0x33300000; //GPIOB7:5 - PP
	
	LCD_RD(1);
	LCDReset();
	
	SetAddress(0x11);
	Delay_Ms(20);
	SetAddress(0xD0);
	WriteValue(0x07);
	WriteValue(0x42);
	WriteValue(0x18);
	
	SetAddress(0xD1);
	WriteValue(0x00);
	WriteValue(0x07);
	WriteValue(0x10);
	
	SetAddress(0xD2);
	WriteValue(0x01);
	WriteValue(0x02);
	
	SetAddress(0xC0);
	WriteValue(0x10);
	WriteValue(0x3B);
	WriteValue(0x00);
	WriteValue(0x02);
	WriteValue(0x11);
	
	SetAddress(0xC5);
	WriteValue(0x03);
	
	SetAddress(0xC8);
	WriteValue(0x00);
	WriteValue(0x32);
	WriteValue(0x36);
	WriteValue(0x45);
	WriteValue(0x06);
	WriteValue(0x16);
	WriteValue(0x37);
	WriteValue(0x75);
	WriteValue(0x77);
	WriteValue(0x54);
	WriteValue(0x0C);
	WriteValue(0x00);
	
	SetAddress(0x36);
	WriteValue(0x0A);
	
	SetAddress(0x3A);
	WriteValue(0x55);
	
	SetAddress(0x2A);
	WriteValue(0x00);
	WriteValue(0x00);
	WriteValue(0x01);
	WriteValue(0x3F);
	
	SetAddress(0x2B);
	WriteValue(0x00);
	WriteValue(0x00);
	WriteValue(0x01);
	WriteValue(0xE0);
	Delay_Ms(120);
	SetAddress(0x29);
	SetAddress(0x002c); 
	
	WriteReg(0x36,0x28);
	
	Clear();
}

void LCD::Clear(){
	SetAddress(0x2A);	//SetX
	WriteValue(0x00); //Start 0 0x00
	WriteValue(0x00);		
	WriteValue(0x01); //End 479 0x01DF
	WriteValue(0xDF);

	SetAddress(0x2B);	//SetY
	WriteValue(0x00); //Start 0 0x00
	WriteValue(0x00);		
	WriteValue(0x01); //End 319 0x013F
	WriteValue(0x3F);	
	
	SetAddress(0x2C); //Prepare to Write GRAM
	
	u32 counter = 153600;
	while(counter--){
		LCD_RS(1);
		LCD_CS(0);
		LCD_DATA(BACKGROUND>>8);	
		LCD_WR(0);
		LCD_WR(1);
		LCD_DATA(BACKGROUND);
		LCD_WR(0);
		LCD_WR(1);
		LCD_CS(1);
	}
}

void LCD::DrawPoint(u16 PosiX,u16 PosiY,u8 Width,u16 Color){
	u16 StartX = PosiX - Width/2;
	u16 EndX = StartX + Width;
	u16 StartY = PosiY - Width/2;
	for(u32 y=0;y<Width;y++){
		SetPixelLine(StartX,StartY+y,EndX,Color);
	}
}

void LCD::WriteReg(u8 Addr,u8 Value){
	SetAddress(Addr);
	WriteValue(Value);
}



void LCD::LCDReset(){
	LCD_RST(1);
	Delay_Ms(50);
	LCD_RST(0);
	Delay_Ms(50);
	LCD_RST(1);
	Delay_Ms(50);
}

void LCD::SetAddress(u8 Addr){
	LCD_RS(0);
	LCD_CS(0);
	LCD_DATA(Addr);	
	LCD_WR(0);
	LCD_WR(1);
	LCD_CS(1);
}

void LCD::WriteValue(u8 Value){
	LCD_RS(1);
	LCD_CS(0);
	LCD_DATA(Value);
	LCD_WR(0);
	LCD_WR(1);
	LCD_CS(1);
}

void LCD::SetCursor(u16 PosiX,u16 PosiY){
	SetAddress(0x2A);	//SetX
	WriteValue(PosiX>>8); //PosiX
	WriteValue(PosiX);		
	WriteValue(PosiX>>8); //PosiX
	WriteValue(PosiX);	
	
	SetAddress(0x2B);	//SetY
	WriteValue(PosiY>>8); //PosiY
	WriteValue(PosiY);		
	WriteValue(PosiY>>8); //PosiY
	WriteValue(PosiY);	
	
	SetAddress(0x2C); //Prepare to Write GRAM
}

void LCD::SetPixelColor(u16 PosiX,u16 PosiY,u16 Color){
	SetCursor(PosiX,PosiY); //设置坐标
	
	LCD_RS(1); //设置颜色
	LCD_CS(0);
	LCD_DATA(Color>>8);	
	LCD_WR(0);
	LCD_WR(1);
	LCD_DATA(Color);
	LCD_WR(0);
	LCD_WR(1);
	LCD_CS(1);
}

void LCD::SetPixelLine(u16 PosiX,u16 PosiY,u16 EndX,u16 Color){
	SetAddress(0x2A);	//SetX
	WriteValue(PosiX>>8); //PosiX
	WriteValue(PosiX);		
	WriteValue(EndX>>8); //EndX
	WriteValue(EndX);	
	
	SetAddress(0x2B);	//SetY
	WriteValue(PosiY>>8); //PosiY
	WriteValue(PosiY);		
	WriteValue(PosiY>>8); //PosiY
	WriteValue(PosiY);	
	
	SetAddress(0x2C); //Prepare to Write GRAM
	
	u16 counter = EndX - PosiX;
	while(counter--){
		LCD_RS(1); //设置颜色
		LCD_CS(0);
		LCD_DATA(Color>>8);	
		LCD_WR(0);
		LCD_WR(1);
		LCD_DATA(Color);
		LCD_WR(0);
		LCD_WR(1);
		LCD_CS(1);
	}
}

void LCD::SetPixelUpright(u16 PosiX,u16 PosiY,u16 EndY,u16 Color){
	SetAddress(0x2A);	//SetX
	WriteValue(PosiX>>8); //PosiX
	WriteValue(PosiX);		
	WriteValue(PosiX>>8); //EndX
	WriteValue(PosiX);	
	
	SetAddress(0x2B);	//SetY
	WriteValue(PosiY>>8); //PosiY
	WriteValue(PosiY);		
	WriteValue(EndY>>8); //PosiY
	WriteValue(EndY);	
	
	SetAddress(0x2C); //Prepare to Write GRAM
	
	u16 counter = EndY - PosiY;
	while(counter--){
		LCD_RS(1); //设置颜色
		LCD_CS(0);
		LCD_DATA(Color>>8);	
		LCD_WR(0);
		LCD_WR(1);
		LCD_DATA(Color);
		LCD_WR(0);
		LCD_WR(1);
		LCD_CS(1);
	}
}

void LCD::DrawNBit(u16 PosiX,u16 PosiY,u32 Data,u8 Numble,u8 Zoom,u16 Color){
	SetAddress(0x2A);	//SetX
	WriteValue(PosiX>>8); //PosiX
	WriteValue(PosiX);		
	WriteValue(PosiX>>8); //PosiX
	WriteValue(PosiX);	
	
	SetAddress(0x2B);	//SetY
	WriteValue(PosiY>>8); //PosiY
	WriteValue(PosiY);		
	WriteValue((PosiY+Numble*Zoom)>>8); //EndY
	WriteValue(PosiY+Numble*Zoom);	
	
	SetAddress(0x2C); //Prepare to Write GRAM
	
	u8 counter = Numble;
	u16 SubColor;
	u8 SubZoom;
	while(counter--){
		if(Data&0x01) SubColor = Color;
		else SubColor = BACKGROUND;
		
		SubZoom = Zoom;
		while(SubZoom--){
			LCD_RS(1); //设置颜色
			LCD_CS(0);
			LCD_DATA(SubColor>>8);	
			LCD_WR(0);
			LCD_WR(1);
			LCD_DATA(SubColor);
			LCD_WR(0);
			LCD_WR(1);
			LCD_CS(1);
		}
		Data >>= 1;
	}
}

void LCD::Delay_Us(u32 n){
	SysTick->LOAD=72*n;
	SysTick->CTRL=0x00000005;
	while(!(SysTick->CTRL&0x00010000));
	SysTick->CTRL=0x00000004;
}
	
void LCD::Delay_Ms(u32 n){
	while(n--)
		Delay_Us(1000);	
}

