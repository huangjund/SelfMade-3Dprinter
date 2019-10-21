#include "stm32f10x.h"

#ifndef _GUI_H
	#define _GUI_H
	struct DashRoundParameter{ //Բ�Ǳ��̲���
		u16 Posi[2]; //��������XY
		u16 RadiusLargeRim; //���⻷�뾶
		u16 RadiusSmallRim; //С�⻷�뾶
		u16 ColorRim; //�⻷��ɫ
		u16 ColorPointer; //ָ����ɫ
		u16 ColorBar; //��������ɫ
		u16 ColorText; //�ı���ɫ
		u8 PercentBar; //�������ٷֱ�
		u8 PercentPointer; //ָ��ٷֱ�
		char *TextUp; //��һ���ı�
		char *TextDown; //�ڶ����ı�
		u8 TextZoom; //�ı�����
		u16 StartAngle; //��ʼ�Ƕ�
		u16 Angle; //�ܽǶ�
	};


	struct BinaryPictureParameter{ //��ֵͼƬ����
		u16 Posi[2]; //���Ͻ�����
		u8 *Picture; //ͼƬ ������
		u8 NumbleRow; //����
		u8 NumbleColumn; //����
		u8 Zoom; //����
		u16 Color; //��ɫ
	};

	struct DashSquareParameter{ //���Ǳ��̲���
		u16 Posi[4]; //��������
		u8 Width; //�߿�
		u16 ColorRim; //�⻷��ɫ
		u16 ColorBar; //��������ɫ
		u16 ColorText; //�ı���ɫ
		u8 PercentBar; //�������ٷֱ�
		char *Text1; //��һ���ı�
		char *Text2; //�ڶ����ı�
		u8 TextZoom; //�ı�����
	};
	
	class GUI{
		public:
			void Init(); //��ʼ��
			void DashRound(DashRoundParameter Data); //Բ�Ǳ���
			void DashSquare(DashSquareParameter Data); //���Ǳ���
	//		void TextBox(); //�ı���
			void BinaryPicture(BinaryPictureParameter Data); //��ֵͼƬ
	//	private:
			void DrawLine(u16 Posi[4],u8 Width,u16 Color); //�ߣ�StartXY,EndXY
			void DrawRectangle(u16 Posi[4],u8 Width,u16 Color); //����StartXY,EndXY
			void DrawFrame(u16 Posi[4],u16 ColorA,u16 ColorB); //��	
			void DrawCircle(u16 Posi[2],u8 Radius,u8 Width,u16 Color); //Բ
			void DrawArc(u16 Posi[2],u8 Radius,u16 StartAngle,u16 Angle,u8 Width,u16 Color); //��
			void DrawChar(u16 Posi[2],s8 Char,u8 Zoom,u16 Color); //ASCII�ַ�
			void DrawString(u16 Posi[2],char String[],u8 Zoom, u16 Color); //�ַ���
			u32 Abs(s32 Data);
	};	
	
	extern class GUI Window;	

#endif
	
