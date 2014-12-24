

BVPU_SDK的一些规范：
1、所有常量的定义（比如define或者枚举等等）都放在BVPU_SDK_Const.h中。注意尽量把所有request/response这种无状态的transaction和函数调用都做成BVPU_SDK_SUBCOMMAND_XXX的命令，这样避免SDK的API太多而导致凌乱。同时要对BVPU_SDK_SUBCOMMAND_XXX中的命令类型有文档进行单独分类说明。
2、所有结构体定义都放在BVPU_SDK_DataTypes.h中，注意把分类放，比如说MediaStream相关的结构体定义就放在structs of Media Stream段中。
3、所有接口定义都放在BVPU_SDK.h中。所有对外接口定义都必须按照doxygen格式，编写完整的注释。


/*===========命令接口分类=====================*/
命令接口类型分类：
1、设备能力集。
		/*Control*/
		
		/*Query*/
		BVPU_SDK_SUBCOMMAND_CAPABILITIES_DEVICE，
		BVPU_SDK_SUBCOMMAND_CAPABILITIES_PTZ，
		
		
		/*Control/Query*/
		
		

2、SDK的log。
		/*Control*/
		
		/*Query*/
		
		/*Control/Query*/
		BVPU_SDK_SUBCOMMAND_SDK_LOG


/*===========流程示例=====================*/
流程示例：
1、打开通道（IP通道）。
		初始化SDK库
		BVPU_SDK_Init()
		设置设备通道信息
		BVPU_SDK_Command(BVPU_SDK_COMMAND_CONTROL，BVPU_SDK_SUBCOMMAND_AVCHANNEL_DEVICE_INFO,xxxx)
		打开通道流，通过回调函数和BVPU_SDK_MediaStream_WriteMediaData（）写入和读取数据流
		BVPU_SDK_MediaStream_Open()
		销毁通道流
		BVPU_SDK_MediaStream_Close()
		销毁SDK库
		BVPU_SDK_DeInit()
		
2、云台控制

3、
