#include "stm32f10x.h"
#define BACKGROUND 0x0000 //Black

class LCD{
	public:
		void Init(); //��ʼ��
		void Clear(); //����
		void DrawPoint(u16 PosiX,u16 PosiY,u8 Width,u16 Color);
		void SetPixelLine(u16 PosiX,u16 PosiY,u16 EndX,u16 Color); //������ˮƽ��
		void SetPixelUpright(u16 PosiX,u16 PosiY,u16 EndY,u16 Color); //����������	
		void DrawNBit(u16 PosiX,u16 PosiY,u32 Data,u8 Numble,u8 Zoom,u16 Color); //���N����������ֱ����
	private:
		void WriteReg(u8 Addr,u8 Value); //д�Ĵ���
		void LCDReset(); //��λ
		void SetAddress(u8 Addr); //���üĴ�����ַ
		void WriteValue(u8 Value); //д������
		void SetCursor(u16 PosiX,u16 PosiY); //���ù��
		void SetPixelColor(u16 PosiX,u16 PosiY,u16 Color); //����������ɫ
		void Delay_Us(u32 n);
    void Delay_Ms(u32 n);
};

extern class LCD LCDInterface;
