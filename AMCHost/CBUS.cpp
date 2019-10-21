#include "CBUS.h"

CBUS Cbus;

extern"C"void USART3_IRQHandler(){
	if(USART3->SR & 0x01) //校验错误
		Cbus.VerifyError = 1;
	if(USART3->SR & 0x01<<5){ //起始：受到数据
		u8 Temp = USART3->DR;//8位数据值
		if(Temp == '%'){ //起始
			Cbus.TempBuffer[0] = Temp;
			Cbus.DataCounter = 1;
		}
		else if(Temp != '^'){ //数据
			if(Cbus.DataCounter < 128) //缓存未溢出
				Cbus.TempBuffer[Cbus.DataCounter++] = Temp; //记录数据
		}
		else if(Temp == '^'){ //结束符
			Cbus.RxFinish = 1; //设置结束标志
			for(u32 i=0;i<128;i++)
				Cbus.Buffer[i] = Cbus.TempBuffer[i];
			Cbus.DataNumber = Cbus.DataCounter;
		}
	}
}

void CBUS::Init(){
	//RX-PB11 TX-PB10
	RCC->APB2ENR |= 0x01<<3;//IO端口B时钟使能
	RCC->APB1ENR |= 0x01<<18;//USART3时钟使能
	GPIOB->CRH &= 0xFFFF00FF;//端口配置高寄存器（10 11）：置零
	GPIOB->CRH |= 0x00004F00;//MODE11：输入模式 CNF11：浮空输入 ； MODE10： 输出模式，最大速度50MHz CNF10：复用功能开漏输出
	
  USART3->CR1 = 0x00;
  USART3->CR2 = 0x00;
  USART3->CR3 = 0x00;//USART3控制寄存器清零
  USART3->GTPR = 0x00;//保护时间和预分频寄存器清零
	
  USART3->BRR = 36000000/375000; //波特率375K（没懂）
  USART3->CR1 |= 0x01<<13;//USART模块使能（软件设置0 1 ）
  USART3->CR1 |= 0x01<<5; //接受缓冲区非空中断使能：当SR中的ORE或者RXNE为“1”时产生USART中断
//  USART3->CR1 |= 0x01<<7; //发送缓冲区空中断使能
	USART3->CR1 |= 0x01<<12; //1个起始位，9个数据位吗，n个停止位
	USART3->CR1 |= 0x01<<9; //奇偶校验选择：奇校验
	USART3->CR1 |= 0x01<<8; //开启校验中断
	USART3->CR1 |= 0x01<<10; //校验控制使能：开启校验
  NVIC->IP[39] = 0x00; //优先级0
  NVIC->ISER[1] |= 0x01<<7; //开放中断线
  USART3->CR1 |= 0x01<<2; //接收使能（软件设置或清除
  USART3->CR1 |= 0x01<<3; //发送使能（软件设置或清除
}


bool CBUS::Transmit(char Address,char Data[],u32 TimeOut){
	RxFinish = 0;
	Tx('%'); //起始符
	Tx((u8)Address); //地址发送三次
	Tx((u8)Address); 
	Tx((u8)Address); 
	Print(Data);
	return WaitForReceive(TimeOut); //等待回复
}

bool CBUS::TestReply(char Sym){ //检测回执
	RxFinish = 0;
	VerifyError = 0;
	
	u32 C = 0;
	if(Buffer[1] == Sym) C++;
	if(Buffer[2] == Sym) C++;
	if(Buffer[3] == Sym) C++;
	
	if(C >= 2) return 1;
	return 0;
}

bool CBUS::TestIfVerifyError(){
	bool TempStatus = VerifyError;
	VerifyError = 0;
	return TempStatus;
}

bool CBUS::WaitForReceive(u32 Ms){//没懂
	while(Ms--){
		SysTick->LOAD=72000;
		SysTick->CTRL=0x00000005;//时钟源选择：处理器时钟AHB Systick使能
		while(!(SysTick->CTRL&0x00010000))//当上次读取本寄存器后，Systick已经计数到0
			if(RxFinish) break;
		SysTick->CTRL=0x00000004;	//取消Systick的使能
		if(RxFinish) break;
	}
	if(RxFinish) return 1;
	else return 0;
}


void CBUS::Print(char *data){
	for(u8 i=0;i<128;i++){ //最多不超过128字符
		if(data[i]=='^' || !data[i]){ //是结束符
			Tx('^'); //发送结束符
			return; //停止发送
		}
		else //非结束符
			Tx((u8)data[i]); //发送一个字符
	}
	Tx('^'); //补充发送结束符
}

void CBUS::Tx(u8 data){
	while(!(USART3->SR&(0x01<<7)));//若数据没有从TDR寄存器中转移到移位寄存器，则循环等待
	USART3->DR = data;//数据已转移至移位寄存器
}
