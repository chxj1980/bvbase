#ifndef INCLUDE_DRV_H
#define INCLUDE_DRV_H

int BSCHGetConfig(int iViDev, int iViChn, int* iBright, int* iSaturation, int* iContrast, int* iHue);
int BSCHSetConfig(int iViDev, int iViChn, int iBright, int iSaturation, int iContrast, int iHue);
int AudioSampleSet(int iSample);
int AudioInVolumeSet(int iAiDev, int iAiChn, int iVolume);
int AudioOutVolumeSet(int iAoDev, int iAoChn, int iVolume);

#define VIDEO_STD_AUTO      0
#define VIDEO_STD_NTSC      1
#define VIDEO_STD_PAL       2
#define VIDEO_STD_UNKNOWN   -1
int VideoInSetStd(int iViDev, int iViChn, int mode);
int VideoInGetStd(int iViDev, int iViChn, int *mode);

#endif
