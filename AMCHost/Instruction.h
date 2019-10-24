#include "stm32f10x.h"

class INSTRUCTION{
	public:
	//TCON	
		void SetTemExtA(u32 Temp); //���ü�����A�¶�ָ��
		void SetTemExtB(u32 Temp); //���ü�����B�¶�ָ��
		void SetTemBP(u32 Temp); //���ü�����B�¶�ָ��

		void SetColExtA(u32 Temp); //���ü�����A��ȴ������ָ��
		void SetColExtB(u32 Temp); //���ü�����B��ȴ������ָ��

		void AutoHome(); //�Զ���λָ��

		void MoveAxisX(u32 Temp); //�ƶ�X��ָ��
		void MoveAxisY(u32 Temp); //�ƶ�Y��ָ��
		void MoveAxisZ(u32 Temp); //�ƶ�Z��ָ��

		void EnableNozzleA();
		void DisableNozzleA();
		void EnableNozzleB();
		void DisableNozzleB();
		
		void EnableStepper(); //ʹ�ܲ������ָ��

		void EnableIDP(); //ʹ��IDPָ��
		void DisableIDP(); //ʧ��IDPָ��
	
		void EnableMainLight(); //������
		void DisableMainLight(); //�ر�����
		void EnableAuxiliaryLight(); //�򿪸���
		void DisableAuxiliaryLight(); //�رո���
	
		void StorageInfo(); //��ʾ�洢����Ϣ
		
		void StartManufacturing(); //��ʼ����
		void StopManufacturing(); //ֹͣ����
		void PauseManufacturing(); //��ͣ����
		void ResumeManufacturing(); //��������
		void ReloadFilament(); //����ԭ��
		
		void WarningInfo(); //������Ϣ
		void ErrorInfo(); //������Ϣ
		void ManufacturingInfo(); //������Ϣ
		void ManufacturingReport(); //���챨��
		
		
		void RefreshDict(); //ˢ���ֵ�ָ��
		void NotFound(); //δ֪����
	private:
		void PrintNum(u32 Num);
		char *Num2Chr(char *Data,u32 Num);
};

extern class INSTRUCTION Instruct;
