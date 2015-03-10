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
#include <libbvutil/bvstring.h>

#include "bvdevice.h"

typedef struct DiskDeviceContext {
    const BVClass *bv_class;
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
    bvfs_uninit();
    return 0;
}

static int disk_format(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    BVDiskDevice *disk = (BVDiskDevice *)pkt_in->data;

    return bvfs_format_disk(disk->index,disk->type);
}

static int disk_search_file(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    DiskDeviceContext *diskctx = h->priv_data;
    int ret = 0;
    int i;
    int temp_num = diskctx->max_files;
    BVFS_FILE_INFO *bvfile_info = NULL;
    BVFileInfo *file_info = NULL;
    
    BVSearchFileConditions *cond = (BVSearchFileConditions *)pkt_in->data;
    
    file_info = (BVFileInfo *)bv_mallocz(diskctx->max_files * sizeof(BVFileInfo));
    if (!file_info) {
        bv_log(h, BV_LOG_ERROR, "bvfile_info calloc error\n");
        return BVERROR(ENOMEM);
    }
    bvfile_info = (BVFS_FILE_INFO *)bv_mallocz(diskctx->max_files * sizeof(BVFS_FILE_INFO));
    if (!bvfile_info) {
        bv_log(h, BV_LOG_ERROR, "file_info calloc error\n");
        bv_free(file_info);
        return BVERROR(ENOMEM);
    }
    
    ret = bvfs_search_file(cond->channel_id, cond->start_time, cond->end_time,
            &temp_num, bvfile_info, cond->file_type, cond->storage_type);
    if (ret < 0) {
        bv_log(h, BV_LOG_ERROR, "search file  error\n");
        temp_num = 0;
    }
    for (i = 0; i < temp_num; i++) {
        bv_strlcpy(file_info[i].name, bvfile_info[i].file_name, sizeof(file_info[i].name));
        file_info[i].start_time = bvfile_info[i].start_time;
        file_info[i].end_time = bvfile_info[i].end_time;
        file_info[i].channel_id = bvfile_info[i].channel_id;
        file_info[i].file_type = bvfile_info[i].file_type;
        file_info[i].disk_id = bvfile_info[i].disk_id;
        file_info[i].storage_type = bvfile_info[i].storage_type;
        file_info[i].file_size = bvfile_info[i].file_size;
    }
    
    pkt_out->data = (void *)file_info;
    pkt_out->size = temp_num;
    bv_free(bvfile_info);
    bvfile_info = NULL;
    
    return ret;
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

BVDevice bv_disk_dev_device = {
    .name           = "disk_dev",
    .type           = BV_DEVICE_TYPE_HRDSK,
    .priv_data_size = sizeof(DiskDeviceContext),
    .dev_open       = disk_device_open,
    .dev_probe      = disk_device_probe,
    .dev_close      = disk_device_close,
    .dev_control    = disk_device_control,
    .priv_class     = &disk_class,
};
