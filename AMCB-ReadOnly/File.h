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
			void Init(); //��ʼ��
			bool FileTest(); //�����ļ��Ƿ���
			bool FileOpen(); //���ļ�
//			bool FileRefreash(); //ˢ���ļ�״̬
//			bool FileCreate(u8 *Name); //�����ļ�
//			bool FileDelete(); //ɾ���ļ�
			bool FileRead(u8 *Data,u8 Length); //��ȡ�ļ�����
			bool FileReadLine(u8 *Data); //��ȡһ������
			bool FileSeek(s32 Offset); //�ƶ��ļ�ָ��
		
//			bool SetFileName(u8 *Name); //�����ļ���
//			bool SetFileCommandsNumber(u32 Number); //�����ļ�������
//			bool SetFileSize(u32 Size); //�����ļ���С
		
			bool GetFileName(u8 *Name); //��ȡ�ļ���
			u32 GetFileSize(); //��ȡ�ļ���С
			u32 GetFileCommandsNumber(); //��ȡ�ļ�������
			u32 GetFileEstimatedTime(); //Ԥ��ʱ��
			u32 GetFileTotalDistance(); //���г�
			u32 GetRestVolume(); //��ȡ�洢��ʣ������

			bool SwitchReadSector(u32 InFileAddress); //������ȡ������		
			u8 ReadByte(u32 InFileAddress); //��ȡһ���ֽ�
			bool ReadData(u32 Address,u8 *Data,u32 Length); //��ȡ������������
			bool ReadSector(u32 Index,u8 *Data); //��ȡһ������
			bool WriteSector(u32 Index,u8 *Data); //д��һ������
			
			u32 CharToNumber(u8 InLength,u8 *Data); //�ַ���ת����
			void NumberToChar(u8 OutLength,u32 Data,u8 *Str); //����ת�ַ���
		
			bool FileHeadVerify(u8 *Data); //����ļ���¼��ȷ��
			
			u8 Temp[4096]; //��ʱ����
			
			struct{
				bool Used; //�ļ�������
				u8 Name[32]; //�ļ���
				u32 CommandsNumber; //��������
				u32 FileSize; //�ļ���С
				u32 EstimatedTime; //Ԥ��ʱ��
				u32 TotalDistance; //���г�
				u32 Reserved_1;
				u32 Reserved_2;
				u32 Reserved_3;
				u32 Reserved_4;
				u32 Reserved_5;
				u32 Reserved_6;
				struct{
					u8 Data[4096]; //��ȡ������
					u32 SectorIndex; //�߼�������
					u32 DataIndex; //EmptyIndex[0:4096]
				}Buffer; //
				u32 Pointer; //�ļ�ָ��
			}FileInfo;
			
	};
	
	extern class FILE File;
	
#endif
	