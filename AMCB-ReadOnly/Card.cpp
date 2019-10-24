#include "CARD.h"

CARD Card;

u8 inline SEL_CELL(u8 Select){ //ѡ��Ԫ
	if(!Select){
		GPIOA->BSRR |= 0x07; //���ѡ��
		for(u8 i=0;i<4;i++);
		GPIOA->BRR |= 0x07; //ѡ��
	}
	else{
		GPIOA->BRR |= 0x07; //���ѡ��	
		for(u8 i=0;i<4;i++);		
		GPIOA->BSRR |= (Select&0x07); //ѡ��
	}
	return Select&0x07; //����ѡ�е�Ԫ��
}
u8 inline GET_CELL_NUMBER(){ //��ȡ��Ԫ��
	GPIOA->BSRR |= 0x07; //ѡ����ߵ�Ԫ
	for(u8 i=0;i<18;i++); //�ȴ�һ��
	u8 Info = (GPIOA->IDR&0x07); //��ȡѡ����Ϣ
	for(u8 i=0;i<18;i++); //�ȴ�һ��
	GPIOA->BRR |= 0x07; //���ѡ��
	switch(Info){ 
		case 0x07: return 8; //��8����Ԫ
		case 0x03: return 4; //��4����Ԫ
		case 0x01: return 2; //��2����Ԫ
		default :return 0; //����
	}
}
void inline UNSEL_CELL(){ //���ѡ��
	GPIOA->BRR |= 0x07;
}

void CARD::Init(){
	//NSS-PA4 SCK-PA5 MISO-PA6 MOSI-PA7
	RCC->APB2ENR |= 0x01<<12;
	RCC->APB2ENR |= 0x01<<2;
	GPIOA->CRL &= 0x0000FFFF;
	GPIOA->CRL |= 0xB4B40000;	//��ģʽ
	SPI1->CR1 = 0x0000;
	SPI1->CR2 = 0x0000;
	SPI1->CR1 |= 0x01<<9; //������豸����
	SPI1->CR1 |= 0x01<<8; //Master
	SPI1->CR1 |= 0x03<<3; //BR = 72M / 2^(n+1)
	SPI1->CR1 |= 0x01<<2; //���豸
	SPI1->CR1 |= 0x01<<1; //Clk����ʱ���ָߵ�ƽ
	SPI1->CR1 |= 0x01<<0; //Clk�ڶ������ز���
  SPI1->CR1 |= 0x01<<6; //ʹ��SPI	
	
	GPIOA->CRL &= 0xFFFFF000;
	GPIOA->CRL |= 0x00000777;
	UNSEL_CELL();
}

u8 CARD::ReadWrite(u8 Data,bool Read){ //������д�ӿ�
	for(u32 Time=1000;Time;Time--) //�ȴ��շ�����
		if(SPI1->SR&0x02) break;
	SPI1->DR = Data; //��������
	
	for(u32 Time=1000;Time;Time--) //�ȴ��������
		if(SPI1->SR&0x01) break; 
	return SPI1->DR; //�����յ�������
}

bool CARD::WaitForReady(u8 Select){ //�ȴ���Ԫ׼����
	u8 Busy = 0x01; //æ��־
	for(u32 i=1000000;Busy&0x01 && i;i--){ //�������1000000��
		if(Select != SEL_CELL(Select)){ //ѡ��Ԫ
			//ѡ��ʧ��
			UNSEL_CELL(); //���ѡ�� 
			return 0; //�ȴ�ʧ��
		}
		
		ReadWrite(0x05,0); //�����ȡ��Ԫ״̬
		Busy = ReadWrite(0xFF,1); //��ȡ��Ԫ״̬
		
		UNSEL_CELL(); //���ѡ��
	}
	if(Busy&0x01) return 0; //�ȴ�ʧ��
	return 1; //�ȴ��ɹ�
}

u8 CARD::ReadCapacity(u8 Select){ //��ȡ��Ԫ����
	if(Select != SEL_CELL(Select)){ //ѡ��Ԫ
		//ѡ��ʧ��
		UNSEL_CELL(); //���ѡ��
		return 0; //����Ϊ0
	}
	
	u16 ID = 0;
	ReadWrite(0x90,0); //�����ȡ����
	ReadWrite(0x00,0);
	ReadWrite(0x00,0);
	ReadWrite(0x00,0);
	ID |= ReadWrite(0xFF,1)<<8;
	ID |= ReadWrite(0xFF,1);
	
	UNSEL_CELL();	//���ѡ��
	switch(ID&0xFF){ 
		case 0x13:return 1;
		case 0x14:return 2;
		case 0x15:return 4;
		case 0x16:return 8;
		case 0x17:return 16;
		default:return 0;
	}
}

void CARD::GetStatus(){ //��ȡ���濨״̬
	for(u8 i=0;i<8;i++){ //���ԭ״̬
		Status.StartSector[i] = 0;
		Status.SectorNumber[i] = 0;
		Status.Connected[i] = 0;
		Status.Capacity[i] = 0;
	}
	
	u8 CellNumber = GET_CELL_NUMBER(); //��ȡ��Ԫ��
	
	for(u8 i=0;i<CellNumber;i++){ //��ȡÿ����Ԫ״̬
		WaitForReady(i); //�ȴ���Ԫ׼����
		Status.Capacity[i] = ReadCapacity(i); //��ȡ��Ԫ����
		if(Status.Capacity[i]){ //������
			Status.Connected[i] = 1; //���õ�Ԫ���߱�־
			Status.SectorNumber[i] = Status.Capacity[i]*256; //���㵥Ԫ������
			WriteDisable(i); //��ֹд��
		}
	}
	
	for(u8 i=1;i<8;i++){
		Status.StartSector[i] = Status.StartSector[i-1]+Status.SectorNumber[i]; //���㵥Ԫ��ʼ�߼�������
	}
}

void CARD::SectorErase(u8 Select,u32 Address){ //����һ������
	WriteEnable(Select);	//����д��
	
	WaitForReady(Select); //�ȴ���Ԫ׼����
	
	if(Select != SEL_CELL(Select)){ //ѡ��Ԫ
		//ѡ��ʧ��
		UNSEL_CELL(); //���ѡ��
		return; //ֹͣ����
	}
	
	ReadWrite(0x20,0); //�������
	ReadWrite((Address>>16)&0xFF,0); //��ַ
	ReadWrite((Address>>8)&0xFF,0);
	ReadWrite(Address&0xFF,0);
	
	UNSEL_CELL(); //���ѡ��
	
	WaitForReady(Select); //�ȴ���Ԫ׼����
	
	WriteDisable(Select); //��ֹд��
}

bool CARD::ReadSector(u32 Index,u8 *Data){ //��ȡһ������
	GetStatus(); //��ȡ�洢��״̬
	u8 CardIndex = 0; //��Ԫ����
	u32 Address = 0; //��Ԫ�ڵ�ַ����
	if(!GetAddress(Index,CardIndex,Address)) //��ȡ�����ַ
		return 0;
	
	WaitForReady(CardIndex); //�ȴ���Ԫ׼����
	
	if(CardIndex != SEL_CELL(CardIndex)){ //ѡ��Ԫ
		//ѡ��ʧ��
		UNSEL_CELL(); //���ѡ��
		return 0; //��ȡʧ��
	}
	
	ReadWrite(0x03,0); //�����ȡ
	ReadWrite((Address>>16)&0xFF,0);
	ReadWrite((Address>>8)&0xFF,0);
	ReadWrite(Address&0xFF,0);
	for(u32 i=0;i<4096;i++) //��ȡ4096B
		Data[i] = ReadWrite(0xFF,1);
	
	UNSEL_CELL(); //���ѡ��
	
	WaitForReady(CardIndex); //�ȴ���Ԫ׼����
	return 1; //��ȡ�ɹ�
}

bool CARD::ReadData(u32 Address,u8 *Data,u32 Length){ //��ȡ������������
	GetStatus(); //��ȡ�洢��״̬ 
	u8 CardIndex = 0; //��Ԫ����
	u32 BaseAddress = 0; //��Ԫ�ڵ�ַ����
	if(!GetAddress(Address/4096,CardIndex,BaseAddress)) //��ȡ�����ַ
		return 0;
	
	WaitForReady(CardIndex); //�ȴ���Ԫ׼����
	
	if(CardIndex != SEL_CELL(CardIndex)){ //ѡ��Ԫ
		//ѡ��ʧ��
		UNSEL_CELL(); //���ѡ��
		return 0; //��ȡʧ��
	}
	
	Address = Address%4096 + BaseAddress; //���������ַ
	ReadWrite(0x03,0); //�����ȡ
	ReadWrite((Address>>16)&0xFF,0);
	ReadWrite((Address>>8)&0xFF,0);
	ReadWrite(Address&0xFF,0);
	for(u32 i=0;i<Length;i++) //��ȡ������������
		Data[i] = ReadWrite(0xFF,1);
	
	UNSEL_CELL(); //���ѡ��
	
	WaitForReady(CardIndex); //�ȴ���Ԫ׼����
	return 1; //��ȡ�ɹ�
}


bool CARD::WriteSector(u32 Index,u8 *Data){ //д��һ������
	GetStatus(); //��ȡ�洢��״̬
	u8 CardIndex = 0; //��Ԫ����
	u32 Address = 0; //��Ԫ�ڵ�ַ����
	if(!GetAddress(Index,CardIndex,Address)) //��ȡ�����ַ
		return 0;
	
	SectorErase(CardIndex,Address); //����һ������
	
	WriteEnable(CardIndex); //����д��
	
	u8 *Pdata = Data; //��д��������
	for(u32 page=0;page<16;page++){ //һ������/16ҳ
		WaitForReady(CardIndex); //�ȴ���Ԫ׼����
		if(CardIndex != SEL_CELL(CardIndex)){ //ѡ��Ԫ
			//ѡ��ʧ��
			UNSEL_CELL(); //���ѡ��
			return 0; //д��ʧ��
		}
		
		ReadWrite(0x02,0); //����д��һҳ
		ReadWrite((Address>>16)&0xFF,0);
		ReadWrite((Address>>8)&0xFF,0);
		ReadWrite(Address&0xFF,0);
		for(u32 i=0;i<256;i++) //д��256B
			 ReadWrite(*(Pdata++),0);
		
		UNSEL_CELL(); //���ѡ��
		WaitForReady(CardIndex); //�ȴ���Ԫ׼����
		Address += 256; //�޸ĵ�ַ
	}
	
	WriteDisable(CardIndex); //��ֹд��
	return 1; //д��ɹ�
}

bool CARD::GetAddress(u32 Index,u8 &Card,u32 &Address){ //��ȡ�����ַ
	for(u8 i=0;i<8;i++){ //��洢��״̬��
		if(Status.StartSector[i] <= Index) //�����ڸõ�Ԫ
			if(Status.StartSector[i]+Status.SectorNumber[i] > Index){ //ȷʵ�ڸõ�Ԫ
				Card = i; //���õ�Ԫ����
				Address = Index-Status.StartSector[i];
				Address *= 4096; //�����߼���ַ
				return 1; //ӳ��ɹ�
			}
	}
	return 0; //ӳ��ʧ��
}

void CARD::WriteEnable(u8 Select){ //����д��
	WaitForReady(Select); //�ȴ���Ԫ׼����
	
	if(Select != SEL_CELL(Select)){ //ѡ��Ԫ
		//ѡ��ʧ��
		UNSEL_CELL(); //���ѡ��
		return; //����
	}
	
	ReadWrite(0x06,0); //��������д��
	
	UNSEL_CELL();	//���ѡ��
	
	WaitForReady(Select); //�ȴ���Ԫ׼����
}

void CARD::WriteDisable(u8 Select){ //��ֹд��
	WaitForReady(Select); //�ȴ���Ԫ׼����
	
	if(Select != SEL_CELL(Select)){ //ѡ��Ԫ
		//ѡ��ʧ��
		UNSEL_CELL(); //���ѡ��
		return; //����
	}
	
	ReadWrite(0x04,0); //�����ֹд��
	
	UNSEL_CELL();	//���ѡ��

	WaitForReady(Select); //�ȴ���Ԫ׼����
}

u8 CARD::GetVolume(){ //��ȡ�洢������
	GetStatus(); //��ȡ�洢��״̬
	u8 Volume = 0; //����
	for(u8 i=0;i<8;i++) //�鿴ÿ����Ԫ����
		Volume += Status.Capacity[i]; //����������
	return Volume;
}
