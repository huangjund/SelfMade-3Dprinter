#include "stm32f10x.h"

class INSTRUCTION{
	public:
	//TCON	
		void SetTemExtA(u32 Temp); //设置挤出机A温度指令
		void SetTemExtB(u32 Temp); //设置挤出机B温度指令
		void SetTemBP(u32 Temp); //设置挤出机B温度指令

		void SetColExtA(u32 Temp); //设置挤出机A冷却器功率指令
		void SetColExtB(u32 Temp); //设置挤出机B冷却器功率指令

		void AutoHome(); //自动复位指令

		void MoveAxisX(u32 Temp); //移动X轴指令
		void MoveAxisY(u32 Temp); //移动Y轴指令
		void MoveAxisZ(u32 Temp); //移动Z轴指令

		void EnableNozzleA();
		void DisableNozzleA();
		void EnableNozzleB();
		void DisableNozzleB();
		
		void EnableStepper(); //使能步进电机指令

		void EnableIDP(); //使能IDP指令
		void DisableIDP(); //失能IDP指令
	
		void EnableMainLight(); //打开主灯
		void DisableMainLight(); //关闭主灯
		void EnableAuxiliaryLight(); //打开副灯
		void DisableAuxiliaryLight(); //关闭副灯
	
		void StorageInfo(); //显示存储器信息
		
		void StartManufacturing(); //开始制造
		void StopManufacturing(); //停止制造
		void PauseManufacturing(); //暂停制造
		void ResumeManufacturing(); //继续制造
		void ReloadFilament(); //更换原料
		
		void WarningInfo(); //警告信息
		void ErrorInfo(); //错误信息
		void ManufacturingInfo(); //制造信息
		void ManufacturingReport(); //制造报告
		
		
		void RefreshDict(); //刷新字典指令
		void NotFound(); //未知命令
	private:
		void PrintNum(u32 Num);
		char *Num2Chr(char *Data,u32 Num);
};

extern class INSTRUCTION Instruct;
