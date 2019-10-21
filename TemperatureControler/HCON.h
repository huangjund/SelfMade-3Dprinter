#include "stm32f10x.h"

class HCON{
	public:
		void Init();

		void SetTemperatureExtruderA(u32 T); //�趨������AԤ���¶�
		void SetTemperatureExtruderB(u32 T); //�趨������BԤ���¶�
		void SetTemperatureBasePlane(u32 T); //�趨��׼��Ԥ���¶�
	
		u32 GetExtruderASetting(); //��ȡ������A�趨�¶�
		u32 GetExtruderBSetting(); //��ȡ������B�趨�¶�
		u32 GetBasePlaneSetting(); //��ȡ��׼���趨�¶�
	
		u32 GetPowerHeaterExtruderA(); //��ȡ������A����������
		u32 GetPowerHeaterExtruderB(); //��ȡ������BA����������
		u32 GetPowerHeaterBasePlane(); //��ȡ��׼�����������
	
	
		char GetStatusExtruderA(); //��ȡ������A״̬
		char GetStatusExtruderB(); //��ȡ������B״̬
		char GetStatusBasePlane(); //��ȡ��׼��״̬
	
		void CaclStatusExtruderA(); //���㼷����A״̬
		void CaclStatusExtruderB(); //���㼷����B״̬
		void CaclStatusBasePlane(); //�����׼��״̬
		
		void CaclPWMExtruderA(); //���㼷����Aռ�ձ�
		void CaclPWMExtruderB(); //���㼷����Bռ�ձ�
		void CaclPWMBasePlane(); //�����׼��ռ�ձ�
		
	private:
		u32 TemperatureExtruderASetting; //������A�趨�¶�
		u32 TemperatureExtruderBSetting; //������B�趨�¶�
		u32 TemperatureBasePlaneSetting; //��׼���趨�¶�
		
		char StatusExtruderA; //������A״̬
		char StatusExtruderB; //������B״̬
		char StatusBasePlane; //��׼��״̬
	

};

extern class HCON HeaterControler;
