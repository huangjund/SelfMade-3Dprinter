#include "COM.h"
#include "LCD.h"

COM Serial;

char STRING_DONE[] = "DDD";
char STRING_SKIP[] = "KKK";
char STRING_FULL[] = "FFF";
char STRING_NONE[] = "NNN";
char STRING_ERROR[] = "RRR";
char STRING_MOVE[] = "MMM";

extern"C"void USART3_IRQHandler(){
	if(USART3->SR & 0x01) //У�����
		Serial.VerifyError = 1;
	if(USART3->SR & 0x01<<5){ //��ʼ
		u8 Temp = USART3->DR;
		if(Temp == '%'){ //��ʼ
			Serial.Buffer[0] = Temp;
			Serial.DataCounter = 1;
		}
		else if(Temp != '^'){ //����
			if(Serial.DataCounter < 128) //����δ���
				Serial.Buffer[Serial.DataCounter++] = Temp; //��¼����
		}
		else if(Temp == '^'){ //������
			if(Serial.TestAddress()){ //ȷ�ϵ�ַ
				Serial.RxFinish = 1; //���ý�����־
				for(u32 i=0;i<128;i++)
					Serial.RxData[i] = Serial.Buffer[i];
				Serial.DataNumber = Serial.DataCounter;
			}
			else{ //��ַ����
				Serial.VerifyError = 0;
			}
		}
	}
}

void COM::Init(){
	
	//RX-PB11 TX-PB10
	RCC->APB2ENR |= 0x01<<3;
	RCC->APB1ENR |= 0x01<<18;
	GPIOB->CRH &= 0xFFFF00FF;
	GPIOB->CRH |= 0x00004F00;
  USART3->CR1 = 0x00;
  USART3->CR2 = 0x00;
  USART3->CR3 = 0x00;
  USART3->GTPR = 0x00;
  USART3->BRR = 36000000/375000; //������375K
  USART3->CR1 |= 0x01<<13;//����ʹ��
  USART3->CR1 |= 0x01<<5; //�����ж�
//  USART3->CR1 |= 0x01<<7; //�����ж�
	USART3->CR1 |= 0x01<<12; //9������λ
	USART3->CR1 |= 0x01<<9; //��У��
	USART3->CR1 |= 0x01<<8; //����У���ж�
	USART3->CR1 |= 0x01<<10; //����У��
  NVIC->IP[39] = 0x00; //���ȼ�0
  NVIC->ISER[1] |= 0x01<<7; //�����ж���
  USART3->CR1 |= 0x01<<2; //����ʹ��
  USART3->CR1 |= 0x01<<3; //����ʹ��
}

void COM::Decoder(){
	if(!RxFinish) return; //δ�������
	
//	if(!TestAddress()){ //��ַ����
//		Serial.RxFinish = 0;
//		Serial.VerifyError = 0;
//		return;
//	}
	
	if(VerifyError){ //У�������
		Print(STRING_ERROR);
		Serial.RxFinish = 0;
		Serial.VerifyError = 0;
		return;
	}
	Serial.RxFinish = 0; //���������ɱ�־
	if(RxData[4] == 'M'){ //�ƶ�ָ��
		if(!Status.TestIfHomed()){ //��δ��λ
			Print(STRING_SKIP);
		}
		else if(Planner.PtrLoader->Full){ //������
			Print(STRING_FULL);
		}
		else{
			
			if(!Status.TestIfEnable())
				Stepper.EnableStepper();
			
			double Posi[5];
			Posi[0] = C2D(RxData+6);
			Posi[1] = C2D(RxData+19);
			Posi[2] = C2D(RxData+32);
			Posi[3] = LC2D(RxData+45);
			Posi[4] = LC2D(RxData+60);
			float Vmax;
			Vmax = C2F(RxData+75);
		
//			for(u32 i=0;i<5;i++)
//				Temp[i] = Posi[i];
//			V = Vmax;
			
			
			//(������չΪ��������������������)
			if(Planner.Load(Posi,Vmax)) //װ�����
				Print(STRING_DONE);
			else //װ��ʧ��
				Print(STRING_FULL);
		}
	}
	else if(RxData[4] == 'E'){ //�ֶ��ƶ�ָ��
		if(Planner.L2RestAmount != L2_SIZE){ //L2����ǿղ����ƶ�
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.L1RestAmount != L1_SIZE){ //L1����ǿղ����ƶ�
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.FAStatus != 'I'){ //�����в����ƶ�
			Print(STRING_SKIP);
			return;
		}
		if(!Status.TestIfHomed()){
			Print("HHH");
			return;
		}
		if(!Status.TestIfEnable()){ //δʹ����ʹ��
			Stepper.EnableStepper();
		}
		
		Print(STRING_MOVE);
		
		double Dest = LC2D(RxData+6);
		if(RxData[5] == 'X')
			Stepper.SectionMove(Dest,0);
		else if(RxData[5] == 'Y')
			Stepper.SectionMove(Dest,1);
		else if(RxData[5] == 'Z')
			Stepper.SectionMove(Dest,2);
		else if(RxData[5] == 'A')
			Stepper.SectionMove(Dest,3);
		else if(RxData[5] == 'B')
			Stepper.SectionMove(Dest,4);
		
		Print(STRING_DONE);
	}
	else if(RxData[4] == 'I'){ //λ�ó�ʼ��ָ��
		double Posi[5];
		Posi[0] = C2D(RxData+6);
		Posi[1] = C2D(RxData+19);
		Posi[2] = C2D(RxData+32);
		Posi[3] = LC2D(RxData+45);
		Posi[4] = LC2D(RxData+60);
		if(Status.InitPosition(Posi))
			Print(STRING_DONE);
		else
			Print(STRING_SKIP);
	}
	else if(RxData[4] == 'H'){ //�Զ���λָ��
		if(RxData[5] != 'H'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[6] != 'O'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[7] != 'M'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[8] != 'E'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[9] != 'X'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[10] != 'Y'){
			Print(STRING_NONE);
			return;
		}
		if(RxData[11] != 'Z'){
			Print(STRING_NONE);
			return;
		}
		
		if(Planner.L2RestAmount != L2_SIZE){ //L2����ǿղ��ɹ�λ
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.L1RestAmount != L1_SIZE){ //L1����ǿղ��ɹ�λ
			Print(STRING_SKIP);
			return;
		}
		if(Stepper.FAStatus != 'I'){ //�����в��ɹ�λ
			Print(STRING_SKIP);
			return;
		}
		if(!Status.TestIfEnable()){ //δʹ�ܲ��ɹ�λ
			Print(STRING_SKIP);
			return;
		}
		
		Print(STRING_MOVE);
		Stepper.AutoHoming();
		Print(STRING_DONE);
	}
	else if(RxData[4] == 'G'){ //��ȡָ��
		switch(RxData[5]){
			case 'X':Tx_Decimal(Status.GetPosition(0));break; //X������
			case 'Y':Tx_Decimal(Status.GetPosition(1));break; //Y������
			case 'Z':Tx_Decimal(Status.GetPosition(2));break; //Z������
			case 'A':Tx_Decimal(Status.GetPosition(3));break; //A������
			case 'B':Tx_Decimal(Status.GetPosition(4));break; //B������
			case 'O':Tx_Decimal(AXIS_X_MAX_POSITION);break;
			case 'P':Tx_Decimal(AXIS_Y_MAX_POSITION);break;
			case 'Q':Tx_Decimal(AXIS_Z_MAX_POSITION);break;
			case 'S':
				if(!Status.TestIfEnable())
					Print("DDD");
				else if(!Status.TestIfHomed())
					Print("HHH");
				else{
					Tx('%'); //�Զ���״̬
					Tx(Stepper.FAStatus);
					Tx(Stepper.FAStatus);
					Tx(Stepper.FAStatus);
					Tx('^');
				}
				break;
			case 'I': //IDP״̬
				if(Status.TestIfIDP()) Print("EEE");
				else Print("DDD");
				break;
			
			case 'E': //���״̬
				if(Status.TestIfEnable()) Print("EEE");
				else Print("DDD");
				break;
			
			case 'H':Tx('%'); //��λ״̬
				if(Status.TestIfHomed()) Print("EEE");
				else Print("DDD");
				break;
			
			case '1':Tx_Integer(Stepper.L1RestAmount);break; //L1����ʣ������
			case '2':Tx_Integer(Planner.L2RestAmount);break; //L2����ʣ������
			case 'M':Tx_Integer(L1_SIZE);break; //L1��������
			case 'N':Tx_Integer(L2_SIZE);break; //L2��������
			case '3': //����Ƿ��Ѿ���ƽ
				if(Status.TestIfLevel()) //�ѵ�ƽ
					Print("EEE");
				else //δ��ƽ
					Print("DDD");
				break;
			case '4': //��ȡ���Ե�A
				Tx_Decimal(Status.TestPoint[0]);
				break;
			case '5': //��ȡ���Ե�B
				Tx_Decimal(Status.TestPoint[1]);
				break;
			case '6': //��ȡ���Ե�C
				Tx_Decimal(Status.TestPoint[2]);
				break;
			case '7': //��ȡ���Ե�D
				Tx_Decimal(Status.TestPoint[3]);
				break;			
			case '8': //��ȡ���Ե�M
				Tx_Decimal(Status.TestPoint[4]);
				break;
			default:Print(STRING_NONE);break; //�޷�ʶ��
		}
	}
	else if(RxData[4] == 'P'){ //��������ָ��
		switch(RxData[5]){
			case 'X':Status.SetPosition(LC2D(RxData+6),0);Print(STRING_DONE);break; //X������
			case 'Y':Status.SetPosition(LC2D(RxData+6),1);Print(STRING_DONE);break; //Y������
			case 'Z':Status.SetPosition(LC2D(RxData+6),2);Print(STRING_DONE);break; //Z������
			case 'A':Status.SetPosition(LC2D(RxData+6),3);Print(STRING_DONE);break; //A������
			case 'B':Status.SetPosition(LC2D(RxData+6),4);Print(STRING_DONE);break; //B������
			default:Print(STRING_NONE);break; //�޷�ʶ��
		}
	}
	else if(RxData[4] == 'S'){ //�������״̬����ָ��
		switch(RxData[5]){
			case 'E':Stepper.EnableStepper();Print(STRING_DONE);break; //��������������ر��ƶ���
			case 'D': //�����ƶ������رղ������
				if(Stepper.DisableStepper())
					Print(STRING_DONE);
				else 
					Print(STRING_SKIP);
				break;
			default:Print(STRING_NONE);break; //�޷�ʶ��
		}
	}
	else if(RxData[4] == 'D'){ //IDP״̬����ָ��
		switch(RxData[5]){
			case 'E':Status.SetIDP(1);Print(STRING_DONE);break; //����IDP
			case 'D':Status.SetIDP(0);Print(STRING_DONE);break; //����IDP
			default:Print(STRING_NONE);break; //�޷�ʶ��
		}
	}
	else if(RxData[4] == 'T'){ //ͨ�Ų���ָ��
//		if(RxData[5] == 'R') Lcd.RestartFlag = 1;
		Print("DDD");
	}
	else{
		Print(STRING_NONE);
	}
}

void COM::Print(char *data){
	Tx('%');
	for(u8 i=0;i<24;i++){ //��಻����16�ַ�
		if(data[i]=='^' || !data[i]){ //�ǽ�����
			Tx('^'); //���ͽ�����
			return; //ֹͣ����
		}
		else //�ǽ�����
			Tx((u8)data[i]); //����һ���ַ�
	}
	Tx('^'); //���ͽ�����
}

void COM::Tx(u8 data){
	while(!(USART3->SR&(0x01<<7)));
	USART3->DR = data;
}

double COM::LC2D(u8 *Data){
	static s64 weight[] = {1,1,10,100,1000,10000,100000};
	s64 Integer = 0;
	s64 Decimal = 0;
	s64 Symbol = 1;
	
	if(*Data == '-') Symbol = -1;
	Data++;
	
	for(u8 i=0;i<6;i++) //��������
		Integer += (*(Data++)-'0')*weight[6-i];
	
	Data++; //С����

	for(u8 i=0;i<6;i++) //С������
		Decimal += (*(Data++)-'0')*weight[6-i];
	
	double Value = (double)Symbol*((double)Integer+(double)Decimal/1000000.0);

	return Value;
}

double COM::C2D(u8 *Data){
	static s64 weight[] = {1,1,10,100,1000,10000,100000,1000000};
	s64 Integer = 0;
	s64 Decimal = 0;
	s64 Symbol = 1;
	
	if(*Data == '-') Symbol = -1;
	Data++;
	
	for(u8 i=0;i<4;i++) //��������
		Integer += (*(Data++)-'0')*weight[4-i];
	
	Data++; //С����

	for(u8 i=0;i<6;i++) //С������
		Decimal += (*(Data++)-'0')*weight[6-i];
	
	double Value = (double)Symbol*((double)Integer+(double)Decimal/1000000.0);

	return Value;
}

float COM::C2F(u8 *Data){
	static u32 weight[] = {1,1,10,100};
	u32 Value = 0;
	for(u8 i=0;i<3;i++) //ת��Ϊ����
		Value += (Data[i]-'0')*weight[3-i];
	return Value*1.0f;
}

void COM::Tx_Decimal(double Data){
	u8 buffer[13] = {};
	u8 *Ptr = buffer;
	double TempFloat = Data;

	if(Data < 0){ //��ȡ����
		*(Ptr++) = '-';
		TempFloat = -TempFloat;
	}
	else *(Ptr++) = '+';
	
	u64 Integer = (u64)TempFloat; //��ȡ����
	u64 Decimal = (u64)(TempFloat*1000000.0); //��ȡС��
	
	static u64 weight[] = {1,1,10,100,1000,10000,100000,100000};
	
	for(u32 i=0;i<6;i++) //��������ת��
		*(Ptr++) = Integer%weight[6-i+1]/weight[6-i] + '0';
	
	*(Ptr++) = '.'; //С����
	
	for(u32 i=0;i<6;i++) //С������ת��
		*(Ptr++) = Decimal%weight[6-i+1]/weight[6-i] + '0';
	
	*Ptr = 0; //������
	
Print((char*)buffer);
}

void COM::Tx_Integer(u32 Data){
	u8 buffer[4] = {};		
	static u32 weight[] = {1,1,10,100,1000};
	for(u32 i=0;i<3;i++)
		buffer[i] = Data%weight[3-i+1]/weight[3-i] + '0'; //ת��
	buffer[3] = 0; //������
	Print((char*)buffer); //����
}

bool COM::TestAddress(){
	u32 C = 0;
	if(Buffer[1] == 'S') C++;
	if(Buffer[2] == 'S') C++;
	if(Buffer[3] == 'S') C++;
	
	if(C >= 2) return 1; //����ɹ�
	return 0;
}
