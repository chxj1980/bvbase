/*************************************************************************
    > File Name: bvfsproto.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月23日 星期五 09时28分15秒
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

#include "bvurl.h"

#include <bvfs.h>
#include <libbvutil/bvstring.h>

typedef struct BVFSContext {
    const BVClass *class;
    int file_type;
    int storage_type;
    int channel_num;
    BVFS_FILE *bvfs_handle;
} BVFSContext;

static int file_open(BVURLContext *h, const char *filename, int flags, BVDictionary **options)
{
    BVFSContext *bvfsctx = h->priv_data;

    bv_strstart(filename, "bvfs://", &filename);
    bv_log(h, BV_LOG_DEBUG, "file name %s\n", filename);
    bv_log(h, BV_LOG_DEBUG, "file_type %d storage_type %d channel_num %d\n", 
            bvfsctx->file_type, bvfsctx->storage_type, bvfsctx->channel_num);
    if (flags & BV_IO_FLAG_WRITE) {
        bvfsctx->bvfs_handle = bvfs_create(filename, bvfsctx->file_type, bvfsctx->storage_type, bvfsctx->channel_num);
    } else {
        bvfsctx->bvfs_handle = bvfs_open(filename, BVFS_READ_ONLY);
    }
    if (!bvfsctx->bvfs_handle) {
        bv_log(h, BV_LOG_ERROR, "bvfs open file error\n");
        return BVERROR(EIO);
    }
    return 0;
}

static int file_read(BVURLContext *h, uint8_t *buf, size_t size)
{
    BVFSContext *bvfsctx = h->priv_data;
    int rsize = 0;
    if (!bvfsctx->bvfs_handle) {
        return BVERROR(EINVAL);
    }
    rsize = bvfs_read(bvfsctx->bvfs_handle, (char *)buf, size);
    return rsize < 0 ? BVERROR(EIO): rsize;
}

static int file_write(BVURLContext *h, const uint8_t *buf, size_t size)
{
    BVFSContext *bvfsctx = h->priv_data;
    int wsize = 0;
    if (!bvfsctx->bvfs_handle) {
        return BVERROR(EINVAL);
    }
    wsize = bvfs_write(bvfsctx->bvfs_handle, (char *)buf, size);

    return wsize;
}

static int64_t file_seek(BVURLContext *h, int64_t pos, int whence)
{
    BVFSContext *bvfsctx = h->priv_data;
    if (!bvfsctx->bvfs_handle) {
        return BVERROR(EINVAL);
    }
    if (whence == BV_SEEK_SIZE) {
        return bvfs_get_file_size(bvfsctx->bvfs_handle);
    }
    return bvfs_seek(bvfsctx->bvfs_handle, pos, whence);
}

static int file_close(BVURLContext *h)
{
    BVFSContext *bvfsctx = h->priv_data;
    if (!bvfsctx->bvfs_handle) {
        return BVERROR(EINVAL);
    }
    return bvfs_close(bvfsctx->bvfs_handle);
}

#define OFFSET(x) offsetof(BVFSContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption bvfs_options[] = {
    { "file_type", "", OFFSET(file_type), BV_OPT_TYPE_INT, {.i64 = 0}, 0, 128, DEC},
    { "storage_type", "", OFFSET(storage_type), BV_OPT_TYPE_INT, {.i64 = 0}, 0, 128, DEC},
    { "channel_num", "", OFFSET(channel_num), BV_OPT_TYPE_INT, {.i64 = 0}, 0, 128, DEC},
    { NULL },
};

static const BVClass bvfs_class = {
    .class_name = "bvfs proto",
    .item_name  = bv_default_item_name,
    .option     = bvfs_options,
    .version    = LIBBVUTIL_VERSION_INT,
};

BVURLProtocol bv_bvfs_protocol = {
    .name               = "bvfs",
    .url_open           = file_open,
    .url_read           = file_read,
    .url_write          = file_write,
    .url_seek           = file_seek,
    .url_close          = file_close,
    .priv_data_size     = sizeof(BVFSContext),
    .priv_class         = &bvfs_class,
};
