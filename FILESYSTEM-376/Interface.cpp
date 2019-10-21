#include "Interface.h"

INTERFACE Interface;

void INTERFACE::Init(){
	Udisk.Init();
}

bool INTERFACE::MountNUnmount(){
	if(!Udisk.ConnectFlag){ //磁盘未连接，解除挂载
		Flat.FileCounter = 0;
		return 0;
	}
	else{ //磁盘已连接，可以挂载
		Flat.FileCounter = Ch376.FileEnum(Flat.File,(u8*)"*.*",1);
		for(u32 i=0;i<Flat.FileCounter;i++){ //清除目录
			Flat.FilePointer[i] = 0; //重置文件指针
			Flat.Opened[i] = 0;
			if(Flat.File[i].DIR_Attr != 0x20){ //不是归档文件
				for(u32 k=i;k<Flat.FileCounter-1;k++)
					Flat.File[k] = Flat.File[k+1]; //整体向前移动一位
				Flat.FileCounter--; //文件计数器减一
			}
		}
		return 1;
	}
}


int INTERFACE::ReadDataWithVerify(u8 Index,u16 Length){
	u8 TempBuffer[1024]; //临时缓存
	bool VerifyDone = 0; //校验完毕标志
	
	if(Index > Flat.FileCounter) return 0; //索引错误

	if(!Flat.Opened[Index]){ //目标文件尚未打开
		if(TestIfOpened()){ //已打开了其他文件
			CloseAll(); //关闭所有文件
		}
		OpenFile(Index); //打开目标文件
	}
	
	for(u32 C=0;C<5;C++){ //最多重复尝试5次
		Ch376.ByteLocate(Flat.FilePointer[Index]); //设置文件指针
		DataCounter = Ch376.ByteRead(TempBuffer,Length); //第一次读取文件
		
		Ch376.ByteLocate(Flat.FilePointer[Index]); //重新设置文件指针
		DataCounter = Ch376.ByteRead(Buffer,Length); //第二次读取文件
		
		VerifyDone = 1; //设置校验完毕标志
		for(u32 i=0;i<DataCounter;i++){ //逐字节校验
			if(Buffer[i] != TempBuffer[i]){ //校验失败
				VerifyDone = 0; //清除校验成功标志
				break; //停止进一步校验
			}
		}
		if(VerifyDone) break; //校验成功停止尝试
	}
	
	if(VerifyDone){ //校验成功
		Flat.FilePointer[Index] += DataCounter; //更新文件指针
		Ch376.ByteLocate(0x00); 
		
		if(DataCounter) return 1; //有数据
		else return -1; //没有数据
	}
	else{ //校验失败
		DataCounter = 0; //重置数据量
		return 0;
	}
}


bool INTERFACE::ReadData(u8 Index,u16 Length){
	if(Index > Flat.FileCounter) return 0;
	if(!Flat.Opened[Index]){
		if(TestIfOpened()){
			CloseAll();
		}
		OpenFile(Index);
	}
	Ch376.ByteLocate(Flat.FilePointer[Index]);
	DataCounter = Ch376.ByteRead(Buffer,Length);
	Flat.FilePointer[Index] += DataCounter;
	Ch376.ByteLocate(0x00);

	if(DataCounter)
		return 1;
	else
		return 0;
	
}

bool INTERFACE::OpenFile(u8 Index){
	if(Index >= Flat.FileCounter) return 0;
	if(Flat.File[Index].DIR_Attr == 0x10){ //是目录
		return 0;
	}
	else{
		u8 TempName[13] = {};
		GenerateFileName(Flat.File[Index].DIR_Name,TempName);
		Ch376.SetFileName(TempName,0);
		if(Ch376.FileOpen() == 0x14){
			Flat.Opened[Index] = 1;
			return 1;
		}
		else{
			return 0;
		}
	}
}

bool INTERFACE::TestIfOpened(){
	for(u32 i=0;i<64;i++)
		if(Flat.Opened[i]) return 1;
	return 0;
}

void INTERFACE::CloseAll(){
	for(u32 i=0;i<DataCounter;i++){
		if(Flat.Opened[i]){
//			u8 TempName[13] = {};
//			GenerateFileName(Flat.File[i].DIR_Name,TempName);
//			Ch376.SetFileName(TempName,0);
//			Ch376.FileClose(0);
		  Flat.Opened[i] = 0;
		}
	}
	ReFreash();
}


void INTERFACE::ReFreash(){
	Ch376.ResetAll();
	Ch376.SetUSBMode(0x07); //复位USB总线
	Delay_Ms(50);
	Ch376.SetUSBMode(0x06); //切换到自动产生SOF模式
	Delay_Ms(50);
	Ch376.DiskMount(&Udisk.DiskInfoS);
}

void INTERFACE::GenerateFileName(u8 *Sour,u8 *Dist){
	u8 *TempPtr = Dist;
	for(u32 i=0;i<8;i++){ //提取文件名
		if(Sour[i]==' ' || !Sour) break;
		*(TempPtr++) = Sour[i];
	}
	*(TempPtr++) = '.';
	for(u32 i=0;i<3;i++){ //提取扩展名
		if(Sour[i+8]==' ' || !Sour) break;
		*(TempPtr++) = Sour[i+8];
	}
	if(*(TempPtr-1) == '.') //没有扩展名
		*(TempPtr-1) = 0;
	else //有扩展名
		*(TempPtr) = 0;
}
