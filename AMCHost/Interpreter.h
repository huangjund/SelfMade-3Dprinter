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
		
		char Start(); //启动
		char Stop(); //停止	
		char Pause(); //暂停
		double PrePosition[5];
		char Resume(); //继续
		char ReLoad(); //换料
	
		void Tick(); //计时器加一秒
	
		void Reset(); 
		
		//K-正忙 1-运行系统故障 2-热力系统故障 3-文件系统故障 4-辅助系统故障
		//H-未复位 F-文件不存在 D-检验完成
		char PreCheck();
	
		//E-空指令 R-校验失败 D-校验成功
		char GetInstruction();
		void LocalAnalyse();
	
		//F-文件不存在 E-缓存空 H-未复位 A-A缺料 B-B缺料 D-检验完毕
		char StatusCheck();
	
		//E-缓存空 D-执行完毕 H-未复位 R/N-通信错误 T-通信超时 U-其他原因错误
		//2-温度过高 1-温度过低 X-温度计故障 
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

