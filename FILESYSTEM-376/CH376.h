#include "stm32f10x.h"

#ifndef _CH376_H
	#define _CH376_H

	#define UART1_RX_SIZE 1024 //最大连续接收字符数
	#define UART1_TX_SIZE 1024 //最大连续发送字符数

	#define RSTI_1
	#define RSTI_0

	#define GET_IC_VER 0x01 //获取芯片及固件版本
	#define SET_BAUDRATE 0x02 //设置串口通讯波特率
	#define ENTER_SLEEP 0x03 //进入低功耗睡眠挂起状态
	#define RESET_ALL 0x05 //执行硬件复位
	#define CHECK_EXIST 0x06 //测试通讯接口和工作状态
	#define SET_SD0_INT 0x0B //设置SPI的SDO引脚的中断方式
	#define GET_FILE_SIZE 0x0C //获取当前文件长度
	#define SET_USB_MODE 0x15 //设置USB工作模式
	#define GET_STATUS 0x22 //获取中断状态并取消中断请求
	#define RD_USB_DATA0 0x27 //从当前 USB 中断的端点缓冲区或者主机端点的接收缓冲区读取数据块
	#define WR_HOST_DATA 0x2C //向 USB 主机端点的发送缓冲区写入数据块
	#define WR_REQ_DATA 0x2D //向内部指定缓冲区写入请求的数据块
	#define WR_OFS_DATA 0x2E //向内部缓冲区指定偏移地址写入数据块
	#define SET_FILE_NAME 0x2F //设置将要操作的文件的文件名
	#define DISK_CONNECT 0x30 //检查磁盘是否连接
	#define DISK_MOUNT 0x31 //初始化磁盘并测试磁盘是否就绪
	#define FILE_OPEN 0x32 //打开文件或目录，枚举文件和目录
	#define FILE_ENUM_GO 0x33 //继续枚举文件和目录
	#define FILE_CREATE 0x34 //新建文件
	#define FILE_ERASE 0x35 //删除文件
	#define FILE_CLOSE 0x36 //关闭当前已经打开的文件或目录
	#define DIR_INFO_READ 0x37 //读取文件的目录信息
	#define DIR_INFO_SAVE 0x38 //保存文件的目录信息
	#define BYTE_LOCATE 0x39 //以字节为单位移动当前文件指针
	#define BYTE_READ 0x3A //以字节为单位从当前位置读取数据块
	#define BYTE_RD_GO 0x3B //继续字节读
	#define BYTE_WRITE 0x3C //以字节为单位向当前位置写入数据块
	#define BYTE_WR_GO 0x3D //继续字节写
	#define DISK_CAPACITY 0x3E //查询磁盘物理容量
	#define DISK_QUERY 0x3F //查询磁盘空间信息
	#define DIR_CREATE 0x40 //新建目录并打开或打开已存在的目录
	#define SEC_LOCATE 0x4A //以扇区为单位移动当前文件指针
	#define SEC_READ 0x4B //以扇区为单位从当前位置读取数据块
	#define SEC_WRITE 0x4C //以扇区为单位在当前位置写入数据块
	#define DISK_BOC_CMD 0x50 //对USB存储器执行BO传输协议的命令
	#define DISK_READ 0x54 //从USB存储器读物理扇区
	#define DISK_RD_GO 0x55 //继续USB存储器的物理扇区读操作
	#define DISK_WRITE 0x56 //向USB存储器写物理扇区
	#define DISK_WR_GO 0x57 //继续USB存储器的物理扇区写操作

	#define CMD_RET_SUCCESS 0x51 //操作成功
	#define CMD_RET_ABORT 0x5F //操作失败

	#define USB_INT_SUCCESS 0x14 //SD卡或者USB事务或者传输操作或者文件操作成功
	#define USB_INT_CONNECT 0x15 //检测到USB设备连接事件
	#define USB_INT_DISCONNECT 0x16 //检测到USB设备断开事件
	#define USB_INT_BUF_OVER 0x17 //传输的数据有误或者数据太多缓冲区溢出
	#define USB_INT_USB_READY 0x18 //USB设备已经被初始化(已经分配USB地址)
	#define USB_INT_DISK_READ 0x1D //存储设备读操作，请求数据读出
	#define USB_INT_DISK_WRITE 0x1E //存储设备写操作，请求数据写入
	#define USB_INT_DISK_ERR 0x1F //存储设备操作失败

	#define ERR_OPEN_DIR 0x41 //指定路径的目录被打开
	#define ERR_MISS_FILE 0x42 //指定路径的文件没有找到，可能是文件名称错误
	#define ERR_FOUND_NAME 0x43 //搜索到相匹配的文件名，或者是要求打开目录而实际结果却打开了文件
	#define ERR_DISK_DISCON 0x82 //磁盘尚未连接，可能磁盘已经断开
	#define ERR_LARGE_SECTOR 0x84 //磁盘的扇区太大，只支持每扇区 512 字节
	#define ERR_TYPE_ERROR 0x92 //磁盘分区类型不支持，需由磁盘管理工具重新分区
	#define ERR_BPB_ERROR 0xA1 //磁盘尚未格式化，或者参数错误，需要由 WINDOWS 采用默认参数重新格式化
	#define ERR_DISK_FULL 0xB1 //磁盘文件太满，剩余空间太少或者已经没有
	#define ERR_FDT_OVER 0xB2 //目录内文件太多，没有空闲目录项，需磁盘整理，FAT12/FAT16根目录下的文件数应该少于512个
	#define ERR_FILE_CLOSE 0xB4 //文件已经关闭，如果需要使用，应该重新打开文件

	#define ATTR_READ_ONLY 0x01 //文件为只读属性
	#define ATTR_HIDDEN 0x02 //文件为隐含属性
	#define ATTR_SYSTEM 0x04 //文件为系统属性
	#define ATTR_VOLUME_ID 0x08	 //卷标
	#define ATTR_DIRECTORY 0x10 //子目录(文件夹)
	#define ATTR_ARCHIVE 0x20 //文件为存档属性
	#define ATTR_LONG_NAME			( ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )	/* 长文件名属性 */
	#define ATTR_LONG_NAME_MASK		( ATTR_LONG_NAME | ATTR_DIRECTORY | ATTR_ARCHIVE )

	#define FILE_TIME(h,m,s) ((h<<11)|(m<<5)|(s>>1)) //生成指定时分秒的文件时间数据
	#define FILE_DATE(y,m,d) (((y-1980)<<9)|(m<<5)|d) //生成指定年月日的文件日期数据

	struct FATINFO {
		u8	DIR_Name[11]; //文件名,共11字节,不足处填空格
		u8	DIR_Attr;	//文件属性,参考后面的说明
		u8	DIR_NTRes; //
		u8	DIR_CrtTimeTenth;	//文件创建的时间,以0.1秒单位计数
		u16	DIR_CrtTime; //文件创建的时间
		u16	DIR_CrtDate; //文件创建的日期
		u16	DIR_LstAccDate; //最近一次存取操作的日期
		u16	DIR_FstClusHI; //
		u16	DIR_WrtTime; //文件修改时间
		u16	DIR_WrtDate; //文件修改日期
		u16	DIR_FstClusLO;
		u32	DIR_FileSize; //文件长度
	};

	struct DISKQUERY{
		u32	TotalSector; //当前逻辑盘的总扇区数
		u32	FreeSector; //当前逻辑盘的剩余扇区数
		u8	DiskFat; //当前逻辑盘的FAT类型,1-FAT12,2-FAT16,3-FAT32
	};	

	struct INQUERY{
		u8	DeviceType; //设备类型
		u8	RemovableMedia; //位7为1说明是移动存储
		u8	Versions; //协议版本
		u8	DataFormatAndEtc;	//指定返回数据格式
		u8	AdditionalLength;	//后续数据的长度
		u8	Reserved1;
		u8	Reserved2;
		u8	MiscFlag;	//一些控制标志
		u8	VendorIdStr[8];	//厂商信息
		u8	ProductIdStr[16];	//产品信息
		u8	ProductRevStr[4];	//产品版本
	};


	class SCI{
		public:	
			void Init(); //初始化
			void TX(u8 data[],u16 length); //发送数据(自动添加同步字)
			void Refresh(); //重置接收状态(计数器归零)
			bool WaitReceive(u32 Number,u16 TimeOut); //等待接收计数器达到预定值
		
			u8 RxBuffer [UART1_RX_SIZE]; //接收缓冲区,R
			u8 TxBuffer [UART1_TX_SIZE];	//发送缓冲区,W
			u16 RxCounter; //接收计数器
			bool TimeOutFlag; //超时标志
			
			bool CommandTime;
	};

	class CH376{
		public:
			u8 GetIcVer(); //获取芯片及固件版本
	//		bool SetBaudRate(bool devide, u8 para); //BaudRate=(6M/8^divide)/(256-para)
			void EnterSleep(); //进入低功耗睡眠挂起状态
			void ResetAll(); //执行硬件复位
			bool CheckExist(); //测试通讯接口和工作状态
	//		void SetSd0Int(bool disable); //禁止/允许SDO引脚用于中断输出
			u32 GetFileSize(); //获取当前文件的长度，即字节数
			bool SetUSBMode(u8 mode); //设置USB工作模式
	//		u8 GetStatus(); //获取中断状态并取消中断请求
			u8 ReadUSBData(u8 data[]); //从当前USB中断的端点缓冲区或者主机端点的接收缓冲区读取数据块
	//		bool WriteHostData(u8 data[],u8 length); //向USB主机端点的发送缓冲区写入数据块
	//		u8 WriteREQData(u8 data[]); //向内部指定缓冲区写入请求的数据块
	//		bool WriteOFSData(u8 offset,u8 length,u8 data[]); //向内部缓冲区指定偏移地址写入数据块
			void SetFileName(u8 data[],bool root); //设置将要操作的文件名或者目录名
			bool DiskConnect(); //检查磁盘是否连接，不支持SD卡
			bool DiskMount(INQUERY *query); //初始化磁盘并测试磁盘是否就绪
			u8 FileOpen(); //打开文件或目录
			u16 FileEnum(FATINFO info[],u8 name[],bool root); //枚举文件和目录
	//		bool FileCreate(u8 name[],bool root); //新建文件
	//		bool FileErase(u8 name[],bool root); //删除文件
			bool FileClose(bool resize); //关闭当前已经打开的文件或者目录
//			bool DirInfoRead(FATINFO *info); //读取文件的目录信息
	//		void DirInfoSave(FATINFO info); //保存文件的目录信息
			u32 ByteLocate(u32 offset); //以字节为单位移动当前文件指针
			u16 ByteRead(u8 data[],u16 length); //以字节为单位读取数据块
	//		void ByteWrite(u8 data[],u32 length); //以字节为单位向当前位置写入数据块
			u32 DiskCapacity(); //查询磁盘物理容量，以字节为单位
	//		bool DiskQuery(DISKQUERY *query); //查询磁盘空间信息，包括剩余空间和文件系统类型
	//		bool DirCreate(u8 name[]); //新建子目录并打开，如果子目录已经存在那么直接打开
	//		bool SecLocate(s32 offset); //以扇区为单位移动当前文件指针
	//		u32 SecRead(u8 data[], u8 number); //以扇区为单位从当前位置读取数据块
	//		u8 SecWrite(u8 data[],u8 number); //以扇区为单位向当前位置写入数据块
	//		u32 DiskRead(u8 data[],u32 address,u8 number); //从USB存储设备读物理扇区
	//		u8 DiskWrite(u8 data[],u32 address,u8 number); //向USB存储设备写物理扇区
	};

	class UDISK{
		public:
			bool Init();
			void TestIfConnect();

			bool ConnectFlag; //挂载标志
			bool TestIfConnectFlag;	//挂载检测请求标志
		
			INQUERY DiskInfoS; //U盘状态
			u32 DiskInfoF; //U盘总物理容量
		//  DISKQUERY DiskInfoC; //U盘容量状态
	};

	void Delay_Us(u32 n);
	void Delay_Ms(u32 n);
	
	extern class CH376 Ch376;
	extern class UDISK Udisk;

#endif
	
