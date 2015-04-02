#ifndef __TW2868_H__
#define __TW2868_H__

#include <linux/ioctl.h>

#ifndef	__TYPEDEFS__
#define	__TYPEDEFS__

#ifdef TASKINGC

#define DATA			_data
#define PDATA			_pdat
#define IDATA			idat
#define CODE			_rom
#define REENTRANT		_reentrant
#define	AT(atpos2)		_at( atpos2 )
#define CODE_P			_rom
#define PDATA_P			_pdat
#define IDATA_P			idat
#define DATA_P			_data

#define INTERRUPT( num, name )	_interrupt(num) void name (void)

#elif defined KEILC

#define DATA		data
#define PDATA		pdata
#define IDATA		idata
#define XDATA		xdata
#define CODE		code
#define CODE_P
#define PDATA_P
#define IDATA_P
#define DATA_P

#define INTERRUPT( num, name ) name() interrupt num

#endif
#define	BIT0		(0x0001)
#define	BIT1		(0x0002)
#define	BIT2		(0x0004)
#define	BIT3		(0x0008)
#define	BIT4		(0x0010)
#define	BIT5		(0x0020)
#define	BIT6		(0x0040)
#define	BIT7		(0x0080)
#define	BIT8		(0x0100)
#define	BIT9		(0x0200)
#define	BIT10		(0x0400)
#define	BIT11		(0x0800)
#define	BIT12		(0x1000)
#define	BIT13		(0x2000)
#define	BIT14		(0x4000)
#define	BIT15		(0x8000)

#define U8  unsigned char
#define U16 unsigned int
#define S8  signed char
#define S16 signed int

//==================================================================================
//#define	NULL		0
#define	ON			1
#define	OFF		0

//==================================================================================
#define	SetBit(x,y)			((x) |= (y))         
#define	ClearBit(x,y)		((x) &= ~(y))        
#define	BitSet(x,y)			(((x)&(y))== (y))    
#define	BitClear(x,y)		(((x)&(y))== 0)      
#define	IfBitSet(x,y)		if(((x)&(y)) == (y)) 
#define	IfBitClear(x,y)	if(((x)&(y)) == 0)


#define _between(x, a, b)	(a<=x && x<=b)
#define _swap(a, b)			{ a^=b; b^=a; a^=b; }

typedef	unsigned char	Register;
typedef	unsigned char	BYTE;
typedef	unsigned int	WORD;
//typedef	unsigned long	DWORD;

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef FALSE
#define	FALSE	0
#endif

//VInputStdDetectMode
#define AUTO	0

//VInputStd
#define NTSC	1			
#define PAL		2
#define SECAM	3
#define NTSC4	4
#define PALM	5
#define PALN	6
#define PAL60	7

#define MAXINPUTSTD	PAL60				// 

#define UNKNOWN	0xfe
#define NOINPUT	0						//0xff


typedef struct _BITMAP{
	unsigned short height;				//vertical size
	unsigned short width;				// horizontal size			
	unsigned char *bmpdata;				//bitmap data pointer
} BITMAP;


#define NIL			0xff

#endif	/* __TYPEDEFS__ */

#define TW286XA_I2C_ADDR 	0x50
#define TW286XB_I2C_ADDR 	0x54
#define TW286XC_I2C_ADDR 	0x52
#define TW286XD_I2C_ADDR 	0x56

#define TW28XX_NTSC	        1
#define TW28XX_PAL	        2
#define TW28XX_AUTO	        3
#define TW28XX_INVALID_MODE	4


#define TW286X_1D1_MODE     0
#define TW286X_2D1_MODE     1
#define TW286X_4CIF_MODE    2
#define TW286X_4D1_MODE     3


#define TW286X_SET_HUE          0x01
#define TW286X_SET_CONTRAST     0x02
#define TW286X_SET_BRIGHT       0x04
#define TW286X_SET_SATURATION   0x08
#define TW286X_SET_SHARPNESS    0x10

typedef struct Tw28xxNode
{
	unsigned char tw28xxaddr;     //芯片的地址
	int tw28xxtype;      // 芯片类型
	int tw28xxindex;     //芯片所在的位置
}Tw28xxNode; 

typedef struct Tw28xxdev
{
	Tw28xxNode tw28xxNode[10];
	int NodeCount;     //一共有多少(2866 2868 2826)芯片
}Tw28xxdev;

typedef struct _tw2865_work_mode
{
    unsigned int chip;
    unsigned int mode;
} tw2865_work_mode;

#define MAX_VIDEO_CHIP_COUNT 10
typedef enum VideoChipType
{
 NONE = 0,
 TW2826 = 1,
 TW2868 = 2,
 TW2864 = 5,
 TW2867= 6,
 NC1104 = 7,
 NC1108 = 8,
 NC1700 = 9,
 CX25828 = 10,
 CX25838 =11,
}VideoChipType_e;

typedef struct VideoChipNode_s
{
 unsigned int addr;
 VideoChipType_e type;
}VideoChipNode_t; 

typedef struct VideoChip_s
{
 int videoChipCount;
 VideoChipNode_t VideoChipNode[MAX_VIDEO_CHIP_COUNT];
}VideoChip_t;



typedef struct _tw2865_video_norm
{
    unsigned int chip;
    unsigned int mode;
} tw2865_video_norm;

typedef struct tw28xxstr
{
	BYTE ChipId;
	BYTE ChipType;
	BYTE Register;
	BYTE Data;
}tw28xxstr;

typedef enum _tw286x_audio_samplerate
{
	TW286X_SAMPLE_RATE_8000,
	TW286X_SAMPLE_RATE_16000,
	TW286X_SAMPLE_RATE_32000,
	TW286X_SAMPLE_RATE_44100,
	TW286X_SAMPLE_RATE_48000,
	TW286X_SAMPLE_RATE_BUTT
} tw286x_audio_samplerate;


typedef struct _tw286x_audio_playback
{
    unsigned int chip;
    unsigned int chn;
} tw286x_audio_playback;

typedef struct _ttw286x_audio_in_volume
{
	unsigned int chip;
	unsigned int chn;
	unsigned int volume;
} tw286x_audio_in_volume;

typedef struct _tw286x_video_sharpness
{
    unsigned int chip;
    unsigned int chn;
    unsigned int horizon_sharp;
    unsigned int vertical_sharp;
} tw286x_video_sharpness;

typedef struct _tw286x_set_XYDelay
{
    unsigned int chip;
    unsigned int chn;
    unsigned int X_Delay;
    unsigned int Y_Delay;
} tw286x_set_XYDelay;


typedef struct _ttw286x_audio_da_volume
{
    unsigned int chip;
    unsigned int volume;
} tw286x_audio_da_volume;

typedef struct _tw286x_video_loss
{
    unsigned int chip;
    unsigned int ch;
    unsigned int is_lost;
} tw286x_video_loss;

typedef struct _tw286x_image_adjust
{
	unsigned char chip;
	unsigned char chn;
	unsigned char hue;
	unsigned char contrast;
	unsigned char brightness;
	unsigned char saturation;
	unsigned long item_sel;/* use such as "TW2865_SET_HUE|TW2865_SET_CONTRAST" */
} tw286x_image_adjust;


#define TW28XX_IOC_MAGIC		't'

#define TW2826_IOC_MAGIC           TW28XX_IOC_MAGIC
#define TW2865_IOC_MAGIC           TW28XX_IOC_MAGIC
#define TW2868_IOC_MAGIC           TW28XX_IOC_MAGIC

#define  TW2826_SELECT_VI_CHN	_IOWR(TW2826_IOC_MAGIC, 1, int) 
#define TW28XXCMD_READ_REG		    _IOWR(TW2826_IOC_MAGIC, 2, tw28xxstr) 
#define TW28XXCMD_WRITE_REG	        _IOW(TW2826_IOC_MAGIC, 3, tw28xxstr) 
#define TW286X_GET_VIDEO_LOSS		_IOWR(TW28XX_IOC_MAGIC, 4, tw286x_video_loss) 
#define TW286X_SET_IMAGE_ADJUST	    _IOW(TW28XX_IOC_MAGIC, 5, tw286x_image_adjust) 
#define TW286X_GET_IMAGE_ADJUST	    _IOWR(TW28XX_IOC_MAGIC, 6, tw286x_image_adjust) 
#define TW2865_SET_VIDEO_NORM	    _IOW(TW2865_IOC_MAGIC, 7, tw2865_video_norm) 
#define TW2865_GET_VIDEO_NORM	    _IOWR(TW2865_IOC_MAGIC, 8, tw2865_video_norm) 
#define TW2865_SET_WORK_MODE	    _IOW(TW2865_IOC_MAGIC, 9, tw2865_work_mode) 
#define TW286X_SET_SAMPLE_RATE	    _IOW(TW28XX_IOC_MAGIC, 10, tw286x_audio_samplerate) 
#define TW286X_SET_AUDIO_PLAYBACK   _IOW(TW28XX_IOC_MAGIC, 11, tw286x_audio_playback) 
#define TW286X_SET_AUDIO_DA_VOLUME  _IOW(TW28XX_IOC_MAGIC, 13, tw286x_audio_da_volume)  
#define TW286X_SET_SHARPNESS     _IOW(TW28XX_IOC_MAGIC, 14, tw286x_video_sharpness)  
#define TW286X_SET_XY_DELAY     _IOW(TW28XX_IOC_MAGIC, 15, tw286x_set_XYDelay)  
#define GET_AD_COUNT     _IOWR(TW28XX_IOC_MAGIC, 16, int)
#define GET_PER_AD_INFOR     _IOWR(TW28XX_IOC_MAGIC, 17, VideoChip_t)

#if 1 /*bxinquan add 20121122*/
#define TW286X_GET_AUDIO_DA_VOLUME  _IOWR(TW28XX_IOC_MAGIC, 18, tw286x_audio_da_volume)
#define TW286X_GET_AUDIO_IN_VOLUME  _IOWR(TW28XX_IOC_MAGIC, 19, tw286x_audio_in_volume)
#define TW286X_SET_AUDIO_IN_VOLUME  _IOW(TW28XX_IOC_MAGIC, 20, tw286x_audio_in_volume)
#endif

#if 0
#define TW2868CMD_READ_REG		    _IOWR(TW2868_IOC_MAGIC, 1, int) 
#define TW2868CMD_WRITE_REG	        _IOW(TW2868_IOC_MAGIC, 2, int) 
#define TW2868_GET_VIDEO_LOSS		_IOWR(TW2868_IOC_MAGIC, 4, tw2868_video_loss) 
#define TW2868_SET_IMAGE_ADJUST	    _IOW(TW2868_IOC_MAGIC, 5, tw2868_image_adjust) 
#define TW2868_GET_IMAGE_ADJUST	    _IOWR(TW2868_IOC_MAGIC, 6, tw2868_image_adjust) 
#define TW2868_SET_VIDEO_NORM	    _IOW(TW2868_IOC_MAGIC, 7, tw2868_video_norm) 
#define TW2868_GET_VIDEO_NORM	    _IOWR(TW2868_IOC_MAGIC, 8, tw2868_video_norm) 
#define TW2868_SET_AUDIO_DA_MUTE    _IOW(TW2868_IOC_MAGIC, 12, tw2868_audio_da_mute)
#define TW2868_SET_AUDIO_DA_VOLUME  _IOW(TW2868_IOC_MAGIC, 13, tw2868_audio_da_volume)   
#endif

#endif


