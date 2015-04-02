#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "drv.h"
#include "tw28xx.h"
#include "tlv320aic23.h"
#include <errno.h>
#include <libbvutil/bvutil.h>
#include <libbvutil/log.h>

#define TW28XX_DEV_PATH "/dev/tw2865dev"
#define AIC23_DEV_PATH  "/dev/tlv320aic23"

//tw2867    4*video_adc + 5*audio_adc + audio_dac
//tlv320aic 2*audio_adc + 2*audio_dac
/***********************************/
/* hi3515.videv0 <----- tw2867.VD1 */
/* hi3515.aidev0 <----- tw2867.IIS */
/* hi3515.aidev1 <----- aic23.IIS  */
/* hi3515.aodev0 -----> aic23.IIS  */
/***********************************/

int BSCHGetConfig(int iViDev, int iViChn, int* iBright, int* iSaturation, int* iContrast, int* iHue)
{ 
    tw286x_image_adjust stParam;
    int iDevFd = open(TW28XX_DEV_PATH, O_RDONLY);
    if (iDevFd < 0) {
        return -1;
    }

    BBCLEAR_STRUCT(stParam);
    stParam.chip = iViDev;
    stParam.chn = iViChn;

    if (ioctl(iDevFd, TW286X_GET_IMAGE_ADJUST, &stParam)) {
        close(iDevFd);
        return -1;
    }

    *iBright = stParam.brightness;
    *iSaturation = stParam.saturation;
    *iContrast = stParam.contrast;
    *iHue = stParam.hue;

    close(iDevFd);

    return 0;
}


int BSCHSetConfig(int iViDev, int iViChn, int iBright, int iSaturation, int iContrast, int iHue)
{
    tw286x_image_adjust stParam;
    int iDevFd = open(TW28XX_DEV_PATH, O_RDONLY);
    if (iDevFd < 0) {
        return -1;
    }

    BBCLEAR_STRUCT(stParam);
    stParam.chip = iViDev;
    stParam.chn = iViChn;
    stParam.brightness = iBright;
    stParam.saturation = iSaturation;
    stParam.contrast = iContrast;
    stParam.hue = iHue;
    stParam.item_sel = TW286X_SET_BRIGHT | TW286X_SET_SATURATION | TW286X_SET_CONTRAST | TW286X_SET_HUE;

    if (ioctl(iDevFd, TW286X_SET_IMAGE_ADJUST, &stParam)) {
        close(iDevFd);
        return -1;
    }
    close(iDevFd);

    return 0;
}

int AudioSampleSet(int iSample)
{
    int fd;
    int ret;
    tw286x_audio_samplerate samplerate;
        
    fd = open(TW28XX_DEV_PATH, O_RDONLY);
    if (fd < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", TW28XX_DEV_PATH);
        return -1;
    }
    
    switch (iSample) {
        case 8000:
            samplerate = TW286X_SAMPLE_RATE_8000;
            break;
        case 16000:
            samplerate = TW286X_SAMPLE_RATE_16000;
            break;
        case 32000:
            samplerate = TW286X_SAMPLE_RATE_32000;
            break;
        case 44100:
            samplerate = TW286X_SAMPLE_RATE_44100;
            break;
        case 48000:
            samplerate = TW286X_SAMPLE_RATE_48000;
            break;
        default:
            bv_log(NULL, BV_LOG_ERROR, "not support iSample %d\n", iSample);
            return -1;
    }
        
    if (ioctl(fd, TW286X_SET_SAMPLE_RATE, &samplerate)) {
        bv_log(NULL, BV_LOG_ERROR, "ioctl TW286X_SET_SAMPLE_RATE err !!! \n");
        close(fd);
        return -1;
    }

    bv_log(NULL, BV_LOG_INFO, "ioctl TW286X_SET_SAMPLE_RATE[%d] success.\n", iSample);
    close(fd);

    if (access(AIC23_DEV_PATH, F_OK) == 0) {
        //configure tlv320aic23 codec
        fd = open(AIC23_DEV_PATH, O_RDONLY);
        if (fd < 0) {
            bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", AIC23_DEV_PATH);
            return -1;
        }

        ret = ioctl(fd, AIC23IOC_S_SAMPLE_RATE, &iSample);
        if (ret < 0) {
            bv_log(NULL, BV_LOG_ERROR, "ioctl AIC23IOC_S_SAMPLE_RATE err !!!\n");
            close(fd);
            return -1;
        }

        close(fd);
        bv_log(NULL, BV_LOG_INFO, "ioctl AIC23IOC_S_SAMPLE_RATE(%d) success.\n", iSample);
    }
    
    return 0;
}

/*
 * 0 <= iVolume <= 100
 * */
int AudioInVolumeSet(int iAiDev, int iAiChn, int iVolume)
{
    int ret = 0;
    int cmd;
    int fd;
    if (iAiDev == 1 && access(AIC23_DEV_PATH, F_OK) == 0) {
        //configure tlv320aic23 codec
        
        fd = open(AIC23_DEV_PATH, O_RDONLY);
        if (fd < 0) {
            bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", AIC23_DEV_PATH);
            return -1;
        }

        if (iAiChn == 0)
            cmd = AIC23IOC_S_AD_LEFT_VOLUME;
        else
            cmd = AIC23IOC_S_AD_RIGHT_VOLUME;

        ret = ioctl(fd, cmd, &iVolume);
        if (ret < 0) {
            bv_log(NULL, BV_LOG_ERROR, "ioctl AIC23IOC_S_AD_VOLUME err !!!\n");
        }
        close(fd);
    } else {
        tw286x_audio_in_volume  involume;
        fd = open(TW28XX_DEV_PATH, O_RDONLY);
        if (fd < 0) {
            bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", TW28XX_DEV_PATH);
            return -1;
        }

        involume.chip   = 0;//iAiDev;
        involume.chn    = iAiDev * 4 + iAiChn;
        involume.volume = iVolume * 15 / 100;

        ret = ioctl(fd, TW286X_SET_AUDIO_IN_VOLUME, &involume);
        if (ret < 0) {
            bv_log(NULL, BV_LOG_ERROR, "ioctl TW286X_SET_AUDIO_IN_VOLUME err !!!\n");
        }
        close(fd);
    }
    return ret;
}

/*
 * 0 <= iVolume <= 100
 * */
int AudioOutVolumeSet(int iAoDev, int iAoChn, int iVolume)
{
    int ret = 0;
    int fd, cmd;
    if (iAoDev == 0 && access(AIC23_DEV_PATH, F_OK) == 0) {
        //configure tlv320aic23 codec
        
        fd = open(AIC23_DEV_PATH, O_RDONLY);
        if (fd < 0) {
            bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", AIC23_DEV_PATH);
            return -1;
        }

        if (iAoChn == 0)
            cmd = AIC23IOC_S_DA_LEFT_VOLUME;
        else
            cmd = AIC23IOC_S_DA_RIGHT_VOLUME;

        ret = ioctl(fd, cmd, &iVolume);
        if (ret < 0) {
            bv_log(NULL, BV_LOG_ERROR, "ioctl AIC23IOC_S_DA_VOLUME err !!!\n");
        }
        close(fd);
    } else {
        tw286x_audio_da_volume  davolume;
        
        fd = open(TW28XX_DEV_PATH, O_RDONLY);
        if (fd < 0)
        {
            bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", TW28XX_DEV_PATH);
            return -1;
        }

        davolume.chip   = iAoDev;
        davolume.volume = iVolume * 15 / 100;

        if (ioctl(fd, TW286X_SET_AUDIO_DA_VOLUME, &davolume)) {
            bv_log(NULL, BV_LOG_ERROR, "ioctl TW286X_SET_AUDIO_DA_VOLUME err !!!\n");
        }
        close(fd);
    }
    return ret;
}

/*
 * Video input set standard (AUTO,NTSC,PAL)
 * */
int VideoInSetStd(int iViDev, int iViChn, int mode)
{
    int fd;

    tw28xxstr tw28xxctx = {0};
    tw28xxctx.ChipId    = iViDev;
    tw28xxctx.ChipType  = 6;//tw2867
    tw28xxctx.Register  = 0x0e + 0x10*iViChn;//standard selection reg
    tw28xxctx.Data      = 0x07;//auto detection

    if (VIDEO_STD_NTSC == mode)
        tw28xxctx.Data = 0x00;
    else if (VIDEO_STD_PAL == mode)
        tw28xxctx.Data = 0x01;
    
    fd = open(TW28XX_DEV_PATH, O_RDONLY);
    if (fd < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", TW28XX_DEV_PATH);
        return -1;
    }

    if (ioctl(fd, TW28XXCMD_WRITE_REG, &tw28xxctx)) {
        bv_log(NULL, BV_LOG_ERROR, "ioctl TW28XXCMD_WRITE_REG err !!!\n");
        close(fd);
        return -1;
    }

    bv_log(NULL, BV_LOG_INFO, "videv = %d vichn = %d mode = %s\n", iViDev, iViChn, \
            mode==VIDEO_STD_NTSC?"ntsc":(mode==VIDEO_STD_PAL?"pal":"auto"));

    close(fd);
    return 0;
}

/*
 * Video input get standard (NTSC,PAL,UNKNOWN)
 * */
int VideoInGetStd(int iViDev, int iViChn, int *mode)
{
    int fd;
    unsigned char std;

    tw28xxstr tw28xxctx = {0};
    tw28xxctx.ChipId    = iViDev;
    tw28xxctx.ChipType  = 6;//tw2867
    tw28xxctx.Register  = 0x0e + 0x10*iViChn;//standard selection reg
    tw28xxctx.Data      = 0x00;

    fd = open(TW28XX_DEV_PATH, O_RDONLY);
    if (fd < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open %s fail\n", TW28XX_DEV_PATH);
        return -1;
    }

    if (ioctl(fd, TW28XXCMD_READ_REG, &tw28xxctx) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "ioctl TW28XXCMD_READ_REG err !!!\n");
        close(fd);
        return -1;
    }

    std = (tw28xxctx.Data >> 4) & 7;
    if (0 == std)
        *mode = VIDEO_STD_NTSC;
    else if (1 == std)
        *mode = VIDEO_STD_PAL;
    else
        *mode = VIDEO_STD_UNKNOWN;

    close(fd);
    return 0;
}
