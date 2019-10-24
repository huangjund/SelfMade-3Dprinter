#include "CARD.h"

CARD Card;

u8 inline SEL_CELL(u8 Select){ //选择单元
	if(!Select){
		GPIOA->BSRR |= 0x07; //清除选择
		for(u8 i=0;i<4;i++);
		GPIOA->BRR |= 0x07; //选中
	}
	else{
		GPIOA->BRR |= 0x07; //清除选择	
		for(u8 i=0;i<4;i++);		
		GPIOA->BSRR |= (Select&0x07); //选中
	}
	return Select&0x07; //返回选中单元号
}
u8 inline GET_CELL_NUMBER(){ //获取单元数
	GPIOA->BSRR |= 0x07; //选中最高单元
	for(u8 i=0;i<18;i++); //等待一会
	u8 Info = (GPIOA->IDR&0x07); //获取选择信息
	for(u8 i=0;i<18;i++); //等待一会
	GPIOA->BRR |= 0x07; //清除选择
	switch(Info){ 
		case 0x07: return 8; //有8个单元
		case 0x03: return 4; //有4个单元
		case 0x01: return 2; //有2个单元
		default :return 0; //错误
	}
}
void inline UNSEL_CELL(){ //清除选择
	GPIOA->BRR |= 0x07;
}

void CARD::Init(){
	//NSS-PA4 SCK-PA5 MISO-PA6 MOSI-PA7
	RCC->APB2ENR |= 0x01<<12;
	RCC->APB2ENR |= 0x01<<2;
	GPIOA->CRL &= 0x0000FFFF;
	GPIOA->CRL |= 0xB4B40000;	//主模式
	SPI1->CR1 = 0x0000;
	SPI1->CR2 = 0x0000;
	SPI1->CR1 |= 0x01<<9; //软件从设备管理
	SPI1->CR1 |= 0x01<<8; //Master
	SPI1->CR1 |= 0x03<<3; //BR = 72M / 2^(n+1)
	SPI1->CR1 |= 0x01<<2; //主设备
	SPI1->CR1 |= 0x01<<1; //Clk空闲时保持高电平
	SPI1->CR1 |= 0x01<<0; //Clk第二个边沿采样
  SPI1->CR1 |= 0x01<<6; //使能SPI	
	
	GPIOA->CRL &= 0xFFFFF000;
	GPIOA->CRL |= 0x00000777;
	UNSEL_CELL();
}

u8 CARD::ReadWrite(u8 Data,bool Read){ //基本读写接口
	for(u32 Time=1000;Time;Time--) //等待收发器好
		if(SPI1->SR&0x02) break;
	SPI1->DR = Data; //发送数据
	
	for(u32 Time=1000;Time;Time--) //等待接收完成
		if(SPI1->SR&0x01) break; 
	return SPI1->DR; //返回收到的数据
}

bool CARD::WaitForReady(u8 Select){ //等待单元准备好
	u8 Busy = 0x01; //忙标志
	for(u32 i=1000000;Busy&0x01 && i;i--){ //测试最多1000000次
		if(Select != SEL_CELL(Select)){ //选择单元
			//选择失败
			UNSEL_CELL(); //清除选择 
			return 0; //等待失败
		}
		
		ReadWrite(0x05,0); //请求获取单元状态
		Busy = ReadWrite(0xFF,1); //读取单元状态
		
		UNSEL_CELL(); //清除选择
	}
	if(Busy&0x01) return 0; //等待失败
	return 1; //等待成功
}

u8 CARD::ReadCapacity(u8 Select){ //获取单元容量
	if(Select != SEL_CELL(Select)){ //选择单元
		//选择失败
		UNSEL_CELL(); //清除选择
		return 0; //容量为0
	}
	
	u16 ID = 0;
	ReadWrite(0x90,0); //请求获取容量
	ReadWrite(0x00,0);
	ReadWrite(0x00,0);
	ReadWrite(0x00,0);
	ID |= ReadWrite(0xFF,1)<<8;
	ID |= ReadWrite(0xFF,1);
	
	UNSEL_CELL();	//清除选择
	switch(ID&0xFF){ 
		case 0x13:return 1;
		case 0x14:return 2;
		case 0x15:return 4;
		case 0x16:return 8;
		case 0x17:return 16;
		default:return 0;
	}
}

void CARD::GetStatus(){ //获取储存卡状态
	for(u8 i=0;i<8;i++){ //清除原状态
		Status.StartSector[i] = 0;
		Status.SectorNumber[i] = 0;
		Status.Connected[i] = 0;
		Status.Capacity[i] = 0;
	}
	
	u8 CellNumber = GET_CELL_NUMBER(); //获取单元数
	
	for(u8 i=0;i<CellNumber;i++){ //读取每个单元状态
		WaitForReady(i); //等待单元准备好
		Status.Capacity[i] = ReadCapacity(i); //获取单元容量
		if(Status.Capacity[i]){ //有容量
			Status.Connected[i] = 1; //设置单元在线标志
			Status.SectorNumber[i] = Status.Capacity[i]*256; //计算单元扇区数
			WriteDisable(i); //禁止写入
		}
	}
	
	for(u8 i=1;i<8;i++){
		Status.StartSector[i] = Status.StartSector[i-1]+Status.SectorNumber[i]; //计算单元起始逻辑扇区号
	}
}

void CARD::SectorErase(u8 Select,u32 Address){ //擦除一个扇区
	WriteEnable(Select);	//允许写入
	
	WaitForReady(Select); //等待单元准备好
	
	if(Select != SEL_CELL(Select)){ //选择单元
		//选择失败
		UNSEL_CELL(); //清除选择
		return; //停止擦除
	}
	
	ReadWrite(0x20,0); //请求擦除
	ReadWrite((Address>>16)&0xFF,0); //地址
	ReadWrite((Address>>8)&0xFF,0);
	ReadWrite(Address&0xFF,0);
	
	UNSEL_CELL(); //清除选择
	
	WaitForReady(Select); //等待单元准备好
	
	WriteDisable(Select); //禁止写入
}

bool CARD::ReadSector(u32 Index,u8 *Data){ //读取一个扇区
	GetStatus(); //获取存储卡状态
	u8 CardIndex = 0; //单元索引
	u32 Address = 0; //单元内地址索引
	if(!GetAddress(Index,CardIndex,Address)) //获取物理地址
		return 0;
	
	WaitForReady(CardIndex); //等待单元准备好
	
	if(CardIndex != SEL_CELL(CardIndex)){ //选择单元
		//选择失败
		UNSEL_CELL(); //清除选择
		return 0; //读取失败
	}
	
	ReadWrite(0x03,0); //请求读取
	ReadWrite((Address>>16)&0xFF,0);
	ReadWrite((Address>>8)&0xFF,0);
	ReadWrite(Address&0xFF,0);
	for(u32 i=0;i<4096;i++) //读取4096B
		Data[i] = ReadWrite(0xFF,1);
	
	UNSEL_CELL(); //清除选择
	
	WaitForReady(CardIndex); //等待单元准备好
	return 1; //读取成功
}

bool CARD::ReadData(u32 Address,u8 *Data,u32 Length){ //读取给定长度数据
	GetStatus(); //获取存储卡状态 
	u8 CardIndex = 0; //单元索引
	u32 BaseAddress = 0; //单元内地址索引
	if(!GetAddress(Address/4096,CardIndex,BaseAddress)) //获取物理地址
		return 0;
	
	WaitForReady(CardIndex); //等待单元准备好
	
	if(CardIndex != SEL_CELL(CardIndex)){ //选择单元
		//选择失败
		UNSEL_CELL(); //清除选择
		return 0; //读取失败
	}
	
	Address = Address%4096 + BaseAddress; //计算物理地址
	ReadWrite(0x03,0); //请求读取
	ReadWrite((Address>>16)&0xFF,0);
	ReadWrite((Address>>8)&0xFF,0);
	ReadWrite(Address&0xFF,0);
	for(u32 i=0;i<Length;i++) //读取给定长度数据
		Data[i] = ReadWrite(0xFF,1);
	
	UNSEL_CELL(); //清除选择
	
	WaitForReady(CardIndex); //等待单元准备好
	return 1; //读取成功
}


bool CARD::WriteSector(u32 Index,u8 *Data){ //写入一个扇区
	GetStatus(); //获取存储卡状态
	u8 CardIndex = 0; //单元索引
	u32 Address = 0; //单元内地址索引
	if(!GetAddress(Index,CardIndex,Address)) //获取物理地址
		return 0;
	
	SectorErase(CardIndex,Address); //擦除一个扇区
	
	WriteEnable(CardIndex); //允许写入
	
	u8 *Pdata = Data; //待写数据索引
	for(u32 page=0;page<16;page++){ //一个扇区/16页
		WaitForReady(CardIndex); //等待单元准备好
		if(CardIndex != SEL_CELL(CardIndex)){ //选择单元
			//选择失败
			UNSEL_CELL(); //清除选择
			return 0; //写入失败
		}
		
		ReadWrite(0x02,0); //请求写入一页
		ReadWrite((Address>>16)&0xFF,0);
		ReadWrite((Address>>8)&0xFF,0);
		ReadWrite(Address&0xFF,0);
		for(u32 i=0;i<256;i++) //写入256B
			 ReadWrite(*(Pdata++),0);
		
		UNSEL_CELL(); //清除选择
		WaitForReady(CardIndex); //等待单元准备好
		Address += 256; //修改地址
	}
	
	WriteDisable(CardIndex); //禁止写入
	return 1; //写入成功
}

bool CARD::GetAddress(u32 Index,u8 &Card,u32 &Address){ //获取物理地址
	for(u8 i=0;i<8;i++){ //查存储卡状态表
		if(Status.StartSector[i] <= Index) //可能在该单元
			if(Status.StartSector[i]+Status.SectorNumber[i] > Index){ //确实在该单元
				Card = i; //设置单元索引
				Address = Index-Status.StartSector[i];
				Address *= 4096; //设置逻辑地址
				return 1; //映射成功
			}
	}
	return 0; //映射失败
}

void CARD::WriteEnable(u8 Select){ //允许写入
	WaitForReady(Select); //等待单元准备好
	
	if(Select != SEL_CELL(Select)){ //选择单元
		//选择失败
		UNSEL_CELL(); //清除选择
		return; //返回
	}
	
	ReadWrite(0x06,0); //请求允许写入
	
	UNSEL_CELL();	//清除选择
	
	WaitForReady(Select); //等待单元准备好
}

void CARD::WriteDisable(u8 Select){ //禁止写入
	WaitForReady(Select); //等待单元准备好
	
	if(Select != SEL_CELL(Select)){ //选择单元
		//选择失败
		UNSEL_CELL(); //清除选择
		return; //返回
	}
	
	ReadWrite(0x04,0); //请求禁止写入
	
	UNSEL_CELL();	//清除选择

	WaitForReady(Select); //等待单元准备好
}

u8 CARD::GetVolume(){ //获取存储卡容量
	GetStatus(); //获取存储卡状态
	u8 Volume = 0; //容量
	for(u8 i=0;i<8;i++) //查看每个单元容量
		Volume += Status.Capacity[i]; //计算总容量
	return Volume;
}
