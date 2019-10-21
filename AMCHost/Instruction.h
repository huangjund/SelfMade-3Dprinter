#include "stm32f10x.h"

class INSTRUCTION{
	public:
	//TCON
		void GetTemExtA(); //获取挤出机A温度指令
		void GetTemExtAInit(); //获取挤出机A温度指令初始化
		char **GetTemExtA_InsSet; //获取挤出机A温度指令关键词集
		u32 GetTemExtAOperand; //获取挤出机A温度指令操作数数量
	
		void GetTemExtB(); //获取挤出机B温度指令
		void GetTemExtBInit(); //获取挤出机B温度指令初始化
		char **GetTemExtB_InsSet; //获取挤出机B温度指令关键词集
		u32 GetTemExtBOperand; //获取挤出机B温度指令操作数数量
		
		void GetTemBP(); //获取基准面温度指令
		void GetTemBPInit(); //获取基准面温度指令初始化
		char **GetTemBP_InsSet; //获取基准面温度指令关键词集
		u32 GetTemBPOperand; //获取基准面温度指令操作数数量

		void GetTemEnv(); //获取环境温度指令
		void GetTemEnvInit(); //获取环境温度指令初始化
		char **GetTemEnv_InsSet; //获取环境温度指令关键词集
		u32 GetTemEnvOperand; //获取环境温度指令操作数数量
	
		void GetColPowEA(); //获取挤出机A冷却器功率指令
		void GetColPowEAInit(); //获取挤出机A冷却器功率指令初始化
		char **GetColPowEA_InsSet; //获取挤出机A冷却器功率指令关键词集
		u32 GetColPowEAOperand; //获取挤出机A冷却器功率指令操作数数量
	
		void GetColPowEB(); //获取挤出机B冷却器功率指令
		void GetColPowEBInit(); //获取挤出机B冷却器功率指令初始化
		char **GetColPowEB_InsSet; //获取挤出机B冷却器功率指令关键词集
		u32 GetColPowEBOperand; //获取挤出机B冷却器功率指令操作数数量
	
		void GetTemEAS(); //获取挤出机A设定温度指令
		void GetTemEASInit(); //获取挤出机A设定温度指令初始化
		char **GetTemEAS_InsSet; //获取挤出机A设定温度指令关键词集
		u32 GetTemEASOperand; //获取挤出机A设定温度指令操作数数量

		void GetTemEBS(); //获取挤出机B设定温度指令
		void GetTemEBSInit(); //获取挤出机B设定温度指令初始化
		char **GetTemEBS_InsSet; //获取挤出机B设定温度指令关键词集
		u32 GetTemEBSOperand; //获取挤出机B设定温度指令操作数数量

		void GetTemBPS(); //获取基准面设定温度指令
		void GetTemBPSInit(); //获取基准面设定温度指令初始化
		char **GetTemBPS_InsSet; //获取基准面设定温度指令关键词集
		u32 GetTemBPSOperand; //获取基准面设定温度指令操作数数量
	
		void SetTemExtA(); //设置挤出机A温度指令
		void SetTemExtAInit(); //设置挤出机A温度指令初始化
		char **SetTemExtA_InsSet; //设置挤出机A温度指令关键词集
		u32 SetTemExtAOperand; //设置挤出机A温度指令操作数数量

		void SetTemExtB(); //设置挤出机B温度指令
		void SetTemExtBInit(); //设置挤出机B温度指令初始化
		char **SetTemExtB_InsSet; //设置挤出机B温度指令关键词集
		u32 SetTemExtBOperand; //设置挤出机B温度指令操作数数量

		void SetTemBP(); //设置挤出机B温度指令
		void SetTemBPInit(); //设置挤出机B温度指令初始化
		char **SetTemBP_InsSet; //设置挤出机B温度指令关键词集
		u32 SetTemBPOperand; //设置挤出机B温度指令操作数数量

		void SetColExtA(); //设置挤出机A冷却器功率指令
		void SetColExtAInit(); //设置挤出机A冷却器功率指令初始化
		char **SetColExtA_InsSet; //设置挤出机A冷却器功率指令关键词集
		u32 SetColExtAOperand; //设置挤出机A冷却器功率指令操作数数量

		void SetColExtB(); //设置挤出机B冷却器功率指令
		void SetColExtBInit(); //设置挤出机B冷却器功率指令初始化
		char **SetColExtB_InsSet; //设置挤出机B冷却器功率指令关键词集
		u32 SetColExtBOperand; //设置挤出机B冷却器功率指令操作数数量

		void GetStaExtA(); //获取挤出机A状态指令
		void GetStaExtAInit(); //获取挤出机A状态指令初始化
		char **GetStaExtA_InsSet; //获取挤出机A状态指令关键词集
		u32 GetStaExtAOperand; //获取挤出机A状态指令操作数数量
	
		void GetStaExtB(); //获取挤出机B状态指令
		void GetStaExtBInit(); //获取挤出机B状态指令初始化
		char **GetStaExtB_InsSet; //获取挤出机B状态指令关键词集
		u32 GetStaExtBOperand; //获取挤出机B状态指令操作数数量
		
		void GetStaBP(); //获取基准面状态指令
		void GetStaBPInit(); //获取基准面状态指令初始化
		char **GetStaBP_InsSet; //获取基准面状态指令关键词集
		u32 GetStaBPOperand; //获取基准面状态指令操作数数量

		void AutoHome(); //自动复位指令
		void AutoHomeInit(); //自动复位指令初始化
		char **AutoHome_InsSet; //自动复位指令关键词集
		u32 AutoHomeOperand; //自动复位指令操作数数量

		void RefreshDict(); //刷新字典指令
		void RefreshDictInit(); //刷新字典指令初始化
		char **RefreshDict_InsSet; //刷新字典指令关键词集
		u32 RefreshDictOperand; //刷新字典指令操作数数量
	
	
		void NotFound(); //未知命令
	private:
		void PrintNum(u32 Num);
		char *Num2Chr(char *Data,u32 Num);
};

extern class INSTRUCTION Instruct;
