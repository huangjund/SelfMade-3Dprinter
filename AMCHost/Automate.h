#include "stm32f10x.h"


class AUTOMATE{
	public:
		void Init();
	
		void Checking(u8 Select);
	
		void ClickCount();
		u32 ClickCounter;
	
		void StatusBarRefresh();
		void InfoLCDRefreash();
		void InfoLCDRefreashCount();
		bool FlagInfoLCDRefreash;
		u32 InfoLCDRefreashCounter;
	
		void TCONGetStatus();
		void TCONGetStatusCount();
		bool FlagTCONGetStatus;
		u32 TCONGetStatusCounter;
		
		void SCONGetStatus();
		void SCONGetStatusCount();
		bool FlagSCONGetStatus;
		u32 SCONGetStatusCounter;
	
		void FCONGetStatus();
		void FCONGetStatusCount();
		bool FlagFCONGetStatus;
		u32 FCONGetStatusCounter;
	
		void ConsoleGetCommand();
		void ConsoleGetCommandCount();
		bool FlagConsoleGetCommand;
		u32 ConsoleGetCommandCounter;
		
		void AUX1GetStatus();
		void AUX1GetStatusCount();
		bool FlagAUX1GetStatus;
		u32 AUX1GetStatusCounter;	
		
		void RTCGetStatus();
		void RTCGetStatusCount();
		bool FlagRTCGetStatus;
		u32 RTCGetStatusCounter;			
		
		void InterpreterRound();
		void InterpreterRoundCount();
		bool FlagInterpreterRound;
		u32 InterpreterRoundCounter;
		
};

extern class AUTOMATE Automate;

