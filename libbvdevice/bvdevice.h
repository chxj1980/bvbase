/*************************************************************************
    > File Name: bvdevice.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年09月25日 星期四 09时44分50秒
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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#ifndef BV_DEVICE_H
#define BV_DEVICE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <libbvutil/bvutil.h>
#include <libbvutil/opt.h>
#include <libbvutil/dict.h>
#include <libbvutil/packet.h>

#include <libbvconfig/common.h>
#include <time.h>

enum BVDeviceType {
    BV_DEVICE_TYPE_NONE = -1,
    BV_DEVICE_TYPE_SERIAL,
    BV_DEVICE_TYPE_NADN,
    BV_DEVICE_TYPE_GPS,
    BV_DEVICE_TYPE_PTZ,
    BV_DEVICE_TYPE_WLAN,
    BV_DEVICE_TYPE_MBLNT,    //mobile network 3/4G网络
    BV_DEVICE_TYPE_HRDSK,    //hard disk

    BV_DEVICE_TYPE_ONVIF_DEVICE,
    BV_DEVICE_TYPE_ONVIF_PTZ,

    BV_DEVICE_TYPE_UNKNOWN
};

enum BVDeviceStatus {
    BV_DEVICE_STATUS_NONE = -1,
    BV_DEVICE_STATUS_DISK_INIT_OK = 0,
    BV_DEVICE_STATUS_DISK_INIT_ERROR = 1,
    BV_DEVICE_STATUS_DISK_FORMAT_ERROR = 2,

    BV_DEVICE_STATUS_IPC_OFF_LINE = 0x100,
    BV_DEVICE_STATUS_IPC_ON_LINE,
};

enum BVDeviceMessageType {
    BV_DEV_MESSAGE_TYPE_NONE = -1,

    //PTZ Message Type
    BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE, //移动PTZ            BVPTZContinuousMove  NULL
    BV_DEV_MESSAGE_TYPE_PTZ_STOP,            //停止PTZ            BVPTZStop             NULL
    BV_DEV_MESSAGE_TYPE_PTZ_SET_PRESET,     //                    BVPTZPreset
    BV_DEV_MESSAGE_TYPE_PTZ_GOTO_PRESET,    //                    BVPTZGotoPreset
    BV_DEV_MESSAGE_TYPE_PTZ_REMOVE_PRESET,  //                    BVPTZPreset

    BV_DEV_MESSAGE_TYPE_DISK_START = 0x200, //磁盘属性开始
    BV_DEV_MESSAGE_TYPE_FORMAT_DISK,        //格式化磁盘           BVDiskDevice NULL
    BV_DEV_MESSAGE_TYPE_SEARCH_FILE,        //查询文件             BVSearchFileConditions BVFileInfo
    BV_DEV_MESSAGE_TYPE_GET_DISK_COUNT,     //获取磁盘个数
    BV_DEV_MESSAGE_TYPE_GET_DISK_STATUS,
    BV_DEV_MESSAGE_TYPE_GET_DISK_INFO,      //获取磁盘信息         BVDiskDevice BVDiskDeviceInfo
    BV_DEV_MESSAGE_TYPE_IPC_START = 0x400,  //IPC属性开始
    BV_DEV_MESSAGE_TYPE_SEARCH_IPC,         //搜索OnvifIPC         NULL BVMobileDevice
    BV_DEV_MESSAGE_TYPE_DETECT_IPC,         //检测IPC设备状态      BVMobileDevice NULL
    BV_DEV_MESSAGE_TYPE_UNKNOWN
};

typedef struct _BVDiskDevice {
    int index;
    int type;               //磁盘类型 硬盘或SD卡
    char name[128];
} BVDiskDevice;

typedef struct _BVDiskDeviceInfo {
    BVDiskDevice device;
    int disk_size;          //MB
    int free_size;          //MB
    int overlay_size;       //覆盖MB
    int disk_status;        //磁盘状态  1 bad disk 0 normal
} BVDiskDeviceInfo;

typedef struct _BVSearchFileConditions {
    time_t start_time;
    time_t end_time;
    uint8_t channel_id;
    uint8_t file_type;
    uint32_t storage_type;
} BVSearchFileConditions;

typedef struct _BVFileInfo {
    char name[255];
    time_t start_time;
    time_t end_time;
    uint8_t channel_id;
    uint8_t file_type;
    uint8_t disk_id;
    uint32_t storage_type;
    uint64_t file_size;
} BVFileInfo;

typedef struct _BVDeviceContext {
    const BVClass *bv_class;
    struct _BVDevice *device;
    void *priv_data;
    size_t buffer_size;
    char url[1024];     //url likes ptz://dev/ttyusb0 get two inmediaion DeviceType and DeviceName
    uint8_t *buffer;
    uint8_t *buffer_ptr;
    uint8_t *buffer_end;
} BVDeviceContext;

#define BV_DEVICE_FLAG_NOOPEN          0x0001
typedef struct _BVDevice {
    const char *name;
    enum BVDeviceType type;
    const BVClass *priv_class;
    int priv_data_size;
    int flags;
    struct _BVDevice *next;
    int     (*dev_open)( BVDeviceContext *h);
    int     (*dev_read)( BVDeviceContext *h, unsigned char *buf, size_t size);
    int     (*dev_write)(BVDeviceContext *h, const unsigned char *buf, size_t size);
    int     (*dev_probe)(BVDeviceContext *h, const char *args);
    int64_t (*dev_seek)( BVDeviceContext *h, int64_t pos, int whence);
    int     (*dev_control)(BVDeviceContext *h, enum BVDeviceMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);
    int     (*get_fd)(BVDeviceContext *h);
    int     (*dev_close)(BVDeviceContext *h);
} BVDevice;

void bv_device_register_all(void);

int bv_device_register(BVDevice *dev);

BVDevice * bv_device_next(BVDevice *dev);

BVDevice *bv_device_find_device(enum BVDeviceType device_type);

BVDeviceContext *bv_device_context_alloc(void);

void bv_device_context_free(BVDeviceContext *devctx);

/**
 * @Synopsis        open a device
 *
 * @Param h         *h can be NULL if not the internal will malloc memory
 * @Param url       device url 
 * @Param dev       device handle can be NULL 
 * @Param options   device private field
 *                  BVDictionary *options = NULL;
 *                  bv_dict_set(&options, "baud_rate", "51200", 0);
 *                  .....
 *                  after bv_device_open() must free with
 *                  bv_dict_free(&options);
 *
 * @Returns         <0 fail ==0 success
 */
int bv_device_open(BVDeviceContext **h, const char *url, BVDevice *dev, BVDictionary **options);

int bv_device_read(BVDeviceContext *h, void *buf, size_t size);

int bv_device_write(BVDeviceContext *h, const void *buf, size_t size);

int64_t bv_device_seek(BVDeviceContext *h, int64_t pos, int whence);

int bv_device_control(BVDeviceContext *h, enum BVDeviceMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out);

int bv_device_get_fd(BVDeviceContext *h);

int bv_device_close(BVDeviceContext **h);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BV_DEVICE_H */

