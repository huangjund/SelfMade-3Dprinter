#include "stm32f10x.h"
#include "CH376.h"

class INTERFACE{
	public:
		void Init(); //初始化
		bool MountNUnmount(); //挂载&解除挂载

		int ReadDataWithVerify(u8 Index,u16 Length); //读取文件数据并校验
	
		bool ReadData(u8 Index,u16 Length); //读取文件数据
	
		u8 Buffer[1024];
		u16 DataCounter;
	private:
		
		struct{
			FATINFO File[64]; //文件列表
			u32 FilePointer[64]; //文件指针
			bool Opened[64];
			u32 FileCounter; //列表中文件数
		}Flat;
		
		
		bool OpenFile(u8 Index); //打开目录或文件		
		
		bool TestIfOpened(); //检测是否有文件打开
		
		void CloseAll(); //关闭所有已打开文件
		
		void ReFreash(); //硬件刷新
		
		void GenerateFileName(u8 *Sour,u8 *Dist); //由8.3型生成n.m型文件名
};


extern class INTERFACE Interface;


