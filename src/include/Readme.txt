

BVPU_SDK��һЩ�淶��
1�����г����Ķ��壨����define����ö�ٵȵȣ�������BVPU_SDK_Const.h�С�ע�⾡��������request/response������״̬��transaction�ͺ������ö�����BVPU_SDK_SUBCOMMAND_XXX�������������SDK��API̫����������ҡ�ͬʱҪ��BVPU_SDK_SUBCOMMAND_XXX�е������������ĵ����е�������˵����
2�����нṹ�嶨�嶼����BVPU_SDK_DataTypes.h�У�ע��ѷ���ţ�����˵MediaStream��صĽṹ�嶨��ͷ���structs of Media Stream���С�
3�����нӿڶ��嶼����BVPU_SDK.h�С����ж���ӿڶ��嶼���밴��doxygen��ʽ����д������ע�͡�


/*===========����ӿڷ���=====================*/
����ӿ����ͷ��ࣺ
1���豸��������
		/*Control*/
		
		/*Query*/
		BVPU_SDK_SUBCOMMAND_CAPABILITIES_DEVICE��
		BVPU_SDK_SUBCOMMAND_CAPABILITIES_PTZ��
		
		
		/*Control/Query*/
		
		

2��SDK��log��
		/*Control*/
		
		/*Query*/
		
		/*Control/Query*/
		BVPU_SDK_SUBCOMMAND_SDK_LOG


/*===========����ʾ��=====================*/
����ʾ����
1����ͨ����IPͨ������
		��ʼ��SDK��
		BVPU_SDK_Init()
		�����豸ͨ����Ϣ
		BVPU_SDK_Command(BVPU_SDK_COMMAND_CONTROL��BVPU_SDK_SUBCOMMAND_AVCHANNEL_DEVICE_INFO,xxxx)
		��ͨ������ͨ���ص�������BVPU_SDK_MediaStream_WriteMediaData����д��Ͷ�ȡ������
		BVPU_SDK_MediaStream_Open()
		����ͨ����
		BVPU_SDK_MediaStream_Close()
		����SDK��
		BVPU_SDK_DeInit()
		
2����̨����

3��
