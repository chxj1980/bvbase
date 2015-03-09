/*************************************************************************
    > File Name: disk.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月09日 星期一 09时13分11秒
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
 * Copyright (C) albert@BesoVideo, 2015
 */

#line 25 "disk.c"

#include <bvfs.h>

#include "bvdevice.h"

typedef struct DiskDeviceContext {
    int max_files;
} DiskDeviceContext;

static int disk_device_open(BVDeviceContext *h)
{
    if (bvfs_init(1, 0) < 0) {
        bv_log(h, BV_LOG_ERROR, "bvfs init error\n");
    }
    return 0;
}
static int disk_device_probe(BVDeviceContext *h, const char *args)
{
    if (strcmp("disk_dev", args) == 0) {
        return 100;
    }
    return 0;
}

static int disk_device_close(BVDeviceContext *h)
{
    return 0;
}

static int disk_format(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return 0;
}

static int disk_search_file(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return 0;
}

static int disk_device_control(BVDeviceContext *h, enum BVDeviceMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVDeviceMessageType type;
        int (*control)(BVDeviceContext *h, const BVControlPacket *, BVControlPacket *);
    } disk_control[] = {
        { BV_DEV_MESSAGE_TYPE_FORMAT_DISK, disk_format},
        { BV_DEV_MESSAGE_TYPE_SEARCH_FILE, disk_search_file},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(disk_control); i++) {
        if (disk_control[i].type == type)
           return disk_control[i].control(h, pkt_in, pkt_out); 
    }
    bv_log(h, BV_LOG_ERROR, "Not Support This command \n");
    return BVERROR(ENOSYS);
}

#define OFFSET(x) offsetof(DiskDeviceContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { "max_files", "", OFFSET(max_files), BV_OPT_TYPE_INT, {.i64 =  1024}, INT_MIN, INT_MAX, DEC },
    { NULL }
};

static const BVClass disk_class = {
    .class_name     = "disk device",
    .item_name      = bv_default_item_name,
    .option         = options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_DEVICE,
};

BVDevice bv_disk_device = {
    .name           = "disk",
    .type           = BV_DEVICE_TYPE_HRDSK,
    .priv_data_size = sizeof(DiskDeviceContext),
    .dev_open       = disk_device_open,
    .dev_probe      = disk_device_probe,
    .dev_close      = disk_device_close,
    .dev_control    = disk_device_control,
    .priv_class     = &disk_class,
};
