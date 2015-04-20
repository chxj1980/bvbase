/*************************************************************************
    > File Name: his3515.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月22日 星期一 15时42分02秒
 ************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#line 25 "his3515.c"

#include <libbvsystem/bvsystem.h>
#include <libbvmedia/driver.h>
#include <libbvconfig/common.h>
#include <libbvutil/bvstring.h>
#include <libbvutil/time.h>

//FIXME c99 not support asm
#define asm __asm__

//His3515 headers
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_aio.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_ai.h"
#include "mpi_ao.h"

#define BREAK_WHEN_SDK_FAILED(comment, s32Ret) \
    do { \
        if (s32Ret != HI_SUCCESS) { \
            bv_log(s, BV_LOG_ERROR, comment " 0x%X\n", s32Ret); \
            goto fail; \
        } \
    }while(0)

#define MAX_AUDIO_CHIPS (16)
#define MAX_VIDEO_CHIPS (16)

typedef struct His3515SystemContext {
    const BVClass *bv_class;
    BVMediaDriverContext *adriver[MAX_AUDIO_CHIPS];
    BVMediaDriverContext *vdriver[MAX_AUDIO_CHIPS];
} His3515SystemContext;

static void his3515_exit(void)
{
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();
}

/**
 *  His3515 MPP 
 */
static int his3515_system_init(BVSystemContext *s)
{
    MPP_SYS_CONF_S stSysConf;
    VB_CONF_S stVbConf;
    MPP_VERSION_S stVersion;
    HI_S32 s32Ret = HI_FAILURE;

    his3515_exit();
    
    BBCLEAR_STRUCT(stSysConf);
    BBCLEAR_STRUCT(stVbConf);
    BBCLEAR_STRUCT(stVersion);

    s32Ret = HI_MPI_SYS_GetVersion(&stVersion);
    BREAK_WHEN_SDK_FAILED("Get MPP Version Error", s32Ret);
    bv_log(s, BV_LOG_INFO, "Mpp Version is %s\n", stVersion.aVersion);
    
    stSysConf.u32AlignWidth = 64;
    s32Ret = HI_MPI_SYS_SetConf(&stSysConf);
    BREAK_WHEN_SDK_FAILED("Set System Config Error", s32Ret);

    s32Ret = HI_MPI_SYS_Init();
    BREAK_WHEN_SDK_FAILED("System Init Error", s32Ret);

    stVbConf.u32MaxPoolCnt = 128;
    stVbConf.astCommPool[0].u32BlkSize = 704 * 576 * 2;
    stVbConf.astCommPool[0].u32BlkCnt  = 64;
    stVbConf.astCommPool[1].u32BlkSize = 352 * 288 * 2;
    stVbConf.astCommPool[1].u32BlkCnt  = 64;

    s32Ret = HI_MPI_VB_SetConf(&stVbConf);
    BREAK_WHEN_SDK_FAILED("Set Video Buffer Config Error", s32Ret);

    s32Ret = HI_MPI_VB_Init();
    BREAK_WHEN_SDK_FAILED("Init Video Buffer Error", s32Ret);

    s32Ret = HI_MPI_SYS_InitPtsBase(0);
    BREAK_WHEN_SDK_FAILED("Init pts Base Error", s32Ret);
     
    bv_log(s, BV_LOG_DEBUG, "system init success\n");
    return 0;
fail:
    return BVERROR(EINVAL);
}

typedef struct StringTable {
    const char *name;
    int type;
} StringTable;

typedef struct IntTable {
    int a;
    int b;
} IntTable;

static int get_input_interface(const char *name)
{
    int i = 0;
    StringTable inputs[] = {
        { "BT656", VI_MODE_BT656},
        { "BT601", VI_MODE_BT601},
        { "BT1120I", VI_MODE_BT1120_INTERLACED},
        { "BT1120P", VI_MODE_BT1120_PROGRESSIVE},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(inputs); i++) {
        if (bv_strcasecmp(inputs[i].name, name) == 0)
           return inputs[i].type; 
    }
    return VI_MODE_BT656;
}

static int get_input_work_mode(const char *name)
{
    int i = 0;
    StringTable modes[] = {
        { "4D1" , VI_WORK_MODE_4D1},
        { "4HALFD1", VI_WORK_MODE_4HALFD1},
        { "2D1", VI_WORK_MODE_2D1},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(modes); i++) {
        if (bv_strcasecmp(modes[i].name, name) == 0)
           return modes[i].type; 
    }
    return VI_WORK_MODE_4D1;
}

static int get_output_interface(const char *name)
{
    int i = 0;
    StringTable outputs[] = {
        { "CVBS", VO_INTF_CVBS},
        { "BT656", VO_INTF_BT656},
        { "VGA", VO_INTF_VGA},
        { "BT1120", VO_INTF_BT1120},
    };

    for (i = 0; i < BV_ARRAY_ELEMS(outputs); i++) {
        if (bv_strcasecmp(outputs[i].name, name) == 0)
           return outputs[i].type; 
    }
    return VO_INTF_CVBS;
}

static int get_output_work_mode(const char *name)
{
    int i = 0;
    StringTable modes[] = {
        { "NTSC", VO_OUTPUT_NTSC},
        { "PAL", VO_OUTPUT_PAL},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(modes); i++) {
        if (bv_strcasecmp(modes[i].name, name) == 0)
           return modes[i].type; 
    }
    return -1;
}

static int get_audio_sample_rate(int sample_rate)
{
    int i = 0;
    IntTable sample_rates[] = {
        { 8000,  AUDIO_SAMPLE_RATE_8000},
        { 11025, AUDIO_SAMPLE_RATE_11025},
        { 16000, AUDIO_SAMPLE_RATE_16000},
        { 22050, AUDIO_SAMPLE_RATE_22050},
        { 24000, AUDIO_SAMPLE_RATE_24000},
        { 32000, AUDIO_SAMPLE_RATE_32000},
        { 44100, AUDIO_SAMPLE_RATE_44100},
        { 48000, AUDIO_SAMPLE_RATE_48000},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(sample_rates); i++) {
        if (sample_rates[i].a == sample_rate) {
            return sample_rates[i].b;
        } 
    }
    return AUDIO_SAMPLE_RATE_8000;
}

static int get_audio_sample_format(int sample_format)
{
    int i = 0;
    IntTable sample_formats[] = {
        { 8 ,  AUDIO_BIT_WIDTH_8},
        { 16 , AUDIO_BIT_WIDTH_16},
        { 32 , AUDIO_BIT_WIDTH_32},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(sample_formats); i++) {
        if (sample_formats[i].a == sample_format) {
            return sample_formats[i].b;
        } 
    }
    return AUDIO_BIT_WIDTH_16;
}

static int get_audio_channel_mode(int channel_count)
{
    int i = 0;
    IntTable channels[] = {
        { 1, AUDIO_SOUND_MODE_MOMO},
        { 2, AUDIO_SOUND_MODE_STEREO},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(channels); i++) {
        if (channels[i].a == channel_count) {
            return channels[i].b;
        } 
    }
    return AUDIO_SOUND_MODE_MOMO;
}

static int get_audio_work_mode(const char *name)
{
    int i = 0;
    StringTable modes[] = {
        { "I2S_SLAVE", AIO_MODE_I2S_SLAVE},
        { "I2S_MASTER", AIO_MODE_I2S_MASTER},
        { "PCM_SLAVE", AIO_MODE_PCM_SLAVE_STD},
        { "PCM_SLAVE", AIO_MODE_PCM_SLAVE_STD},
        { "PCM_NSLAVE", AIO_MODE_PCM_SLAVE_NSTD},
        { "PCM_MASTER", AIO_MODE_PCM_MASTER_STD},
        { "PCM_NMASTER", AIO_MODE_PCM_MASTER_NSTD},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(modes); i++) {
        if (bv_strcasecmp(modes[i].name, name) == 0)
           return modes[i].type; 
    }
    return AIO_MODE_I2S_SLAVE;
}

static int his3515_videv_config(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
 //   His3515SystemContext *hisctx = s->priv_data;
    VI_PUB_ATTR_S stViPubAttr;
    HI_S32 s32Ret = HI_FAILURE; 
    BVVideoSourceDevice *videvice = pkt_in->data;
    int32_t videv = 0;
    BBCLEAR_STRUCT(stViPubAttr);

    if (sscanf(videvice->token, "%d", &videv) != 1) {
        bv_log(s, BV_LOG_ERROR, "videv param error\n");
        return BVERROR(EINVAL);
    }
    //FIXME
    stViPubAttr.enInputMode = get_input_interface(videvice->interface);
    if (stViPubAttr.enInputMode == VI_MODE_BT656) {
        stViPubAttr.enWorkMode  = get_input_work_mode(videvice->work_mode);
    }
    s32Ret = HI_MPI_VI_SetPubAttr(videv, &stViPubAttr);
    BREAK_WHEN_SDK_FAILED("Set ViDev Pub Attr Error", s32Ret);

    s32Ret = HI_MPI_VI_Enable(videv);
    BREAK_WHEN_SDK_FAILED("Enable Videv Error", s32Ret);
    bv_log(s, BV_LOG_DEBUG, "set videv config success\n");
    return 0;
fail:
    return BVERROR(EINVAL);
}

static int his3515_vodev_config(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
 //   His3515SystemContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    VO_PUB_ATTR_S stVoPubAttr;
    VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
    BVVideoOutputDevice *vodevice = (BVVideoOutputDevice *)pkt_in->data;
    int32_t vodev = 0;
    if (sscanf(vodevice->token, "%d", &vodev) != 1) {
        bv_log(s, BV_LOG_ERROR, "vodev param error\n");
        return BVERROR(EINVAL);
    }

    BBCLEAR_STRUCT(stVoPubAttr);
    BBCLEAR_STRUCT(stVoLayerAttr);
    
    s32Ret = HI_MPI_VO_GetPubAttr(vodev, &stVoPubAttr);
    BREAK_WHEN_SDK_FAILED("get VO PubAttr Error", s32Ret);

    s32Ret = HI_MPI_VO_GetVideoLayerAttr(vodev, &stVoLayerAttr);
    BREAK_WHEN_SDK_FAILED("get VIdeo LayerAttr Error", s32Ret);
    
    HI_MPI_VO_DisableVideoLayer(vodev);
    HI_MPI_VO_Disable(vodev);

    stVoPubAttr.u32BgColor = 0;
    //FIXME
    stVoPubAttr.enIntfType = get_output_interface(vodevice->interface);
    //stVoPubAttr.enIntfSync = pConfig->iMode == VIDEO_STD_NTSC ? VO_OUTPUT_NTSC : VO_OUTPUT_PAL;
    stVoPubAttr.enIntfSync = get_output_work_mode(vodevice->work_mode);
    stVoLayerAttr.stDispRect.s32X = vodevice->display.x;
    stVoLayerAttr.stDispRect.s32Y = vodevice->display.y;
    stVoLayerAttr.stDispRect.u32Width = vodevice->display.width;
    stVoLayerAttr.stDispRect.u32Height = vodevice->display.height;
    stVoLayerAttr.stImageSize.u32Width = vodevice->display.width;
    stVoLayerAttr.stImageSize.u32Height = vodevice->display.height;
    stVoLayerAttr.u32DispFrmRt = 25;
    if (stVoPubAttr.enIntfSync == VO_OUTPUT_NTSC) {
        stVoLayerAttr.u32DispFrmRt = 30;
    }
    stVoLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;

	s32Ret = HI_MPI_VO_SetPubAttr(vodev, &stVoPubAttr);
	BREAK_WHEN_SDK_FAILED("Set VOPub Attr Error", s32Ret);

    s32Ret = HI_MPI_VO_Enable(vodev);
    BREAK_WHEN_SDK_FAILED("Enable VO Device Error", s32Ret);

    s32Ret = HI_MPI_VO_SetVideoLayerAttr(vodev, &stVoLayerAttr);
    BREAK_WHEN_SDK_FAILED("Set VideoLayerAttr Error", s32Ret);

    s32Ret = HI_MPI_VO_EnableVideoLayer(vodev);
    BREAK_WHEN_SDK_FAILED("Enable Video Layer Error", s32Ret);

    bv_log(s, BV_LOG_DEBUG, "set vodev config success\n");
    return 0;
fail:
    return BVERROR(EINVAL);
}

static int his3515_aidev_config(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    His3515SystemContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    AIO_ATTR_S stAIOAttr;
    BVAudioSourceDevice *aidevice = (BVAudioSourceDevice *)pkt_in->data;
    BVControlPacket pkt;
    int32_t aidev = 0;

    if (sscanf(aidevice->token, "%d", &aidev) != 1) {
        bv_log(s, BV_LOG_ERROR, "aidev param error\n");
        return BVERROR(EINVAL);
    }

    BBCLEAR_STRUCT(stAIOAttr);

	s32Ret = HI_MPI_AI_Disable(aidev);
    BREAK_WHEN_SDK_FAILED("Disable AiDev Error", s32Ret);
    stAIOAttr.enBitwidth = get_audio_sample_format(aidevice->sample_format);
    stAIOAttr.enSamplerate = get_audio_sample_rate(aidevice->sample_rate);
    stAIOAttr.enSoundmode = get_audio_channel_mode(aidevice->channel_mode);
    stAIOAttr.enWorkmode = get_audio_work_mode(aidevice->work_mode);
    stAIOAttr.u32ChnCnt = aidevice->channel_counts;
    stAIOAttr.u32FrmNum = 30;
    stAIOAttr.u32PtNumPerFrm = aidevice->sample_points;

    if (bv_media_driver_open(&hisctx->adriver[aidev], aidevice->dev, aidevice->chip, NULL, NULL) < 0) {
        bv_log(s, BV_LOG_ERROR, "open audio driver error %s\n", aidevice->dev);
    }
    pkt.data = &aidevice->sample_rate;
    pkt.size = 1;
    bv_media_driver_control(hisctx->adriver[aidev], BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_SOURCE_SET_SAMPLE, &pkt, NULL);

//    AudioSampleSet(aidevice->sample_rate);

    s32Ret = HI_MPI_AI_SetPubAttr(aidev, &stAIOAttr);
    BREAK_WHEN_SDK_FAILED("set aidev config error", s32Ret);

    s32Ret = HI_MPI_AI_Enable(aidev);
    BREAK_WHEN_SDK_FAILED("Enable AiDev Error", s32Ret);

    bv_log(s, BV_LOG_DEBUG, "set aidev config success\n");
    return 0;
fail:
    return BVERROR(EINVAL);
}

static int his3515_aodev_config(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
 //   His3515SystemContext *hisctx = s->priv_data;
    HI_S32 s32Ret = HI_FAILURE;
    AIO_ATTR_S stAIOAttr;
    BVAudioOutputDevice *aodevice = (BVAudioOutputDevice *)pkt_in->data;
    int32_t aodev = 0;

    if (sscanf(aodevice->token, "%d", &aodev) != 1) {
        bv_log(s, BV_LOG_ERROR, "aodev param error\n");
        return BVERROR(EINVAL);
    }

    BBCLEAR_STRUCT(stAIOAttr);

	s32Ret = HI_MPI_AO_Disable(aodev);
    BREAK_WHEN_SDK_FAILED("Disable AoDev Error", s32Ret);

    stAIOAttr.enBitwidth = get_audio_sample_format(aodevice->sample_format);
    stAIOAttr.enSamplerate = get_audio_sample_rate(aodevice->sample_rate);
    stAIOAttr.enSoundmode = get_audio_channel_mode(aodevice->channel_mode);
    stAIOAttr.enWorkmode = get_audio_work_mode(aodevice->work_mode);
    stAIOAttr.u32ChnCnt = aodevice->channel_counts;
    stAIOAttr.u32FrmNum = 25;
    stAIOAttr.u32PtNumPerFrm = 320;

    //AudioSampleSet(aodevice->sample_rate);

	s32Ret = HI_MPI_AO_SetPubAttr(aodev, &stAIOAttr);
    BREAK_WHEN_SDK_FAILED("Set AoPubAttr Error", s32Ret);
	
	s32Ret = HI_MPI_AO_Enable(aodev);
    BREAK_WHEN_SDK_FAILED("Enable AoDev Error", s32Ret);

    bv_log(s, BV_LOG_DEBUG, "set aodev config success\n");
    return 0;
fail:
    return BVERROR(EINVAL);
}

static int his3515_sync_pts(BVSystemContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    HI_S32 s32Ret = HI_FAILURE;
    int64_t cur_pts = bv_gettime_relative();
    s32Ret = HI_MPI_SYS_SyncPts(cur_pts);
    BREAK_WHEN_SDK_FAILED("sync pts error", s32Ret);
    return 0;
fail:
    return BVERROR(EIO);
}

static int his3515_system_control(BVSystemContext *s, enum BVSystemMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVSystemMessageType type;
        int (*control)(BVSystemContext *h, const BVControlPacket *, BVControlPacket *);
    } system_control[] = {
        { BV_SYS_MESSAGE_TYPE_SYNPTS, his3515_sync_pts},
        { BV_SYS_MESSAGE_TYPE_VIUDEV, his3515_videv_config},
        { BV_SYS_MESSAGE_TYPE_VOUDEV, his3515_vodev_config},
        { BV_SYS_MESSAGE_TYPE_AIMDEV, his3515_aidev_config},
        { BV_SYS_MESSAGE_TYPE_AOMDEV, his3515_aodev_config},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(system_control); i++) {
        if (system_control[i].type == type)
           return system_control[i].control(s, pkt_in, pkt_out); 
    }
    bv_log(s, BV_LOG_ERROR, "Not Support This command \n");
    
    return BVERROR(ENOSYS);
}

static int his3515_system_exit(BVSystemContext *s)
{
    his3515_exit();
    return 0;
}

#define OFFSET(x) offsetof(His3515SystemContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
//    { "ad_chip", "", OFFSET(ad_chip), BV_OPT_TYPE_STRING, {.str = "/dev/tw2865dev"}, 0, 0, DEC},
    { NULL }
};

static const BVClass his3515_class = {
    .class_name         = "his3515 system",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_SYSTEM,
};

BVSystem bv_his3515_system = {
    .name               = "his3515",
    .type               = BV_SYSTEM_TYPE_HIS3515,
    .priv_data_size     = sizeof(His3515SystemContext),
    .sys_init           = his3515_system_init,
    .sys_control        = his3515_system_control,
    .sys_exit           = his3515_system_exit,
    .priv_class         = &his3515_class,
};
