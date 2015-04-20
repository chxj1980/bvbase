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
    int status;
    int disk_count;
    int max_files;
} DiskDeviceContext;

/**
 *  初始化没检测到硬盘返回出错。
 *  检测到硬盘但硬盘文件系统损坏返回成功。
 */

static int disk_device_open(BVDeviceContext *h)
{
    DiskDeviceContext *diskctx = h->priv_data;
    if (bvfs_init(1, 0) < 0) {
        bv_log(h, BV_LOG_ERROR, "bvfs init error\n");
        diskctx->status |= BV_DEVICE_STATUS_DISK_INIT_ERROR;
    }
    if (bvfs_get_disk_num(&diskctx->disk_count) < 0 || !diskctx->disk_count) {
        bv_log(h, BV_LOG_ERROR, "bvfs get disk num error\n");
        return BVERROR(EIO);
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
    DiskDeviceContext *diskctx = h->priv_data;
    BVDiskDevice *disk = (BVDiskDevice *)pkt_in->data;
    int ret = 0;
    ret = bvfs_format_disk(disk->index,disk->type);
    if (ret < 0) {
        bv_log(h, BV_LOG_ERROR, "format disk error\n");
        diskctx->status |= BV_DEVICE_STATUS_DISK_FORMAT_ERROR;
    } else {
        diskctx->status &= ~BV_DEVICE_STATUS_DISK_FORMAT_ERROR;
        diskctx->status &= ~BV_DEVICE_STATUS_DISK_INIT_ERROR;
    }
    return ret;
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
    pkt_out->size = 0;
    pkt_out->data = NULL;

    if (diskctx->status) {
        bv_log(h, BV_LOG_ERROR, "disk init error cant search files\n");
        return BVERROR(ENOSYS);
    } 
    
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
        //bv_strlcpy(file_info[i].name, bvfile_info[i].file_name, sizeof(file_info[i].name));
        bv_sprintf(file_info[i].name, sizeof(file_info[i].name), "/%02d_%02d%s", bvfile_info[i].disk_id, 0, bvfile_info[i].file_name);
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

static int disk_get_count(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    DiskDeviceContext *diskctx = h->priv_data;
    if (!pkt_out) {
        return BVERROR(EINVAL);
    }
    pkt_out->data = bv_mallocz(sizeof(int));
    if (!pkt_out->data) {
        return BVERROR(ENOMEM);
    }
    *(int *)pkt_out->data = diskctx->disk_count;
    return 0;
}

static int disk_get_status(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    DiskDeviceContext *diskctx = h->priv_data;
    pkt_out->data = bv_mallocz(sizeof(int));
    if (!pkt_out->data) {
        return BVERROR(ENOMEM);
    }
    *(int *)pkt_out->data = diskctx->status;
    return 0;
}

static int save_disk_info(BVFS_DISK_INFO *disk_info, BVDiskDeviceInfo *bv_disk_info)
{
    bv_disk_info->device.index = disk_info->disk_id;
    bv_disk_info->device.type  = disk_info->disk_type;
    bv_strlcpy(bv_disk_info->device.name, disk_info->disk_name, sizeof(bv_disk_info->device.name));
    bv_disk_info->disk_size = disk_info->disk_total_size;
    bv_disk_info->free_size = disk_info->disk_free_size;
    bv_disk_info->overlay_size = disk_info->disk_overlay_size;
    bv_disk_info->disk_status = disk_info->bad_disk;
    return 0;
}

static int disk_get_single_info(DiskDeviceContext *diskctx, int index, BVControlPacket *pkt_out)
{
    BVDiskDeviceInfo *bv_disk_info = NULL;
    BVFS_DISK_INFO *disk_info = bv_mallocz(sizeof(BVFS_DISK_INFO));
    if (!disk_info)
        return BVERROR(ENOMEM);
    if (bvfs_get_disk_info(disk_info, index) < 0) {
        bv_log(diskctx, BV_LOG_ERROR, "get disk info error\n");
        bv_free(disk_info);
        return BVERROR(EIO);
    }
    bv_disk_info = bv_mallocz(sizeof(BVDiskDeviceInfo));
    if (!bv_disk_info) {
        bv_free(disk_info);
        return BVERROR(ENOMEM);
    }
    save_disk_info(disk_info, bv_disk_info);
    pkt_out->size = 1;
    pkt_out->data = bv_disk_info;
    bv_free(disk_info);
    return 0;
}

static int disk_get_all_info(DiskDeviceContext *diskctx, BVControlPacket *pkt_out)
{
    int count = diskctx->disk_count;
    BVDiskDeviceInfo *bv_disk_info = NULL;
    BVFS_DISK_INFO *disk_info = bv_mallocz(sizeof(BVFS_DISK_INFO) * diskctx->disk_count);
    int i = 0;
    if (!disk_info) {
        return BVERROR(ENOMEM);
    }
    if (bvfs_get_all_disk_info(disk_info, &count) < 0 || !count) {
        bv_log(diskctx, BV_LOG_ERROR, "get all disk info error\n");
        bv_free(disk_info);
        return BVERROR(EIO);
    }
    if (count != diskctx->disk_count) {
        bv_log(diskctx, BV_LOG_WARNING, "disk count changed\n");
    }
    bv_disk_info = bv_mallocz(sizeof(BVDiskDeviceInfo) * count);
    if (!bv_disk_info) {
        bv_free(disk_info);
        return BVERROR(ENOMEM);
    }
    for (i = 0; i < count; i++) {
        save_disk_info(disk_info + i, bv_disk_info + i);
    }
    bv_free(disk_info);
    pkt_out->size = count;
    pkt_out->data = bv_disk_info;
    return 0;
}

static int disk_get_info(BVDeviceContext *h, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    DiskDeviceContext *diskctx = h->priv_data;
    BVDiskDevice *disk = (BVDiskDevice *)pkt_in->data;
    if (!pkt_in || !pkt_out) {
        return BVERROR(EINVAL);
    }
    if (disk->index != -1) {
        return disk_get_single_info(diskctx, disk->index, pkt_out);
    }
    return disk_get_all_info(diskctx, pkt_out);
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
        { BV_DEV_MESSAGE_TYPE_GET_DISK_COUNT, disk_get_count},
        { BV_DEV_MESSAGE_TYPE_GET_DISK_STATUS, disk_get_status},
        { BV_DEV_MESSAGE_TYPE_GET_DISK_INFO,  disk_get_info},
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
    .class_name         = "disk device",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEVICE,
};

BVDevice bv_disk_dev_device = {
    .name               = "disk_dev",
    .type               = BV_DEVICE_TYPE_HRDSK,
    .priv_data_size     = sizeof(DiskDeviceContext),
    .dev_open           = disk_device_open,
    .dev_probe          = disk_device_probe,
    .dev_close          = disk_device_close,
    .dev_control        = disk_device_control,
    .priv_class         = &disk_class,
};
