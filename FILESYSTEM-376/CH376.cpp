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

extern"C"void TIM3_IRQHandler(void){ //超时计数器
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
	USART3->BRR = 36000000/250000; //波特率250K
	USART3->CR1 = 0x01<<13;//外设使能
	USART3->CR1 |= 0x01<<5; //接收中断
//	USART3->CR1 |= 0x01<<7; //发送中断
	NVIC->IP[39] = 0x00; //优先级0
	NVIC->ISER[1] |= 0x01<<7; //开放中断线
	USART3->CR1 |= 0x01<<2; //接收使能
	USART3->CR1 |= 0x01<<3; //发送使能
	
	RCC->APB1ENR |= 0x01<<1;
  TIM3->ARR = 0;//预装载
  TIM3->PSC = 719;//分频
  TIM3->CR1 |= 0x01<<4;
  TIM3->DIER |= 0x01<<0;
  NVIC->IP[29] = 0x10;//优先级1
  NVIC->ISER[0] |= 0x01<<29;
  TIM3->CR1 |= 0x01<<0;//使能
}

void SCI::TX(u8 data[],u16 length){
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = 0x57; //第一同步字
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = 0xAB; //第二同步字
	for(u32 i=0;i<length;i++){
		while(!(USART3->SR&(0x01<<7)));
		USART3->DR=data[i];
	}
}

void SCI::Refresh(){ //重置接收状态
	RxCounter = 0;
}

bool SCI::WaitReceive(u32 Number,u16 TimeOut){
	TimeOutFlag = 0; //清除超时标志
	TIM3->ARR = TimeOut; //开启超时计数器
	while(RxCounter < Number){ //等待接收到指定数量数据
		if(TimeOutFlag){ //超时
			TimeOutFlag = 0; //清除超时标志
			return 0; //接收失败
		}
	}
	return 1; //接收完成
}

u8 CH376::GetIcVer(){
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = GET_IC_VER; //写入命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	if(Sci.WaitReceive(1,50000)) //接收成功
		return Sci.RxBuffer[0]&0x3F;
	else //接收失败
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
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = ENTER_SLEEP; //写入 睡眠 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
}

void CH376::ResetAll(){
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = RESET_ALL; //写入 复位 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	Delay_Ms(500); //等待复位完成
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
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = GET_FILE_SIZE; //写入 获取文件大小 命令
	Sci.TxBuffer[1] = 0x68;
	Sci.TX(Sci.TxBuffer,2); //发送命令
	if(Sci.WaitReceive(4,50000)){ //接收完成
		FCT_32._u8[0] = Sci.RxBuffer[0]; //转码
		FCT_32._u8[1] = Sci.RxBuffer[1];
		FCT_32._u8[2] = Sci.RxBuffer[2];
		FCT_32._u8[3] = Sci.RxBuffer[3];
		return FCT_32._u32; //返回文件大小
	}
	else //接受失败
		return 0;
}

bool CH376::SetUSBMode(u8 mode){
	Sci.CommandTime = 1;
	Sci.Refresh();
	Sci.TxBuffer[0] = SET_USB_MODE;
	Sci.TxBuffer[1] = mode;
	Sci.TX(Sci.TxBuffer,2);
	if(Sci.WaitReceive(1,50000)){ //接收成功
		if(Sci.RxBuffer[0]==CMD_RET_SUCCESS){ //设置成功
			Sci.CommandTime = 0;
			return 1;
		}
		else{ //设置失败
			Sci.CommandTime = 0;
			return 0;
		}
	}
	else{ //接收失败
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
	
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = RD_USB_DATA0; //写入 USB数据读取 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	if(Sci.WaitReceive(1,50000)){ //数据长度接收成功
		u8 length = Sci.RxBuffer[0]+1; //获取长度
		if(Sci.WaitReceive(length,50000)){ //等待剩余数据接收完成
			length--; //调整长度
			for(int i=0;i<length;i++) //将数据写入缓存
				data[i] = Sci.RxBuffer[i+1];
			return length; //返回数据长度
		}
		else{ //剩余数据接收失败
			return 0;
		}
	}
	else{ //接收失败
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
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = SET_FILE_NAME; //写入 设置文件名 命令
	u32 length = 2+root; //计算长度
	u8 *ptr = Sci.TxBuffer+1;
	if(root) *(ptr++) = '/';
	for(int i=0;i<13&&name[i];i++,length++)
			*(ptr++) = name[i];
	*ptr = 0;
	Sci.TX(Sci.TxBuffer,length); //发送命令
}

bool CH376::DiskConnect(){
	Sci.CommandTime = 1;
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = DISK_CONNECT; //写入 检测磁盘是否连接 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	if(Sci.WaitReceive(1,50000)){ //接收成功
		if(Sci.RxBuffer[0]==USB_INT_SUCCESS){ //确认磁盘连接
			Sci.CommandTime = 0;
			return 1;
		}
		else{ //磁盘未连接
			Sci.CommandTime = 0;
			return 0;
		}
	}
	else{ //接受失败
		Sci.CommandTime = 0;
		return 0;
	}
}
bool CH376::DiskMount(INQUERY *query){
	Sci.CommandTime = 1;
	union{
		u8 _8[36];
		INQUERY _288;
	}FCT_IN; //数据格式转换器
	
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = DISK_MOUNT; //写入 初始化磁盘 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	if(Sci.WaitReceive(1,50000)){ //接收成功
		if(Sci.RxBuffer[0] != USB_INT_SUCCESS){ //获取失败
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //获取成功
			if(ReadUSBData(FCT_IN._8)){ //读取设备信息成功
				*query = FCT_IN._288;
				Sci.CommandTime = 0;
				return 1;
			}
			else{ //读取设备信息失败
				Sci.CommandTime = 0;
				return 0;
			}
		}
	}
	else{ //接收失败
		Sci.CommandTime = 0;
		return 0;
	}
}

u8 CH376::FileOpen(){
	Sci.CommandTime = 1;
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = FILE_OPEN; //写入 打开文件 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	if(Sci.WaitReceive(1,50000)){ //接收成功
		Sci.CommandTime = 0;
		return Sci.RxBuffer[0]; 
	}
	else{ //接收失败
		Sci.CommandTime = 0;
		return 0;
	}
}

u16 CH376::FileEnum(FATINFO info[],u8 name[],bool root){
	Sci.CommandTime = 1;
	union{
		FATINFO _256;
		u8 _8[32];
	}FCT_FAT; //数据类型转换器
	bool sign = 1; //通配标志
	u16 counter = 0; //文件计数器
	for(u32 i=0;i<13&&name[i];i++) //查找通配符
		if(name[i]=='*') sign = 0;
//	if(sign) return 0; //没有通配符，不进行枚举
	
	SetFileName(name,root); //设置文件名
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = FILE_OPEN; //写入 打开文件 命令
	Sci.TX(Sci.TxBuffer,1);	 //发送命令
	while(1){
		if(Sci.WaitReceive(1,50000)){ //接收到标识
			if(Sci.RxBuffer[0]!=USB_INT_DISK_READ){ //枚举完毕
				break;
			}
			else{ //枚举尚未完毕
				if(ReadUSBData(FCT_FAT._8)){ //接收文件信息成功
					info[counter++] = FCT_FAT._256; //转码并存储
					Sci.Refresh(); //重置接收状态
					Sci.TxBuffer[0] = FILE_ENUM_GO; //写入 继续枚举 命令
					Sci.TX(Sci.TxBuffer,1);	//发送命令
				}
				else{ //接收文件信息失败
					break;
				}
			}
		}
		else{ //接收标识失败，枚举中断
			break;
		}
	}
	Sci.CommandTime = 0;
	return counter; //返回文件数
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
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = FILE_CLOSE; //写入 关闭文件 命令
	Sci.TxBuffer[1] = resize; //写入是否更新文件长度
	Sci.TX(Sci.TxBuffer,1); //发送命令
	TIM3->PSC = 35999;
	if(Sci.WaitReceive(1,50000)){ //接收成功
		if(Sci.RxBuffer[0]==USB_INT_SUCCESS){ //关闭成功
			Sci.CommandTime = 0;
			TIM3->PSC = 719;
			return 1;
		}
		else{ //关闭失败
			Sci.CommandTime = 0;
			TIM3->PSC = 719;
			return 0;
		}
	}
	else{ //接收失败
		Sci.CommandTime = 0;
		TIM3->PSC = 719;
		return 0;
	}
}

/*bool CH376::DirInfoRead(FATINFO *info){
	union{
		FATINFO _256;
		u8 _8[32];
	}FCT_FAT; //数据格式转换器
	
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = DIR_INFO_READ; //写入 目录信息读取 命令
	Sci.TxBuffer[1] = 0xFF; //读取当前已打开的目录
	Sci.TX(Sci.TxBuffer,2); //发送命令
	
	if(Sci.WaitReceive(1,50000)){ //读取成功
		if(ReadUSBData(FCT_FAT._8)){ //读取目录信息成功
			*info = FCT_FAT._256;
			return 1;
		}
		else{ //读取目录信息失败
			return 0;
		}
	}
	else{ //读取失败
		return 0;
	}
}*/

u32 CH376::ByteLocate(u32 offset){
	Sci.CommandTime = 1;
	FCT_32._u32 = offset; //写入偏移量
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = BYTE_LOCATE; //写入 移动文件指针 命令
	Sci.TxBuffer[1] = FCT_32._u8[0]; //写入偏移量
	Sci.TxBuffer[2] = FCT_32._u8[1];
	Sci.TxBuffer[3] = FCT_32._u8[2];
	Sci.TxBuffer[4] = FCT_32._u8[3];	
	Sci.TX(Sci.TxBuffer,5); //发送命令
	if(Sci.WaitReceive(1,50000)){ //接收成功
		if(Sci.RxBuffer[0]!=USB_INT_SUCCESS){ //移动失败
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //移动成功
			if(ReadUSBData(FCT_32._u8)){ //获取绝对线性扇区号成功
				Sci.CommandTime = 0;
				return FCT_32._u32;
			}
			else{ //获取绝对线性扇区号失败
				Sci.CommandTime = 0;
				return 1;
			}
		}
	}
	else{ //接收失败
		Sci.CommandTime = 0;
		return 0;
	}
}

u16 CH376::ByteRead(u8 data[],u16 length){
	Sci.CommandTime = 1;
	FCT_16._u16 = length; //写入数据量
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = BYTE_READ; //写入 字节读取 命令
	Sci.TxBuffer[1] = FCT_16._u8[0]; //写入数据量
	Sci.TxBuffer[2] = FCT_16._u8[1];	
	Sci.TX(Sci.TxBuffer,3); //发送命令
	u16 counter = 0; //数据量计数器
	while(1){	
//		while(!Sci.RxCounter);
//		GPIOC->BRR |= 0x01<<13;
//		while(1);
		if(Sci.WaitReceive(1,50000)){ //接收到标志
			if(Sci.RxBuffer[0]!=USB_INT_DISK_READ){ //读取完毕
				break;
			}
			else{ //读取尚未完毕
				counter += ReadUSBData(data+counter); //读取数据并更改数据量计数器
				Sci.Refresh(); //重置接收状态
				Sci.TxBuffer[0] = BYTE_RD_GO;	//写入 继续字节读取 命令
				Sci.TX(Sci.TxBuffer,1); //发送命令
			}
		}
		else{ //接收标志失败
			break;
		}
	}
	Sci.CommandTime = 0;
	return counter; //返回数据量
}

u32 CH376::DiskCapacity(){
	Sci.CommandTime = 1;
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = DISK_CAPACITY; //写入 查询磁盘容量 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	if(Sci.WaitReceive(1,50000)){ //接收成功
		if(Sci.RxBuffer[0]!=USB_INT_SUCCESS){ //查询失败
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //查询成功
			if(ReadUSBData(FCT_32._u8)){ //读取容量成功
				Sci.CommandTime = 0;
				return FCT_32._u32; //返回容量
			}
			else{
				Sci.CommandTime = 0;
				return 0;
			}
		}
	}
	else{ //接收失败
		Sci.CommandTime = 0;
		return 0;
	}
}

/*bool CH376::DiskQuery(DISKQUERY *query){
	Sci.CommandTime = 1;
	union{
		DISKQUERY _288;
		u8 _8[36];
	}FCT_DQ; //数据格式转换器
	Sci.Refresh(); //重置接收状态
	Sci.TxBuffer[0] = DISK_QUERY; //写入 查询磁盘空间信息 命令
	Sci.TX(Sci.TxBuffer,1); //发送命令
	if(Sci.WaitReceive(1,50000)){ //接收成功
		if(Sci.RxBuffer[0]!=USB_INT_SUCCESS){ //查询失败
			Sci.CommandTime = 0;
			return 0;
		}
		else{ //查询成功
			if(ReadUSBData(FCT_DQ._8)){ //读取信息成功
				*query = FCT_DQ._288;
				Sci.CommandTime = 0;
				return 1;
			}
			else{ //读取信息失败
				Sci.CommandTime = 0;
				return 0;
			}
		}
	}
	else{ //接收失败
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
	TestIfConnectFlag = 0; //清除挂载检测请求标志

	if(Ch376.DiskConnect()){ //检测到挂载
		if(ConnectFlag) return; //早已挂载，不必处理
		Delay_Ms(50); //延迟消抖
		if(Ch376.DiskConnect()){ //仍然能检查到挂载
			Ch376.SetUSBMode(0x07); //复位USB总线
			Delay_Ms(500);
			Ch376.SetUSBMode(0x06); //切换到自动产生SOF模式
			Delay_Ms(500);
			
			for(u32 i=0;i<5;i++){ //获取磁盘信息，最多重复5次
				if(Ch376.DiskMount(&DiskInfoS)){ //获取信息成功
					DiskInfoF = Ch376.DiskCapacity(); //获取磁盘总物理容量
					ConnectFlag = 1; //设置挂载标志
					Interface.MountNUnmount(); //根据连接状况决定是否挂载
					return;
				}
				else{ //获取信息失败
					ConnectFlag = 0;
				}
			}
		}
	}
	else{ //未检测到挂载
		Ch376.SetUSBMode(0x05); //切换到不产生SOF模式
		ConnectFlag = 0;
	}
}


