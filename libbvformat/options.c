/*************************************************************************
	> File Name: options.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月16日 星期二 10时25分30秒
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

#include "bvformat.h"

/**
 *  @file
 *  Options definition for BVDeviceContext
 */

#include "options_table.h"

static const char * device_to_name(void *ptr)
{
    BVDeviceContext *dc = (BVDeviceContext *)ptr;
    if (dc->device)
        return dc->device->name;
    return "NULL";
}
static const BVClass bv_device_context_class = {
    .class_name     = "BVDeviceContext",
    .item_name      = device_to_name,
    .option         = device_options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       =   BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

static void bv_device_get_context_default(BVDeviceContext *device)
{
    device->bv_class = &bv_device_context_class;
    bv_opt_set_defaults(device);
}

BVDeviceContext *bv_device_alloc_context(void)
{
    BVDeviceContext *s = bv_mallocz(sizeof(BVDeviceContext));
    if (!s) {
        bv_log(NULL, BV_LOG_ERROR, "malloc BVDeviceContext error");
        return NULL;
    }
    bv_device_get_context_default(s);
    return s;
}

void bv_device_free_context(BVDeviceContext * devctx)
{
    if (!devctx) 
        return;
    bv_opt_free(devctx);
    if (devctx->device && devctx->device->priv_class && devctx->priv_data)
        bv_opt_free(devctx->priv_data);
    bv_freep(&devctx->priv_data);
    if (devctx->buffer)
        bv_free(devctx->buffer);
    bv_free(devctx);
    return; 
}

