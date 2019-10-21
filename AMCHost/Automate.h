#include "stm32f10x.h"


class AUTOMATE{
	public:
		void Init();
	
		void Checking();
	
		//LCD
		void InfoLCDRefreash();
		void InfoLCDRefreashCount();
		bool FlagInfoLCDRefreash;
		u32 InfoLCDRefreashCounter;
		//Temperature Control
		void TCONGetStatus();
		void TCONGetStatusCount();
		bool FlagTCONGetStatus;
		u32 TCONGetStatusCounter;
		//Steper Control
		void SCONGetStatus();
		void SCONGetStatusCount();
		bool FlagSCONGetStatus;
		u32 SCONGetStatusCounter;
		//Console Control
		void ConsoleGetCommand();
		void ConsoleGetCommandCount();
		bool FlagConsoleGetCommand;
		u32 ConsoleGetCommandCounter;
};

extern class AUTOMATE Automate;