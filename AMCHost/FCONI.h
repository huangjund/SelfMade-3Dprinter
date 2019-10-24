#include "stm32f10x.h"

class FCONI{
	public:
		char GetTotalVolume();
		char GetRestVolume();
	
		char GetFileName();
		char GetFileSize();
		char GetFileCommandNumber();
		char GetFileEstimateTime();
		char GetFileTotalDistance();
		char GetFileInfo1();
		char GetFileInfo2();
		char GetFileInfo3();
		char GetFileInfo4();
		char GetFileInfo5();
		char GetFileInfo6();
		char GetFilePointer();
	
		char FileOpen();
		char FileTest();
		char FileRead(u8 Length,u8 *Buffer);
		char FileReadLine(u8 Length,u8 *Buffer);
		char FilePointerSeek(bool Forward,u32 Offset);
	
		bool TestLink();
	private:
		u32 CharToNumber(u8 InLength,u8 *Data);
		u8* NumberToChar(u8 OutLength,u32 Data,u8 *Str);
};

extern class FCONI Fconi;
