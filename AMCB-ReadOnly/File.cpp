#include "File.h"
#include "Card.h"

FILE File;

/*
	Offset 00 : Used 1B
	Offset 01 : FileName 32B  #31*Chars and 1*Null
	Offset 33 : CommandsNumber 8B
	Offset 41 : FileSize 10B  #Not Include FileHead
	Offset 51 : EstimatedTime 8B
	Offset 59 : TotalDistance 8B
	Offset 67 : Reserved 8B
	Offset 75 : Reserved 8B
	Offset 83 : Reserved 8B
	Offset 91 : Reserved 8B
	Offset 99 : Reserved 8B
	Offset 107 : Reserved 8B
	Offset 115 : NotUsed 5B #"0000000"
	Offset 120 : FileHeadSum 8B
*/

void FILE::Init(){ //≥ı ºªØ
	if(FileTest()){
		FileInfo.Used = 1;
		for(u8 i=0;i<32;i++)
			FileInfo.Name[i] = FileInfo.Buffer.Data[1+i];
		FileInfo.CommandsNumber = CharToNumber(8,FileInfo.Buffer.Data+33);
		FileInfo.FileSize = CharToNumber(10,FileInfo.Buffer.Data+41);
		FileInfo.EstimatedTime = CharToNumber(8,FileInfo.Buffer.Data+51);
		FileInfo.TotalDistance = CharToNumber(8,FileInfo.Buffer.Data+59);
		FileInfo.Reserved_1 = CharToNumber(8,FileInfo.Buffer.Data+67);
		FileInfo.Reserved_2 = CharToNumber(8,FileInfo.Buffer.Data+75);
		FileInfo.Reserved_3 = CharToNumber(8,FileInfo.Buffer.Data+83);
		FileInfo.Reserved_4 = CharToNumber(8,FileInfo.Buffer.Data+91);
		FileInfo.Reserved_5 = CharToNumber(8,FileInfo.Buffer.Data+99);
		FileInfo.Reserved_6 = CharToNumber(8,FileInfo.Buffer.Data+107);
		FileInfo.Buffer.DataIndex = 0;
		FileInfo.Buffer.SectorIndex = 0;
		FileInfo.Pointer = 0;
	}
}

bool FILE::FileTest(){ //
	for(u32 i=0;i<128;i++)
		Temp[i] = 0;
	ReadData(0,Temp,128);
	if(!FileHeadVerify(Temp)){
		FileInfo.Used = 0;
		for(u8 i=0;i<32;i++)
			FileInfo.Name[i] = 0;
		FileInfo.CommandsNumber = 0;
		FileInfo.FileSize = 0;
		FileInfo.EstimatedTime = 0;
		FileInfo.TotalDistance = 0;
		FileInfo.Reserved_1 = 0;
		FileInfo.Reserved_2 = 0;
		FileInfo.Reserved_3 = 0;
		FileInfo.Reserved_4 = 0;
		FileInfo.Reserved_5 = 0;
		FileInfo.Reserved_6 = 0;
		FileInfo.Buffer.DataIndex = 0;
		FileInfo.Buffer.SectorIndex = 0;
		FileInfo.Pointer = 0;
		return 0;
	}
	return 1;
}

bool FILE::FileOpen(){ //
	if(FileTest()){
		FileInfo.Used = 1;
		for(u8 i=0;i<32;i++)
			FileInfo.Name[i] = Temp[1+i];
		FileInfo.CommandsNumber = CharToNumber(8,Temp+33);
		FileInfo.FileSize = CharToNumber(10,Temp+41);
		FileInfo.EstimatedTime = CharToNumber(8,Temp+51);
		FileInfo.TotalDistance = CharToNumber(8,Temp+59);
		FileInfo.Reserved_1 = CharToNumber(8,Temp+67);
		FileInfo.Reserved_2 = CharToNumber(8,Temp+75);
		FileInfo.Reserved_3 = CharToNumber(8,Temp+83);
		FileInfo.Reserved_4 = CharToNumber(8,Temp+91);
		FileInfo.Reserved_5 = CharToNumber(8,Temp+99);
		FileInfo.Reserved_6 = CharToNumber(8,Temp+107);
		FileInfo.Buffer.DataIndex = 0;
		FileInfo.Buffer.SectorIndex = 0;
		FileInfo.Pointer = 0;
		return 1;
	}
	return 0;
}

/*bool FILE::FileRefreash(){ //
	if(FileTest()){
		for(u32 i=0;i<4096;i++)
			Temp[i] = 0;
		Temp[0] = 'D';
		FileInfo.Used = 1;
		for(u8 i=0;i<32;i++)
			Temp[1+i] = FileInfo.Name[i];
		NumberToChar(8,FileInfo.CommandsNumber,Temp+33);
		NumberToChar(10,FileInfo.FileSize,Temp+41);
		NumberToChar(8,FileInfo.EstimatedTime,Temp+51);
		NumberToChar(8,FileInfo.TotalDistance,Temp+59);
		NumberToChar(8,FileInfo.Reserved_1,Temp+67);
		NumberToChar(8,FileInfo.Reserved_2,Temp+75);
		NumberToChar(8,FileInfo.Reserved_3,Temp+83);
		NumberToChar(8,FileInfo.Reserved_4,Temp+91);
		NumberToChar(8,FileInfo.Reserved_5,Temp+99);
		NumberToChar(8,FileInfo.Reserved_6,Temp+107);
		NumberToChar(5,0,Temp+115);
		u32 CheckSum = 0;
		for(u8 i=0;i<120;i++)
			CheckSum += Temp[i];
		NumberToChar(8,CheckSum,Temp+120);
		WriteSector(0,Temp);
		return 1;
	}
	return 1;
}*/

/*bool FILE::FileCreate(u8 *Name){ //
	if(!Card.GetVolume())
		return 0;
	
	for(u32 i=0;i<4096;i++)
		Temp[i] = 0;
	Temp[0] = 'D';
	for(u8 i=0;i<31;i++)
		Temp[1+i] = Name[i];
	for(u8 i=33;i<120;i++)
		Temp[i] = '0';
	u32 CheckSum = 0;
	for(u8 i=0;i<120;i++)
		CheckSum += Temp[i];
	NumberToChar(8,CheckSum,Temp+120);
	WriteSector(0,Temp);
	FileOpen();
	return 1;
}*/

/*bool FILE::FileDelete(){ //
	if(!FileTest())
		return 0;
	
	FileInfo.Used = 0;
	for(u8 i=0;i<32;i++)
		FileInfo.Name[i] = 0;
	FileInfo.CommandsNumber = 0;
	FileInfo.FileSize = 0;
	FileInfo.EstimatedTime = 0;
	FileInfo.TotalDistance = 0;
	FileInfo.Reserved_1 = 0;
	FileInfo.Reserved_2 = 0;
	FileInfo.Reserved_3 = 0;
	FileInfo.Reserved_4 = 0;
	FileInfo.Reserved_5 = 0;
	FileInfo.Reserved_6 = 0;
	FileInfo.Buffer.DataIndex = 0;
	FileInfo.Buffer.SectorIndex = 0;
	FileInfo.Pointer = 0;
	for(u32 i=0;i<4096;i++)
		FileInfo.Buffer.Data[i] = 0;
	WriteSector(0,FileInfo.Buffer.Data);
	FileOpen();
	return 1;
}*/

bool FILE::FileRead(u8 *Data,u8 Length){ //
	if(!FileInfo.Used)
		return 0;
	
	for(u32 i=0;i<128;i++)
		Data[i] = 0;
	for(u8 i=0;i<128&&i<Length;i++){
		Data[i] = ReadByte(FileInfo.Pointer);
		
		if(Data[i]==0 || Data[i]==255)
			break;
	}
	
	return 1;
}

bool FILE::FileReadLine(u8 *Data){ //
	if(!FileInfo.Used)
		return 0;
	
	for(u32 i=0;i<128;i++)
		Data[i] = 0;
	for(u8 i=0;i<128;i++){
		Data[i] = ReadByte(FileInfo.Pointer);
		
		if(Data[i]==10 || Data[i]==13 || Data[i]==0 || Data[i]==255){
			Data[i] = 0;
			break;
		}
	}
	
	return 1;
}

bool FILE::FileSeek(s32 Offset){ //
	if(!FileInfo.Used)
		return 0;
	
	s32 TempPointer = FileInfo.Pointer;
	TempPointer += Offset;
	if(TempPointer < 0)
		TempPointer = 0;
	if(TempPointer > FileInfo.FileSize)
		TempPointer = FileInfo.FileSize;
	
	FileInfo.Pointer = TempPointer;
	return 1;
}

/*bool FILE::SetFileName(u8 *Name){ //
	if(!FileInfo.Used)
		return 0;
	
	for(u8 i=0;i<31;i++)
		FileInfo.Name[i] = Name[i];
	FileInfo.Name[31] = 0;
	return 1;
}*/

u32 FILE::GetFileSize(){ //
	if(!FileInfo.Used)
		return 0;
	
	return FileInfo.FileSize;
}

/*bool FILE::SetFileCommandsNumber(u32 Number){ //
	if(!FileInfo.Used)
		return 0;
	
	FileInfo.CommandsNumber = Number;
	return 1;
}*/

/*bool FILE::SetFileSize(u32 Size){ //
	if(!FileInfo.Used)
		return 0;
	
	FileInfo.FileSize = Size;
	return 1;
}*/

bool FILE::GetFileName(u8 *Name){ //
	if(!FileInfo.Used){
		for(u8 i=0;i<32;i++)
			Name[i] = 0;
		return 0;
	}
	
	for(u8 i=0;i<32;i++)
		Name[i] = FileInfo.Name[i];
	
	return 1;
}

u32 FILE::GetFileCommandsNumber(){ //
	if(!FileInfo.Used)
		return 0;
	
	return FileInfo.CommandsNumber;
	
}

u32 FILE::GetFileEstimatedTime(){ //
	if(!FileInfo.Used)
		return 0;
	
	return FileInfo.EstimatedTime;
}

u32 FILE::GetFileTotalDistance(){ //
	if(!FileInfo.Used)
		return 0;
	
	return FileInfo.TotalDistance;
}


u32 FILE::GetRestVolume(){ //
	u32 Volume = Card.GetVolume();
	Volume *= 1048576;
	Volume -= 4096;
	
	if(!FileInfo.Used)
		return Volume;	
	
	Volume -= FileInfo.FileSize;
	return Volume;
}

bool FILE::SwitchReadSector(u32 InFileAddress){ //
	u32 SectorForRead = InFileAddress/4096 + 1;
	
	if(SectorForRead >= Card.GetVolume()*256)
		return 0;
	
	if(FileInfo.Buffer.SectorIndex == SectorForRead){
		FileInfo.Pointer = InFileAddress;
		return 1;
	}
	
	if(!ReadSector(SectorForRead,FileInfo.Buffer.Data)) return 0;
	FileInfo.Buffer.SectorIndex = SectorForRead;
	FileInfo.Buffer.DataIndex = 4096;
	FileInfo.Pointer = InFileAddress;
	return 1;
}

u8 FILE::ReadByte(u32 InFileAddress){ //
	if(!FileInfo.Used)
		return 0;
	
	if(InFileAddress >= FileInfo.FileSize)
		return 0;
	
	if(!SwitchReadSector(InFileAddress)) 
		return 0;
	
	u8 Data = FileInfo.Buffer.Data[FileInfo.Pointer%4096];
	FileInfo.Pointer++;
	
	return Data;
}

bool FILE::ReadSector(u32 Index,u8 *Data){ //
	if(!Card.GetVolume())
		return 0;
	
	u8 TryCounter = 0;
	for(TryCounter=0;TryCounter<3;TryCounter++){
		if(!Card.ReadSector(Index,Temp)) continue;
		if(!Card.ReadSector(Index,Data)) continue;
		for(u32 i=0;i<4096;i++)
			if(Temp[i] != Data[i]) continue;
		break;
	}
	
	if(TryCounter >= 3) return 0;
	return 1;
}

bool FILE::ReadData(u32 Address,u8 *Data,u32 Length){ //
//	if(!Card.GetVolume())
//		return 0;
	
	u8 Temp[512];
	
	u8 TryCounter = 0;
	for(TryCounter=0;TryCounter<3;TryCounter++){
		if(!Card.ReadData(Address,Temp,Length)) continue;
		if(!Card.ReadData(Address,Data,Length)) continue;
		for(u32 i=0;i<Length;i++)
			if(Temp[i] != Data[i]) continue;
		break;
	}
	
	if(TryCounter >= 3) return 0;
	return 1;
}


bool FILE::WriteSector(u32 Index,u8 *Data){ //
	if(!Card.GetVolume())
		return 0;
	
	u8 Temp[4096];
	
	u8 TryCounter = 0;
	for(TryCounter=0;TryCounter<3;TryCounter++){
		if(!Card.WriteSector(Index,Data)) continue;
		if(!Card.ReadSector(Index,Temp)) continue;
		for(u32 i=0;i<4096;i++)
			if(Temp[i] != Data[i]) continue;
		break;
	}
	
	if(TryCounter >= 3) return 0;
	return 1;
}

u32 FILE::CharToNumber(u8 InLength,u8 *Data){ //
	if(InLength > 10) InLength = 10;
	
	const u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
	
	u64 Value = 0;
	for(u8 i=0;i<InLength;i++)
		Value += (Data[i]-'0')*Weight[InLength-i];
	
	return Value;
}

void FILE::NumberToChar(u8 OutLength,u32 Data,u8 *Str){ //
	const u64 Weight[] = {1,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};

	for(u8 i=0;i<OutLength;i++)
		Str[i] = Data%Weight[OutLength-i+1]/Weight[OutLength-i] + '0';

}

bool FILE::FileHeadVerify(u8 *Data){ //
	if(Data[0] != 'D')
		return 0;
	
	u32 Sum = 0;
	for(u8 i=0;i<120;i++)
		Sum += Data[i];
	
	u32 Check = CharToNumber(8,Data+120);
	
	if(Sum == Check)
		return 1;
	
	return 0;
}
