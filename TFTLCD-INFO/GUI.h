#include "stm32f10x.h"

#ifndef _GUI_H
	#define _GUI_H
	struct DashRoundParameter{ //圆仪表盘参数
		u16 Posi[2]; //中心坐标XY
		u16 RadiusLargeRim; //大外环半径
		u16 RadiusSmallRim; //小外环半径
		u16 ColorRim; //外环颜色
		u16 ColorPointer; //指针颜色
		u16 ColorBar; //进度条颜色
		u16 ColorText; //文本颜色
		u8 PercentBar; //进度条百分比
		u8 PercentPointer; //指针百分比
		char *TextUp; //第一行文本
		char *TextDown; //第二行文本
		u8 TextZoom; //文本缩放
		u16 StartAngle; //起始角度
		u16 Angle; //总角度
	};


	struct BinaryPictureParameter{ //二值图片参数
		u16 Posi[2]; //左上角坐标
		u8 *Picture; //图片 行优先
		u8 NumbleRow; //行数
		u8 NumbleColumn; //列数
		u8 Zoom; //缩放
		u16 Color; //颜色
	};

	struct DashSquareParameter{ //方仪表盘参数
		u16 Posi[4]; //顶点坐标
		u8 Width; //线宽
		u16 ColorRim; //外环颜色
		u16 ColorBar; //进度条颜色
		u16 ColorText; //文本颜色
		u8 PercentBar; //进度条百分比
		char *Text1; //第一行文本
		char *Text2; //第二行文本
		u8 TextZoom; //文本缩放
	};
	
	class GUI{
		public:
			void Init(); //初始化
			void DashRound(DashRoundParameter Data); //圆仪表盘
			void DashSquare(DashSquareParameter Data); //方仪表盘
	//		void TextBox(); //文本框
			void BinaryPicture(BinaryPictureParameter Data); //二值图片
	//	private:
			void DrawLine(u16 Posi[4],u8 Width,u16 Color); //线，StartXY,EndXY
			void DrawRectangle(u16 Posi[4],u8 Width,u16 Color); //方，StartXY,EndXY
			void DrawFrame(u16 Posi[4],u16 ColorA,u16 ColorB); //框	
			void DrawCircle(u16 Posi[2],u8 Radius,u8 Width,u16 Color); //圆
			void DrawArc(u16 Posi[2],u8 Radius,u16 StartAngle,u16 Angle,u8 Width,u16 Color); //弧
			void DrawChar(u16 Posi[2],s8 Char,u8 Zoom,u16 Color); //ASCII字符
			void DrawString(u16 Posi[2],char String[],u8 Zoom, u16 Color); //字符串
			u32 Abs(s32 Data);
	};	
	
	extern class GUI Window;	

#endif
	
