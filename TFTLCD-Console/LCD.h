#include "stm32f10x.h"
#define BACKGROUND 0x0000 //Black

class LCD{
	public:
		void Init(); //初始化
		void Clear(); //清屏
		void DrawPoint(u16 PosiX,u16 PosiY,u8 Width,u16 Color);
		void SetPixelLine(u16 PosiX,u16 PosiY,u16 EndX,u16 Color); //单像素水平线
		void SetPixelUpright(u16 PosiX,u16 PosiY,u16 EndY,u16 Color); //单像素竖线	
		void DrawNBit(u16 PosiX,u16 PosiY,u32 Data,u8 Numble,u8 Zoom,u16 Color); //填充N个连续的竖直像素
	private:
		void WriteReg(u8 Addr,u8 Value); //写寄存器
		void LCDReset(); //复位
		void SetAddress(u8 Addr); //设置寄存器地址
		void WriteValue(u8 Value); //写入数据
		void SetCursor(u16 PosiX,u16 PosiY); //设置光标
		void SetPixelColor(u16 PosiX,u16 PosiY,u16 Color); //设置像素颜色
		void Delay_Us(u32 n);
    void Delay_Ms(u32 n);
};

extern class LCD LCDInterface;
