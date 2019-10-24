#include "stm32f10x.h"

/*       

                 Failed                                
  +------+----------------+                                                                                 +----------------+
  |      |                |                                                                                 V                |
	| 	+------+ Start +----------+ Done +----------------+ Done +--------------+ Done +-------------+ Done +---------+        |
	+-->| Idel |---+   | PreCheck |----->| GetInstruction |----->| LocalAnalyse |----->| StatusCheck |----->| Process |--+     |
      +------+   |   +----------+      +----------------+      +--------------+      +-------------+      +---------+  |     |
         A       |        A                  A     |                     |                     |                  |    |     |
				 |			 |		 	  |                  |     +---------------------+---------------------+------------------+    |     |
				 |			 |		+-------+		           |Unblock 		  							  					 				                    |    |     |
				 |			 +--->| Reset |              |                    +-------------+	 Error 	                        |    |     |
				 |					  +-------+				       |                    | ErrorManage |<--------------------------------+    |     |
				 |											 	           |                    +-------------+                                      |     |
         |                                   |                           |                                             |     |
         |                                   |                           V                                             |     |
         |          +-------+  Stop          |                    +--------------+                      Done           |     |
				 +----------| Reset |<---------------+----------+---------| SignalHandle |<---+--------------------------------+     |
										+-------+                |          |         +--------------+    |                                      |
														                 |          |Reload      |                |                                      |
                                             |          |            +----------------+                                      |
																						 |		      V                 Blocked     |				                               |
	                                           |      +--------+     Done               |                                      |
																						 |			| Reload |------------------------+                                      |
																						 |			+--------+                                   Insufficient Buffer         |
																						 +-------------------------------------------------------------------------------+			
*/

class INTERPRETER{
	public:
		void Init();
		void Round();
		
		char Start(); //����
		char Stop(); //ֹͣ	
		char Pause(); //��ͣ
		double PrePosition[5];
		char Resume(); //����
		char ReLoad(); //����
	
		void Tick(); //��ʱ����һ��
	
		void Reset(); 
		
		//K-��æ 1-����ϵͳ���� 2-����ϵͳ���� 3-�ļ�ϵͳ���� 4-����ϵͳ����
		//H-δ��λ F-�ļ������� D-�������
		char PreCheck();
	
		//E-��ָ�� R-У��ʧ�� D-У��ɹ�
		char GetInstruction();
		void LocalAnalyse();
	
		//F-�ļ������� E-����� H-δ��λ A-Aȱ�� B-Bȱ�� D-�������
		char StatusCheck();
	
		//E-����� D-ִ����� H-δ��λ R/N-ͨ�Ŵ��� T-ͨ�ų�ʱ U-����ԭ�����
		//2-�¶ȹ��� 1-�¶ȹ��� X-�¶ȼƹ��� 
		char Process();
		
		//Step: 0-GetInstruction 1-StatusCheck 2-Process
		u8 ErrorStep;
		char ErrorCode;
		
		struct{
			bool Used;
			u16 HMS[3];
			u32 InsCounter;
		}Report;
		
		struct{
			char Warning[10][2];
			u8 WarningLoad;
			char Error[2];
			u16 HMS[3];
			u32 Percent;
			bool CountEnable;
		}Info;
	
		char RoundStatus;
		char Signal;
	
		u8 LoadIndex;
		u8 LaunchIndex;
		u8 RestBuffer;
		struct{
			u32 InsCounter;
			float VelocityPres;
			double PositionPres[5];
			bool ExtruderPres;
			bool AbsoluteCrod;
		}StreamStatus;
	
		struct{
			u32 BaseAddress;
			bool GM_Exis;
			u8 GM_Para;
			bool T_Exis;
			u8 T_Para;
			bool S_Exis;
			u32 S_Para;
			bool P_Exis;
			u32 P_Para;
			bool X_Exis;
			double X_Para;
			bool Y_Exis;
			double Y_Para;
			bool Z_Exis;
			double Z_Para;
			bool A_Exis;
			double A_Para;
			bool B_Exis;
			double B_Para;
			bool E_Exis;
			double E_Para;
			bool F_Exis;
			float F_Para;
			bool V_Exis;
			u8 V_Para;
		}TempIns;
		struct{
			u8 Type;
			u32 LineCounter;
			u32 BaseAddress;
			u8 Segment;
			
			bool AxisExis[5];
			double Position[5];
			float Velocity;
			
			bool ExtruderExis[2];
			u32 TemperatureE[2];
			
			bool BasePlaneExis;
			u32 TemperatureB;
			
			bool CoolerExis[2];
			u8 CoolerPower[2];

		}Buffer[32];
		
		u8 TempData[128];
	private:
		double CharToDouble(u8 *Data);
		double AbsDouble(double Data);
		s32 CharToDecimal(u8 *Data);
		u32 AbsDecimal(s32 Data);
		void AxisRound(double &Data,double Diff);
};

extern class INTERPRETER Interpreter;

