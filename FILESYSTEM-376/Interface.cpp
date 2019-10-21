#include "Interface.h"

INTERFACE Interface;

void INTERFACE::Init(){
	Udisk.Init();
}

bool INTERFACE::MountNUnmount(){
	if(!Udisk.ConnectFlag){ //����δ���ӣ��������
		Flat.FileCounter = 0;
		return 0;
	}
	else{ //���������ӣ����Թ���
		Flat.FileCounter = Ch376.FileEnum(Flat.File,(u8*)"*.*",1);
		for(u32 i=0;i<Flat.FileCounter;i++){ //���Ŀ¼
			Flat.FilePointer[i] = 0; //�����ļ�ָ��
			Flat.Opened[i] = 0;
			if(Flat.File[i].DIR_Attr != 0x20){ //���ǹ鵵�ļ�
				for(u32 k=i;k<Flat.FileCounter-1;k++)
					Flat.File[k] = Flat.File[k+1]; //������ǰ�ƶ�һλ
				Flat.FileCounter--; //�ļ���������һ
			}
		}
		return 1;
	}
}


int INTERFACE::ReadDataWithVerify(u8 Index,u16 Length){
	u8 TempBuffer[1024]; //��ʱ����
	bool VerifyDone = 0; //У����ϱ�־
	
	if(Index > Flat.FileCounter) return 0; //��������

	if(!Flat.Opened[Index]){ //Ŀ���ļ���δ��
		if(TestIfOpened()){ //�Ѵ��������ļ�
			CloseAll(); //�ر������ļ�
		}
		OpenFile(Index); //��Ŀ���ļ�
	}
	
	for(u32 C=0;C<5;C++){ //����ظ�����5��
		Ch376.ByteLocate(Flat.FilePointer[Index]); //�����ļ�ָ��
		DataCounter = Ch376.ByteRead(TempBuffer,Length); //��һ�ζ�ȡ�ļ�
		
		Ch376.ByteLocate(Flat.FilePointer[Index]); //���������ļ�ָ��
		DataCounter = Ch376.ByteRead(Buffer,Length); //�ڶ��ζ�ȡ�ļ�
		
		VerifyDone = 1; //����У����ϱ�־
		for(u32 i=0;i<DataCounter;i++){ //���ֽ�У��
			if(Buffer[i] != TempBuffer[i]){ //У��ʧ��
				VerifyDone = 0; //���У��ɹ���־
				break; //ֹͣ��һ��У��
			}
		}
		if(VerifyDone) break; //У��ɹ�ֹͣ����
	}
	
	if(VerifyDone){ //У��ɹ�
		Flat.FilePointer[Index] += DataCounter; //�����ļ�ָ��
		Ch376.ByteLocate(0x00); 
		
		if(DataCounter) return 1; //������
		else return -1; //û������
	}
	else{ //У��ʧ��
		DataCounter = 0; //����������
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
	if(Flat.File[Index].DIR_Attr == 0x10){ //��Ŀ¼
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
	Ch376.SetUSBMode(0x07); //��λUSB����
	Delay_Ms(50);
	Ch376.SetUSBMode(0x06); //�л����Զ�����SOFģʽ
	Delay_Ms(50);
	Ch376.DiskMount(&Udisk.DiskInfoS);
}

void INTERFACE::GenerateFileName(u8 *Sour,u8 *Dist){
	u8 *TempPtr = Dist;
	for(u32 i=0;i<8;i++){ //��ȡ�ļ���
		if(Sour[i]==' ' || !Sour) break;
		*(TempPtr++) = Sour[i];
	}
	*(TempPtr++) = '.';
	for(u32 i=0;i<3;i++){ //��ȡ��չ��
		if(Sour[i+8]==' ' || !Sour) break;
		*(TempPtr++) = Sour[i+8];
	}
	if(*(TempPtr-1) == '.') //û����չ��
		*(TempPtr-1) = 0;
	else //����չ��
		*(TempPtr) = 0;
}
