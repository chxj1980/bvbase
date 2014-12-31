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

enum BVDeviceType {
	BV_DEVICE_TYPE_NONE = -1,
	BV_DEVICE_TYPE_SERIAL,
	BV_DEVICE_TYPE_NADN,
	BV_DEVICE_TYPE_GPS,
	BV_DEVICE_TYPE_PTZ,
	BV_DEVICE_TYPE_WLAN,
	BV_DEVICE_TYPE_MBLNT,	//mobile network 3/4G网络
	BV_DEVICE_TYPE_HRDSK,	//hard disk

    BV_DEVICE_TYPE_ONVIF_DEVICE,
	BV_DEVICE_TYPE_ONVIF_PTZ,

	BV_DEVICE_TYPE_UNKNOWN
};

enum BVDeviceMessageType {
	BV_DEV_MESSAGE_TYPE_NONE = -1,

    //PTZ Message Type
    BV_DEV_MESSAGE_TYPE_PTZ_CONTINUOUS_MOVE,
#if 0
    BV_DEV_MESSAGE_TYPE_PTZ_LEFT,
    BV_DEV_MESSAGE_TYPE_PTZ_LEFT_DOWN,
    BV_DEV_MESSAGE_TYPE_PTZ_DOWN,
    BV_DEV_MESSAGE_TYPE_PTZ_RIGHT_DOWN,
    BV_DEV_MESSAGE_TYPE_PTZ_RIGHT,
    BV_DEV_MESSAGE_TYPE_PTZ_RIGHT_UP,
    BV_DEV_MESSAGE_TYPE_PTZ_UP,
    BV_DEV_MESSAGE_TYPE_PTZ_LEFT_UP,
    BV_DEV_MESSAGE_TYPE_PTZ_ZOOM_IN,
    BV_DEV_MESSAGE_TYPE_PTZ_ZOOM_OUT,
#endif
    BV_DEV_MESSAGE_TYPE_PTZ_STOP,
    BV_DEV_MESSAGE_TYPE_PTZ_SET_PRESET,
    BV_DEV_MESSAGE_TYPE_PTZ_GOTO_PRESET,
    BV_DEV_MESSAGE_TYPE_PTZ_REMOVE_PRESET,

    BV_DEV_MESSAGE_TYPE_FORMAT_DISK,     //格式化磁盘

	BV_DEV_MESSAGE_TYPE_UNKNOWN
};

enum BVMobileDeviceType {
    BV_MOBILE_DEVICE_TYPE_NONE = 0,
    BV_MOBILE_DEVICE_TYPE_NVS = (1 << 0),
    BV_MOBILE_DEVICE_TYPE_NVT = (1 << 1),
    BV_MOBILE_DEVICE_TYPE_NVA = (1 << 2),
    BV_MOBILE_DEVICE_TYPE_NVD = (1 << 3),

    BV_MOBILE_DEVICE_TYPE_UNKNOWN,
};

typedef struct _BVMobileDevice {
    enum BVMobileDeviceType type;
    char url[1024];
    char user[64];
    char pswd[64];
    int  timeout;
} BVMobileDevice;

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
    int     (*dev_scan)( BVDeviceContext *h, BVMobileDevice *device, int *max_ret);
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
 * @Param dev       device handle can be NULL ,But Not Support Now FIXME
 * @Param url       device url 
 * @Param options   device private field
 *                  BVDictionary *options = NULL;
 *                  bv_dict_set(&options, "baud_rate", "51200", 0);
 *                  .....
 *                  after bv_device_open() must free with
 *                  bv_dict_free(&options);
 *
 * @Returns         <0 fail ==0 success
 */
int bv_device_open(BVDeviceContext **h, BVDevice *dev, const char *url, BVDictionary **options);

int bv_device_scan(BVDeviceContext *h, BVMobileDevice *device, int *max_ret);

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

