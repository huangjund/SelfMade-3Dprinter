#include "stm32f10x.h"

#ifndef _CH376_H
	#define _CH376_H

	#define UART1_RX_SIZE 1024 //������������ַ���
	#define UART1_TX_SIZE 1024 //������������ַ���

	#define RSTI_1
	#define RSTI_0

	#define GET_IC_VER 0x01 //��ȡоƬ���̼��汾
	#define SET_BAUDRATE 0x02 //���ô���ͨѶ������
	#define ENTER_SLEEP 0x03 //����͹���˯�߹���״̬
	#define RESET_ALL 0x05 //ִ��Ӳ����λ
	#define CHECK_EXIST 0x06 //����ͨѶ�ӿں͹���״̬
	#define SET_SD0_INT 0x0B //����SPI��SDO���ŵ��жϷ�ʽ
	#define GET_FILE_SIZE 0x0C //��ȡ��ǰ�ļ�����
	#define SET_USB_MODE 0x15 //����USB����ģʽ
	#define GET_STATUS 0x22 //��ȡ�ж�״̬��ȡ���ж�����
	#define RD_USB_DATA0 0x27 //�ӵ�ǰ USB �жϵĶ˵㻺�������������˵�Ľ��ջ�������ȡ���ݿ�
	#define WR_HOST_DATA 0x2C //�� USB �����˵�ķ��ͻ�����д�����ݿ�
	#define WR_REQ_DATA 0x2D //���ڲ�ָ��������д����������ݿ�
	#define WR_OFS_DATA 0x2E //���ڲ�������ָ��ƫ�Ƶ�ַд�����ݿ�
	#define SET_FILE_NAME 0x2F //���ý�Ҫ�������ļ����ļ���
	#define DISK_CONNECT 0x30 //�������Ƿ�����
	#define DISK_MOUNT 0x31 //��ʼ�����̲����Դ����Ƿ����
	#define FILE_OPEN 0x32 //���ļ���Ŀ¼��ö���ļ���Ŀ¼
	#define FILE_ENUM_GO 0x33 //����ö���ļ���Ŀ¼
	#define FILE_CREATE 0x34 //�½��ļ�
	#define FILE_ERASE 0x35 //ɾ���ļ�
	#define FILE_CLOSE 0x36 //�رյ�ǰ�Ѿ��򿪵��ļ���Ŀ¼
	#define DIR_INFO_READ 0x37 //��ȡ�ļ���Ŀ¼��Ϣ
	#define DIR_INFO_SAVE 0x38 //�����ļ���Ŀ¼��Ϣ
	#define BYTE_LOCATE 0x39 //���ֽ�Ϊ��λ�ƶ���ǰ�ļ�ָ��
	#define BYTE_READ 0x3A //���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ�
	#define BYTE_RD_GO 0x3B //�����ֽڶ�
	#define BYTE_WRITE 0x3C //���ֽ�Ϊ��λ��ǰλ��д�����ݿ�
	#define BYTE_WR_GO 0x3D //�����ֽ�д
	#define DISK_CAPACITY 0x3E //��ѯ������������
	#define DISK_QUERY 0x3F //��ѯ���̿ռ���Ϣ
	#define DIR_CREATE 0x40 //�½�Ŀ¼���򿪻���Ѵ��ڵ�Ŀ¼
	#define SEC_LOCATE 0x4A //������Ϊ��λ�ƶ���ǰ�ļ�ָ��
	#define SEC_READ 0x4B //������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ�
	#define SEC_WRITE 0x4C //������Ϊ��λ�ڵ�ǰλ��д�����ݿ�
	#define DISK_BOC_CMD 0x50 //��USB�洢��ִ��BO����Э�������
	#define DISK_READ 0x54 //��USB�洢������������
	#define DISK_RD_GO 0x55 //����USB�洢������������������
	#define DISK_WRITE 0x56 //��USB�洢��д��������
	#define DISK_WR_GO 0x57 //����USB�洢������������д����

	#define CMD_RET_SUCCESS 0x51 //�����ɹ�
	#define CMD_RET_ABORT 0x5F //����ʧ��

	#define USB_INT_SUCCESS 0x14 //SD������USB������ߴ�����������ļ������ɹ�
	#define USB_INT_CONNECT 0x15 //��⵽USB�豸�����¼�
	#define USB_INT_DISCONNECT 0x16 //��⵽USB�豸�Ͽ��¼�
	#define USB_INT_BUF_OVER 0x17 //��������������������̫�໺�������
	#define USB_INT_USB_READY 0x18 //USB�豸�Ѿ�����ʼ��(�Ѿ�����USB��ַ)
	#define USB_INT_DISK_READ 0x1D //�洢�豸���������������ݶ���
	#define USB_INT_DISK_WRITE 0x1E //�洢�豸д��������������д��
	#define USB_INT_DISK_ERR 0x1F //�洢�豸����ʧ��

	#define ERR_OPEN_DIR 0x41 //ָ��·����Ŀ¼����
	#define ERR_MISS_FILE 0x42 //ָ��·�����ļ�û���ҵ����������ļ����ƴ���
	#define ERR_FOUND_NAME 0x43 //��������ƥ����ļ�����������Ҫ���Ŀ¼��ʵ�ʽ��ȴ�����ļ�
	#define ERR_DISK_DISCON 0x82 //������δ���ӣ����ܴ����Ѿ��Ͽ�
	#define ERR_LARGE_SECTOR 0x84 //���̵�����̫��ֻ֧��ÿ���� 512 �ֽ�
	#define ERR_TYPE_ERROR 0x92 //���̷������Ͳ�֧�֣����ɴ��̹��������·���
	#define ERR_BPB_ERROR 0xA1 //������δ��ʽ�������߲���������Ҫ�� WINDOWS ����Ĭ�ϲ������¸�ʽ��
	#define ERR_DISK_FULL 0xB1 //�����ļ�̫����ʣ��ռ�̫�ٻ����Ѿ�û��
	#define ERR_FDT_OVER 0xB2 //Ŀ¼���ļ�̫�࣬û�п���Ŀ¼����������FAT12/FAT16��Ŀ¼�µ��ļ���Ӧ������512��
	#define ERR_FILE_CLOSE 0xB4 //�ļ��Ѿ��رգ������Ҫʹ�ã�Ӧ�����´��ļ�

	#define ATTR_READ_ONLY 0x01 //�ļ�Ϊֻ������
	#define ATTR_HIDDEN 0x02 //�ļ�Ϊ��������
	#define ATTR_SYSTEM 0x04 //�ļ�Ϊϵͳ����
	#define ATTR_VOLUME_ID 0x08	 //���
	#define ATTR_DIRECTORY 0x10 //��Ŀ¼(�ļ���)
	#define ATTR_ARCHIVE 0x20 //�ļ�Ϊ�浵����
	#define ATTR_LONG_NAME			( ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )	/* ���ļ������� */
	#define ATTR_LONG_NAME_MASK		( ATTR_LONG_NAME | ATTR_DIRECTORY | ATTR_ARCHIVE )

	#define FILE_TIME(h,m,s) ((h<<11)|(m<<5)|(s>>1)) //����ָ��ʱ������ļ�ʱ������
	#define FILE_DATE(y,m,d) (((y-1980)<<9)|(m<<5)|d) //����ָ�������յ��ļ���������

	struct FATINFO {
		u8	DIR_Name[11]; //�ļ���,��11�ֽ�,���㴦��ո�
		u8	DIR_Attr;	//�ļ�����,�ο������˵��
		u8	DIR_NTRes; //
		u8	DIR_CrtTimeTenth;	//�ļ�������ʱ��,��0.1�뵥λ����
		u16	DIR_CrtTime; //�ļ�������ʱ��
		u16	DIR_CrtDate; //�ļ�����������
		u16	DIR_LstAccDate; //���һ�δ�ȡ����������
		u16	DIR_FstClusHI; //
		u16	DIR_WrtTime; //�ļ��޸�ʱ��
		u16	DIR_WrtDate; //�ļ��޸�����
		u16	DIR_FstClusLO;
		u32	DIR_FileSize; //�ļ�����
	};

	struct DISKQUERY{
		u32	TotalSector; //��ǰ�߼��̵���������
		u32	FreeSector; //��ǰ�߼��̵�ʣ��������
		u8	DiskFat; //��ǰ�߼��̵�FAT����,1-FAT12,2-FAT16,3-FAT32
	};	

	struct INQUERY{
		u8	DeviceType; //�豸����
		u8	RemovableMedia; //λ7Ϊ1˵�����ƶ��洢
		u8	Versions; //Э��汾
		u8	DataFormatAndEtc;	//ָ���������ݸ�ʽ
		u8	AdditionalLength;	//�������ݵĳ���
		u8	Reserved1;
		u8	Reserved2;
		u8	MiscFlag;	//һЩ���Ʊ�־
		u8	VendorIdStr[8];	//������Ϣ
		u8	ProductIdStr[16];	//��Ʒ��Ϣ
		u8	ProductRevStr[4];	//��Ʒ�汾
	};


	class SCI{
		public:	
			void Init(); //��ʼ��
			void TX(u8 data[],u16 length); //��������(�Զ����ͬ����)
			void Refresh(); //���ý���״̬(����������)
			bool WaitReceive(u32 Number,u16 TimeOut); //�ȴ����ռ������ﵽԤ��ֵ
		
			u8 RxBuffer [UART1_RX_SIZE]; //���ջ�����,R
			u8 TxBuffer [UART1_TX_SIZE];	//���ͻ�����,W
			u16 RxCounter; //���ռ�����
			bool TimeOutFlag; //��ʱ��־
			
			bool CommandTime;
	};

	class CH376{
		public:
			u8 GetIcVer(); //��ȡоƬ���̼��汾
	//		bool SetBaudRate(bool devide, u8 para); //BaudRate=(6M/8^divide)/(256-para)
			void EnterSleep(); //����͹���˯�߹���״̬
			void ResetAll(); //ִ��Ӳ����λ
			bool CheckExist(); //����ͨѶ�ӿں͹���״̬
	//		void SetSd0Int(bool disable); //��ֹ/����SDO���������ж����
			u32 GetFileSize(); //��ȡ��ǰ�ļ��ĳ��ȣ����ֽ���
			bool SetUSBMode(u8 mode); //����USB����ģʽ
	//		u8 GetStatus(); //��ȡ�ж�״̬��ȡ���ж�����
			u8 ReadUSBData(u8 data[]); //�ӵ�ǰUSB�жϵĶ˵㻺�������������˵�Ľ��ջ�������ȡ���ݿ�
	//		bool WriteHostData(u8 data[],u8 length); //��USB�����˵�ķ��ͻ�����д�����ݿ�
	//		u8 WriteREQData(u8 data[]); //���ڲ�ָ��������д����������ݿ�
	//		bool WriteOFSData(u8 offset,u8 length,u8 data[]); //���ڲ�������ָ��ƫ�Ƶ�ַд�����ݿ�
			void SetFileName(u8 data[],bool root); //���ý�Ҫ�������ļ�������Ŀ¼��
			bool DiskConnect(); //�������Ƿ����ӣ���֧��SD��
			bool DiskMount(INQUERY *query); //��ʼ�����̲����Դ����Ƿ����
			u8 FileOpen(); //���ļ���Ŀ¼
			u16 FileEnum(FATINFO info[],u8 name[],bool root); //ö���ļ���Ŀ¼
	//		bool FileCreate(u8 name[],bool root); //�½��ļ�
	//		bool FileErase(u8 name[],bool root); //ɾ���ļ�
			bool FileClose(bool resize); //�رյ�ǰ�Ѿ��򿪵��ļ�����Ŀ¼
//			bool DirInfoRead(FATINFO *info); //��ȡ�ļ���Ŀ¼��Ϣ
	//		void DirInfoSave(FATINFO info); //�����ļ���Ŀ¼��Ϣ
			u32 ByteLocate(u32 offset); //���ֽ�Ϊ��λ�ƶ���ǰ�ļ�ָ��
			u16 ByteRead(u8 data[],u16 length); //���ֽ�Ϊ��λ��ȡ���ݿ�
	//		void ByteWrite(u8 data[],u32 length); //���ֽ�Ϊ��λ��ǰλ��д�����ݿ�
			u32 DiskCapacity(); //��ѯ�����������������ֽ�Ϊ��λ
	//		bool DiskQuery(DISKQUERY *query); //��ѯ���̿ռ���Ϣ������ʣ��ռ���ļ�ϵͳ����
	//		bool DirCreate(u8 name[]); //�½���Ŀ¼���򿪣������Ŀ¼�Ѿ�������ôֱ�Ӵ�
	//		bool SecLocate(s32 offset); //������Ϊ��λ�ƶ���ǰ�ļ�ָ��
	//		u32 SecRead(u8 data[], u8 number); //������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ�
	//		u8 SecWrite(u8 data[],u8 number); //������Ϊ��λ��ǰλ��д�����ݿ�
	//		u32 DiskRead(u8 data[],u32 address,u8 number); //��USB�洢�豸����������
	//		u8 DiskWrite(u8 data[],u32 address,u8 number); //��USB�洢�豸д��������
	};

	class UDISK{
		public:
			bool Init();
			void TestIfConnect();

			bool ConnectFlag; //���ر�־
			bool TestIfConnectFlag;	//���ؼ�������־
		
			INQUERY DiskInfoS; //U��״̬
			u32 DiskInfoF; //U������������
		//  DISKQUERY DiskInfoC; //U������״̬
	};

	void Delay_Us(u32 n);
	void Delay_Ms(u32 n);
	
	extern class CH376 Ch376;
	extern class UDISK Udisk;

#endif
	
