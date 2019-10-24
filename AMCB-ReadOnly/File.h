#include "stm32f10x.h"

/*
	Offset 00 : Used 1B
	Offset 01 : FileName 32B  #31*Chars and 1*Null
	Offset 33 : CommandsNumber 8B
	Offset 41 : FileSize 8B  #Not Include FileHead
	Offset 49 : EstimatedTime 8B
	Offset 57 : TotalDistance 8B
	Offset 65 : Reserved 8B
	Offset 73 : Reserved 8B
	Offset 81 : Reserved 8B
	Offset 89 : Reserved 8B
	Offset 97 : Reserved 8B
	Offset 105 : Reserved 8B
	Offset 113 : NotUsed 7B #"0000000"
	Offset 120 : FileHeadSum 8B
*/

#ifndef _FILE_H
	#define _FILE_H
	
	#define u64 long long
	#define MAX_FILE_NUMBER 8
	#define BANK_SIZE 2048 //[Sector]
	class FILE{
		public:
			void Init(); //初始化
			bool FileTest(); //测试文件是否还在
			bool FileOpen(); //打开文件
//			bool FileRefreash(); //刷新文件状态
//			bool FileCreate(u8 *Name); //创建文件
//			bool FileDelete(); //删除文件
			bool FileRead(u8 *Data,u8 Length); //读取文件内容
			bool FileReadLine(u8 *Data); //读取一行内容
			bool FileSeek(s32 Offset); //移动文件指针
		
//			bool SetFileName(u8 *Name); //设置文件名
//			bool SetFileCommandsNumber(u32 Number); //设置文件命令数
//			bool SetFileSize(u32 Size); //设置文件大小
		
			bool GetFileName(u8 *Name); //获取文件名
			u32 GetFileSize(); //获取文件大小
			u32 GetFileCommandsNumber(); //获取文件命令数
			u32 GetFileEstimatedTime(); //预计时间
			u32 GetFileTotalDistance(); //总行程
			u32 GetRestVolume(); //获取存储卡剩余容量

			bool SwitchReadSector(u32 InFileAddress); //调整读取的扇区		
			u8 ReadByte(u32 InFileAddress); //读取一个字节
			bool ReadData(u32 Address,u8 *Data,u32 Length); //读取给定长度数据
			bool ReadSector(u32 Index,u8 *Data); //读取一个扇区
			bool WriteSector(u32 Index,u8 *Data); //写入一个扇区
			
			u32 CharToNumber(u8 InLength,u8 *Data); //字符串转数字
			void NumberToChar(u8 OutLength,u32 Data,u8 *Str); //数字转字符串
		
			bool FileHeadVerify(u8 *Data); //检测文件记录正确性
			
			u8 Temp[4096]; //临时缓存
			
			struct{
				bool Used; //文件存在性
				u8 Name[32]; //文件名
				u32 CommandsNumber; //命令数量
				u32 FileSize; //文件大小
				u32 EstimatedTime; //预计时间
				u32 TotalDistance; //总行程
				u32 Reserved_1;
				u32 Reserved_2;
				u32 Reserved_3;
				u32 Reserved_4;
				u32 Reserved_5;
				u32 Reserved_6;
				struct{
					u8 Data[4096]; //读取缓冲区
					u32 SectorIndex; //逻辑扇区号
					u32 DataIndex; //EmptyIndex[0:4096]
				}Buffer; //
				u32 Pointer; //文件指针
			}FileInfo;
			
	};
	
	extern class FILE File;
	
#endif
	