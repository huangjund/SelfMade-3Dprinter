#include "stm32f10x.h"

class INSTRUCTION{
	public:
	//TCON
		void GetTemExtA(); //��ȡ������A�¶�ָ��
		void GetTemExtAInit(); //��ȡ������A�¶�ָ���ʼ��
		char **GetTemExtA_InsSet; //��ȡ������A�¶�ָ��ؼ��ʼ�
		u32 GetTemExtAOperand; //��ȡ������A�¶�ָ�����������
	
		void GetTemExtB(); //��ȡ������B�¶�ָ��
		void GetTemExtBInit(); //��ȡ������B�¶�ָ���ʼ��
		char **GetTemExtB_InsSet; //��ȡ������B�¶�ָ��ؼ��ʼ�
		u32 GetTemExtBOperand; //��ȡ������B�¶�ָ�����������
		
		void GetTemBP(); //��ȡ��׼���¶�ָ��
		void GetTemBPInit(); //��ȡ��׼���¶�ָ���ʼ��
		char **GetTemBP_InsSet; //��ȡ��׼���¶�ָ��ؼ��ʼ�
		u32 GetTemBPOperand; //��ȡ��׼���¶�ָ�����������

		void GetTemEnv(); //��ȡ�����¶�ָ��
		void GetTemEnvInit(); //��ȡ�����¶�ָ���ʼ��
		char **GetTemEnv_InsSet; //��ȡ�����¶�ָ��ؼ��ʼ�
		u32 GetTemEnvOperand; //��ȡ�����¶�ָ�����������
	
		void GetColPowEA(); //��ȡ������A��ȴ������ָ��
		void GetColPowEAInit(); //��ȡ������A��ȴ������ָ���ʼ��
		char **GetColPowEA_InsSet; //��ȡ������A��ȴ������ָ��ؼ��ʼ�
		u32 GetColPowEAOperand; //��ȡ������A��ȴ������ָ�����������
	
		void GetColPowEB(); //��ȡ������B��ȴ������ָ��
		void GetColPowEBInit(); //��ȡ������B��ȴ������ָ���ʼ��
		char **GetColPowEB_InsSet; //��ȡ������B��ȴ������ָ��ؼ��ʼ�
		u32 GetColPowEBOperand; //��ȡ������B��ȴ������ָ�����������
	
		void GetTemEAS(); //��ȡ������A�趨�¶�ָ��
		void GetTemEASInit(); //��ȡ������A�趨�¶�ָ���ʼ��
		char **GetTemEAS_InsSet; //��ȡ������A�趨�¶�ָ��ؼ��ʼ�
		u32 GetTemEASOperand; //��ȡ������A�趨�¶�ָ�����������

		void GetTemEBS(); //��ȡ������B�趨�¶�ָ��
		void GetTemEBSInit(); //��ȡ������B�趨�¶�ָ���ʼ��
		char **GetTemEBS_InsSet; //��ȡ������B�趨�¶�ָ��ؼ��ʼ�
		u32 GetTemEBSOperand; //��ȡ������B�趨�¶�ָ�����������

		void GetTemBPS(); //��ȡ��׼���趨�¶�ָ��
		void GetTemBPSInit(); //��ȡ��׼���趨�¶�ָ���ʼ��
		char **GetTemBPS_InsSet; //��ȡ��׼���趨�¶�ָ��ؼ��ʼ�
		u32 GetTemBPSOperand; //��ȡ��׼���趨�¶�ָ�����������
	
		void SetTemExtA(); //���ü�����A�¶�ָ��
		void SetTemExtAInit(); //���ü�����A�¶�ָ���ʼ��
		char **SetTemExtA_InsSet; //���ü�����A�¶�ָ��ؼ��ʼ�
		u32 SetTemExtAOperand; //���ü�����A�¶�ָ�����������

		void SetTemExtB(); //���ü�����B�¶�ָ��
		void SetTemExtBInit(); //���ü�����B�¶�ָ���ʼ��
		char **SetTemExtB_InsSet; //���ü�����B�¶�ָ��ؼ��ʼ�
		u32 SetTemExtBOperand; //���ü�����B�¶�ָ�����������

		void SetTemBP(); //���ü�����B�¶�ָ��
		void SetTemBPInit(); //���ü�����B�¶�ָ���ʼ��
		char **SetTemBP_InsSet; //���ü�����B�¶�ָ��ؼ��ʼ�
		u32 SetTemBPOperand; //���ü�����B�¶�ָ�����������

		void SetColExtA(); //���ü�����A��ȴ������ָ��
		void SetColExtAInit(); //���ü�����A��ȴ������ָ���ʼ��
		char **SetColExtA_InsSet; //���ü�����A��ȴ������ָ��ؼ��ʼ�
		u32 SetColExtAOperand; //���ü�����A��ȴ������ָ�����������

		void SetColExtB(); //���ü�����B��ȴ������ָ��
		void SetColExtBInit(); //���ü�����B��ȴ������ָ���ʼ��
		char **SetColExtB_InsSet; //���ü�����B��ȴ������ָ��ؼ��ʼ�
		u32 SetColExtBOperand; //���ü�����B��ȴ������ָ�����������

		void GetStaExtA(); //��ȡ������A״ָ̬��
		void GetStaExtAInit(); //��ȡ������A״ָ̬���ʼ��
		char **GetStaExtA_InsSet; //��ȡ������A״ָ̬��ؼ��ʼ�
		u32 GetStaExtAOperand; //��ȡ������A״ָ̬�����������
	
		void GetStaExtB(); //��ȡ������B״ָ̬��
		void GetStaExtBInit(); //��ȡ������B״ָ̬���ʼ��
		char **GetStaExtB_InsSet; //��ȡ������B״ָ̬��ؼ��ʼ�
		u32 GetStaExtBOperand; //��ȡ������B״ָ̬�����������
		
		void GetStaBP(); //��ȡ��׼��״ָ̬��
		void GetStaBPInit(); //��ȡ��׼��״ָ̬���ʼ��
		char **GetStaBP_InsSet; //��ȡ��׼��״ָ̬��ؼ��ʼ�
		u32 GetStaBPOperand; //��ȡ��׼��״ָ̬�����������

		void AutoHome(); //�Զ���λָ��
		void AutoHomeInit(); //�Զ���λָ���ʼ��
		char **AutoHome_InsSet; //�Զ���λָ��ؼ��ʼ�
		u32 AutoHomeOperand; //�Զ���λָ�����������

		void RefreshDict(); //ˢ���ֵ�ָ��
		void RefreshDictInit(); //ˢ���ֵ�ָ���ʼ��
		char **RefreshDict_InsSet; //ˢ���ֵ�ָ��ؼ��ʼ�
		u32 RefreshDictOperand; //ˢ���ֵ�ָ�����������
	
	
		void NotFound(); //δ֪����
	private:
		void PrintNum(u32 Num);
		char *Num2Chr(char *Data,u32 Num);
};

extern class INSTRUCTION Instruct;
