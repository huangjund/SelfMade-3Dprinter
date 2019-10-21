#include "CH376.h"
#include "Interface.h"

SCI Sci;
CH376 Ch376;
UDISK Udisk;

union{
	u8 _u8[4];
	u32 _u32;
	int _s32;
}FCT_32;
union{
	u8 _u8[2];
	u16 _u16;
	s16 _s16;
}FCT_16;


void Delay_Us(u32 n){
	SysTick->LOAD=72*n;
	SysTick->CTRL=0x00000005;
	while(!(SysTick->CTRL&0x00010000));
	SysTick->CTRL=0x00000004;
}
void Delay_Ms(u32 n){
	while(n--)
		Delay_Us(1000);
}

extern"C"void USART3_IRQHandler(void){
	u8 Temp=USART3->DR;
	if(Sci.RxCounter<UART1_RX_SIZE && Sci.CommandTime)
		Sci.RxBuffer[Sci.RxCounter++] = Temp;
/*	else if(!Sci.CommandTime){
		if(Temp == USB_INT_CONNECT)
			Udisk.TestIfConnectFlag = 1;
		else if(Temp == USB_INT_DISCONNECT)
			Udisk.TestIfConnectFlag = 1;
	}*/
}

extern"C"void TIM3_IRQHandler(void){ //��ʱ������
	if(TIM3->SR&0x01){
		Sci.TimeOutFlag = 1;
		TIM3->ARR = 0;
		TIM3->SR&=~0x01;
	}
}


void SCI::Init(){
	//RX-PB11 TX-PB10
	RCC->APB2ENR |= 0x01<<3;
	RCC->APB1ENR |= 0x01<<18;
	GPIOB->CRH&=0xFFFF00FF;
	GPIOB->CRH|=0x00004B00;
	USART3->CR2 = 0x0000;
	USART3->CR3 = 0x0000;
	USART3->GTPR = 0x0000;
	USART3->BRR = 36000000/250000; //������250K
	USART3->CR1 = 0x01<<13;//����ʹ��
	USART3->CR1 |= 0x01<<5; //�����ж�
//	USART3->CR1 |= 0x01<<7; //�����ж�
	NVIC->IP[39] = 0x00; //���ȼ�0
	NVIC->ISER[1] |= 0x01<<7; //�����ж���
	USART3->CR1 |= 0x01<<2; //����ʹ��
	USART3->CR1 |= 0x01<<3; //����ʹ��
	
	RCC->APB1ENR |= 0x01<<1;
  TIM3->ARR = 0;//Ԥװ��
  TIM3->PSC = 719;//��Ƶ
  TIM3->CR1 |= 0x01<<4;
  TIM3->DIER |= 0x01<<0;
  NVIC->IP[29] = 0x10;//���ȼ�1
  NVIC->ISER[0] |= 0x01<<29;
  TIM3->CR1 |= 0x01<<0;//ʹ��
}

void SCI::TX(u8 data[],u16 length){
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = 0x57; //��һͬ����
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = 0xAB; //�ڶ�ͬ����
	for(u32 i=0;i<length;i++){
		while(!(USART3->SR&(0x01<<7)));
		USART3->DR=data[i];
	}
}

void SCI::Refresh(){ //���ý���״̬
	RxCounter = 0;
}

bool SCI::WaitReceive(u32 Number,u16 TimeOut){
	TimeOutFlag = 0; //�����ʱ��־
	TIM3->ARR = TimeOut; //������ʱ������
	while(RxCounter < Number){ //�ȴ����յ�ָ����������
		if(TimeOutFlag){ //��ʱ
			TimeOutFlag = 0; //�����ʱ��־
			return 0; //����ʧ��
		}
	}
	return 1; //�������
}

u8 CH376::GetIcVer(){
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = GET_IC_VER; //д������
	Sci.TX(Sci.TxBuffer,1); //��������
	if(Sci.WaitReceive(1,50000)) //���ճɹ�
		return Sci.RxBuffer[0]&0x3F;
	else //����ʧ��
		return 0xFF; 
}

/*
bool CH376::SetBaudRate(bool devide,u8 para){
	u32 baudrate;
	Sci.Refresh();
	Sci.TxBuffer[0] = SET_BAUDRATE;
	if(devide){
		baudrate = 750000/(256-para);
		Sci.TxBuffer[1] = 0x02;
	}
	else{
		baudrate = 6000000/(256-para);
		Sci.TxBuffer[1] = 0x03;
	}
	Sci.TxBuffer[2] = para;
	baudrate = 36000000/baudrate;
	Sci.TX(Sci.TxBuffer,2);
	USART3->BRR = baudrate;
	while(!Sci.RxCounter);
	if(Sci.RxBuffer[0]==CMD_RET_SUCCESS) return 1;
	else return 0;
}*/


void CH376::EnterSleep(){
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = ENTER_SLEEP; //д�� ˯�� ����
	Sci.TX(Sci.TxBuffer,1); //��������
}

void CH376::ResetAll(){
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = RESET_ALL; //д�� ��λ ����
	Sci.TX(Sci.TxBuffer,1); //��������
	Delay_Ms(500); //�ȴ���λ���
}

bool CH376::CheckExist(){
	Sci.Refresh();
	Sci.TxBuffer[0] = CHECK_EXIST;
	Sci.TxBuffer[1] = 0x12;
	Sci.TX(Sci.TxBuffer,2);
	while(!Sci.RxCounter);
	if(Sci.RxBuffer[0]==0xED) return 1;
	return 0;
}

/*void CH376::SetSd0Int(bool disable){
//	Sci.Refresh();
//	Sci.TxBuffer[0] = SET_SD0_INT;
//	Sci.TxBuffer[1] = 0x16;
//	if(disable) Sci.TxBuffer[2] = 0x10;
//	else Sci.TxBuffer[2] = 0x90;
//	Sci.TX(Sci.TxBuffer,3);
}*/

u32 CH376::GetFileSize(){
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = GET_FILE_SIZE; //д�� ��ȡ�ļ���С ����
	Sci.TxBuffer[1] = 0x68;
	Sci.TX(Sci.TxBuffer,2); //��������
	if(Sci.WaitReceive(4,50000)){ //�������
		FCT_32._u8[0] = Sci.RxBuffer[0]; //ת��
		FCT_32._u8[1] = Sci.RxBuffer[1];
		FCT_32._u8[2] = Sci.RxBuffer[2];
		FCT_32._u8[3] = Sci.RxBuffer[3];
		return FCT_32._u32; //�����ļ���С
	}
	else //����ʧ��
		return 0;
}

bool CH376::SetUSBMode(u8 mode){
	Sci.CommandTime = 1;
	Sci.Refresh();
	Sci.TxBuffer[0] = SET_USB_MODE;
	Sci.TxBuffer[1] = mode;
	Sci.TX(Sci.TxBuffer,2);
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		if(Sci.RxBuffer[0]==CMD_RET_SUCCESS){ //���óɹ�
			Sci.CommandTime = 0;
			return 1;
		}
		else{ //����ʧ��
			Sci.CommandTime = 0;
			return 0;
		}
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		return 0;
	}
}

/*u8 CH376::GetStatus(){
//	Sci.Refresh();
//	Sci.TxBuffer[0] = SET_USB_MODE;
//	Sci.TX(Sci.TxBuffer,1);
//	while(!Sci.RxCounter);
//	return Sci.RxBuffer[0];
}*/

u8 CH376::ReadUSBData(u8 data[]){
	
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = RD_USB_DATA0; //д�� USB���ݶ�ȡ ����
	Sci.TX(Sci.TxBuffer,1); //��������
	if(Sci.WaitReceive(1,50000)){ //���ݳ��Ƚ��ճɹ�
		u8 length = Sci.RxBuffer[0]+1; //��ȡ����
		if(Sci.WaitReceive(length,50000)){ //�ȴ�ʣ�����ݽ������
			length--; //��������
			for(int i=0;i<length;i++) //������д�뻺��
				data[i] = Sci.RxBuffer[i+1];
			return length; //�������ݳ���
		}
		else{ //ʣ�����ݽ���ʧ��
			return 0;
		}
	}
	else{ //����ʧ��
		return 0;
	}
}

/*bool CH376::WriteHostData(u8 data[],u8 length){
	if(length>64) return 0;
	Sci.Refresh();
	Sci.TxBuffer[0] = WR_HOST_DATA;
	Sci.TxBuffer[1] = length;
	for(int i=0;i<length;i++)
		Sci.TxBuffer[i+2] = data[i];
	Sci.TX(Sci.RxBuffer,length);
	return 1;
}*/

/*u8 CH376::WriteREQData(u8 data[]){
//	Sci.Refresh();
//	Sci.TxBuffer[0] = WR_REQ_DATA;
//	Sci.TX(Sci.TxBuffer,1);
//	while(!Sci.RxCounter);
//	u8 length = Sci.RxBuffer[0];
//	Sci.TX(data,length);
//	return length;
}*/

/*bool CH376::WriteOFSData(u8 offset,u8 length,u8 data[]){
//	if(offset+length>32) return 0;
//	Sci.Refresh();
//	Sci.TxBuffer[0] = WR_OFS_DATA;
//	Sci.TxBuffer[1] = offset;
//	Sci.TxBuffer[2] = length;
//	for(int i=0;i<length;i++)
//			Sci.TxBuffer[i+3] = data[i];
//	Sci.TX(Sci.TxBuffer,length+3);
}*/

void CH376::SetFileName(u8 name[],bool root){
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = SET_FILE_NAME; //д�� �����ļ��� ����
	u32 length = 2+root; //���㳤��
	u8 *ptr = Sci.TxBuffer+1;
	if(root) *(ptr++) = '/';
	for(int i=0;i<13&&name[i];i++,length++)
			*(ptr++) = name[i];
	*ptr = 0;
	Sci.TX(Sci.TxBuffer,length); //��������
}

bool CH376::DiskConnect(){
	Sci.CommandTime = 1;
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = DISK_CONNECT; //д�� �������Ƿ����� ����
	Sci.TX(Sci.TxBuffer,1); //��������
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		if(Sci.RxBuffer[0]==USB_INT_SUCCESS){ //ȷ�ϴ�������
			Sci.CommandTime = 0;
			return 1;
		}
		else{ //����δ����
			Sci.CommandTime = 0;
			return 0;
		}
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		return 0;
	}
}
bool CH376::DiskMount(INQUERY *query){
	Sci.CommandTime = 1;
	union{
		u8 _8[36];
		INQUERY _288;
	}FCT_IN; //���ݸ�ʽת����
	
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = DISK_MOUNT; //д�� ��ʼ������ ����
	Sci.TX(Sci.TxBuffer,1); //��������
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		if(Sci.RxBuffer[0] != USB_INT_SUCCESS){ //��ȡʧ��
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //��ȡ�ɹ�
			if(ReadUSBData(FCT_IN._8)){ //��ȡ�豸��Ϣ�ɹ�
				*query = FCT_IN._288;
				Sci.CommandTime = 0;
				return 1;
			}
			else{ //��ȡ�豸��Ϣʧ��
				Sci.CommandTime = 0;
				return 0;
			}
		}
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		return 0;
	}
}

u8 CH376::FileOpen(){
	Sci.CommandTime = 1;
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = FILE_OPEN; //д�� ���ļ� ����
	Sci.TX(Sci.TxBuffer,1); //��������
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		Sci.CommandTime = 0;
		return Sci.RxBuffer[0]; 
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		return 0;
	}
}

u16 CH376::FileEnum(FATINFO info[],u8 name[],bool root){
	Sci.CommandTime = 1;
	union{
		FATINFO _256;
		u8 _8[32];
	}FCT_FAT; //��������ת����
	bool sign = 1; //ͨ���־
	u16 counter = 0; //�ļ�������
	for(u32 i=0;i<13&&name[i];i++) //����ͨ���
		if(name[i]=='*') sign = 0;
//	if(sign) return 0; //û��ͨ�����������ö��
	
	SetFileName(name,root); //�����ļ���
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = FILE_OPEN; //д�� ���ļ� ����
	Sci.TX(Sci.TxBuffer,1);	 //��������
	while(1){
		if(Sci.WaitReceive(1,50000)){ //���յ���ʶ
			if(Sci.RxBuffer[0]!=USB_INT_DISK_READ){ //ö�����
				break;
			}
			else{ //ö����δ���
				if(ReadUSBData(FCT_FAT._8)){ //�����ļ���Ϣ�ɹ�
					info[counter++] = FCT_FAT._256; //ת�벢�洢
					Sci.Refresh(); //���ý���״̬
					Sci.TxBuffer[0] = FILE_ENUM_GO; //д�� ����ö�� ����
					Sci.TX(Sci.TxBuffer,1);	//��������
				}
				else{ //�����ļ���Ϣʧ��
					break;
				}
			}
		}
		else{ //���ձ�ʶʧ�ܣ�ö���ж�
			break;
		}
	}
	Sci.CommandTime = 0;
	return counter; //�����ļ���
}

/*bool CH376::FileCreate(u8 name[],bool root){
//	if(FileOpen()==ERR_MISS_FILE) return 0;
//	SetFileName(name,root);	
//	Sci.Refresh();
//	Sci.TxBuffer[0] = FILE_CREATE;
//	Sci.TX(Sci.TxBuffer,1);	
//	while(!Sci.RxCounter);
//	if(Sci.RxBuffer[0]!=USB_INT_SUCCESS) 
//		return 1;
//	return 0;
}*/

/*bool CH376::FileErase(u8 name[],bool root){
//	SetFileName(name,root);
//	Sci.Refresh();
//	Sci.TxBuffer[0] = FILE_ERASE;
//	Sci.TX(Sci.TxBuffer,1);	
//	while(!Sci.RxCounter);
//	if(Sci.RxBuffer[0]!=USB_INT_SUCCESS) 
//		return 1;
//	return 0;
}*/

bool CH376::FileClose(bool resize){
	Sci.CommandTime = 1;
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = FILE_CLOSE; //д�� �ر��ļ� ����
	Sci.TxBuffer[1] = resize; //д���Ƿ�����ļ�����
	Sci.TX(Sci.TxBuffer,1); //��������
	TIM3->PSC = 35999;
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		if(Sci.RxBuffer[0]==USB_INT_SUCCESS){ //�رճɹ�
			Sci.CommandTime = 0;
			TIM3->PSC = 719;
			return 1;
		}
		else{ //�ر�ʧ��
			Sci.CommandTime = 0;
			TIM3->PSC = 719;
			return 0;
		}
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		TIM3->PSC = 719;
		return 0;
	}
}

/*bool CH376::DirInfoRead(FATINFO *info){
	union{
		FATINFO _256;
		u8 _8[32];
	}FCT_FAT; //���ݸ�ʽת����
	
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = DIR_INFO_READ; //д�� Ŀ¼��Ϣ��ȡ ����
	Sci.TxBuffer[1] = 0xFF; //��ȡ��ǰ�Ѵ򿪵�Ŀ¼
	Sci.TX(Sci.TxBuffer,2); //��������
	
	if(Sci.WaitReceive(1,50000)){ //��ȡ�ɹ�
		if(ReadUSBData(FCT_FAT._8)){ //��ȡĿ¼��Ϣ�ɹ�
			*info = FCT_FAT._256;
			return 1;
		}
		else{ //��ȡĿ¼��Ϣʧ��
			return 0;
		}
	}
	else{ //��ȡʧ��
		return 0;
	}
}*/

u32 CH376::ByteLocate(u32 offset){
	Sci.CommandTime = 1;
	FCT_32._u32 = offset; //д��ƫ����
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = BYTE_LOCATE; //д�� �ƶ��ļ�ָ�� ����
	Sci.TxBuffer[1] = FCT_32._u8[0]; //д��ƫ����
	Sci.TxBuffer[2] = FCT_32._u8[1];
	Sci.TxBuffer[3] = FCT_32._u8[2];
	Sci.TxBuffer[4] = FCT_32._u8[3];	
	Sci.TX(Sci.TxBuffer,5); //��������
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		if(Sci.RxBuffer[0]!=USB_INT_SUCCESS){ //�ƶ�ʧ��
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //�ƶ��ɹ�
			if(ReadUSBData(FCT_32._u8)){ //��ȡ�������������ųɹ�
				Sci.CommandTime = 0;
				return FCT_32._u32;
			}
			else{ //��ȡ��������������ʧ��
				Sci.CommandTime = 0;
				return 1;
			}
		}
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		return 0;
	}
}

u16 CH376::ByteRead(u8 data[],u16 length){
	Sci.CommandTime = 1;
	FCT_16._u16 = length; //д��������
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = BYTE_READ; //д�� �ֽڶ�ȡ ����
	Sci.TxBuffer[1] = FCT_16._u8[0]; //д��������
	Sci.TxBuffer[2] = FCT_16._u8[1];	
	Sci.TX(Sci.TxBuffer,3); //��������
	u16 counter = 0; //������������
	while(1){	
//		while(!Sci.RxCounter);
//		GPIOC->BRR |= 0x01<<13;
//		while(1);
		if(Sci.WaitReceive(1,50000)){ //���յ���־
			if(Sci.RxBuffer[0]!=USB_INT_DISK_READ){ //��ȡ���
				break;
			}
			else{ //��ȡ��δ���
				counter += ReadUSBData(data+counter); //��ȡ���ݲ�����������������
				Sci.Refresh(); //���ý���״̬
				Sci.TxBuffer[0] = BYTE_RD_GO;	//д�� �����ֽڶ�ȡ ����
				Sci.TX(Sci.TxBuffer,1); //��������
			}
		}
		else{ //���ձ�־ʧ��
			break;
		}
	}
	Sci.CommandTime = 0;
	return counter; //����������
}

u32 CH376::DiskCapacity(){
	Sci.CommandTime = 1;
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = DISK_CAPACITY; //д�� ��ѯ�������� ����
	Sci.TX(Sci.TxBuffer,1); //��������
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		if(Sci.RxBuffer[0]!=USB_INT_SUCCESS){ //��ѯʧ��
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //��ѯ�ɹ�
			if(ReadUSBData(FCT_32._u8)){ //��ȡ�����ɹ�
				Sci.CommandTime = 0;
				return FCT_32._u32; //��������
			}
			else{
				Sci.CommandTime = 0;
				return 0;
			}
		}
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		return 0;
	}
}

/*bool CH376::DiskQuery(DISKQUERY *query){
	Sci.CommandTime = 1;
	union{
		DISKQUERY _288;
		u8 _8[36];
	}FCT_DQ; //���ݸ�ʽת����
	Sci.Refresh(); //���ý���״̬
	Sci.TxBuffer[0] = DISK_QUERY; //д�� ��ѯ���̿ռ���Ϣ ����
	Sci.TX(Sci.TxBuffer,1); //��������
	if(Sci.WaitReceive(1,50000)){ //���ճɹ�
		if(Sci.RxBuffer[0]!=USB_INT_SUCCESS){ //��ѯʧ��
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //��ѯ�ɹ�
			if(ReadUSBData(FCT_DQ._8)){ //��ȡ��Ϣ�ɹ�
				*query = FCT_DQ._288;
				Sci.CommandTime = 0;
				return 1;
			}
			else{ //��ȡ��Ϣʧ��
				Sci.CommandTime = 0;
				return 0;
			}
		}
	}
	else{ //����ʧ��
		Sci.CommandTime = 0;
		return 0;
	}
}*/

/*bool CH376::SecLocate(u32 offset){
	FCT_32._s32 = offset;
	Sci.Refresh();
	Sci.TxBuffer[0] = SEC_LOCATE;
	Sci.TxBuffer[1] = FCT_32._u8[0];
	Sci.TxBuffer[2] = FCT_32._u8[1];
	Sci.TxBuffer[3] = FCT_32._u8[2];
	Sci.TxBuffer[4] = FCT_32._u8[3];	
	Sci.TX(Sci.TxBuffer,5);
	while(!Sci.RxCounter);
	if(Sci.RxBuffer[0]!=USB_INT_SUCCESS) 
		return 0;
	ReadUSBData(FCT_32._u8);
	return FCT_32._u32;
}*/

/*u32 CH376::SecRead(u8 data[],u8 number){
	Sci.Refresh();
	Sci.TxBuffer[0] = SEC_LOCATE;
	Sci.TxBuffer[1] = number;
	Sci.TX(Sci.TxBuffer,2);
	while(!Sci.RxCounter);
	if(Sci.RxBuffer[0]!=USB_INT_SUCCESS) 
		return 0;
	ReadUSBData(data);
	FCT_32._u8[0] = data[0];
	FCT_32._u8[1] = data[1];
	FCT_32._u8[2] = data[2];
	FCT_32._u8[3] = data[3];
	return DiskRead(data,FCT_32._u32,data[4]);
}*/

/*u32 CH376::DiskRead(u8 data[],u32 address,u8 number){
	FCT_32._u32 = address;
	Sci.Refresh();
	Sci.TxBuffer[0] = DISK_READ;
	Sci.TxBuffer[1] = FCT_32._u8[0];
	Sci.TxBuffer[2] = FCT_32._u8[1];	
	Sci.TxBuffer[3] = FCT_32._u8[2];
	Sci.TxBuffer[4] = FCT_32._u8[3];	
	Sci.TxBuffer[5] = number;
	Sci.TX(Sci.TxBuffer,6);
	u8 counter = 0;
	while(1){	
		while(!Sci.RxCounter);
		if(Sci.RxBuffer[0]!=USB_INT_DISK_READ) break;
		counter += ReadUSBData(data+counter);
		Sci.Refresh();
		Sci.TxBuffer[0] = DISK_RD_GO;	
		Sci.TX(Sci.TxBuffer,1);
	}
	return counter;
}*/

bool UDISK::Init(){
	Sci.Init();
//	Ch376.ResetAll();
	for(u32 i=0;i<10;i++){
		if(Ch376.SetUSBMode(0x05)) break;
		if(i == 9) return 0;
	}	
	
	return 1;
}

void UDISK::TestIfConnect(){
	TestIfConnectFlag = 0; //������ؼ�������־

	if(Ch376.DiskConnect()){ //��⵽����
		if(ConnectFlag) return; //���ѹ��أ����ش���
		Delay_Ms(50); //�ӳ�����
		if(Ch376.DiskConnect()){ //��Ȼ�ܼ�鵽����
			Ch376.SetUSBMode(0x07); //��λUSB����
			Delay_Ms(500);
			Ch376.SetUSBMode(0x06); //�л����Զ�����SOFģʽ
			Delay_Ms(500);
			
			for(u32 i=0;i<5;i++){ //��ȡ������Ϣ������ظ�5��
				if(Ch376.DiskMount(&DiskInfoS)){ //��ȡ��Ϣ�ɹ�
					DiskInfoF = Ch376.DiskCapacity(); //��ȡ��������������
					ConnectFlag = 1; //���ù��ر�־
					Interface.MountNUnmount(); //��������״�������Ƿ����
					return;
				}
				else{ //��ȡ��Ϣʧ��
					ConnectFlag = 0;
				}
			}
		}
	}
	else{ //δ��⵽����
		Ch376.SetUSBMode(0x05); //�л���������SOFģʽ
		ConnectFlag = 0;
	}
}


