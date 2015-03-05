/*************************************************************************
    > File Name: local.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月13日 星期二 10时56分35秒
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

#include <libbvutil/bvstring.h>

#include "bvconfig.h"

typedef struct _LocalContext {
    const BVClass *bv_class;
}LocalContext;

static int local_probe(BVConfigContext *h, BVProbeData *p)
{
    if (bv_strstart(p->filename, "local:", NULL))
        return BV_PROBE_SCORE_MAX;
    return 0;
}

#define OFFSET(x) offsetof(LocalContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};

static const BVClass local_class = {
    .class_name     = "local config",
    .item_name      = bv_default_item_name,
    .option         = options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_CONFIG,
};

BVConfig bv_local_config = {
    .name           = "local",
    .type           = BV_CONFIG_TYPE_LOCAL,
    .priv_data_size = sizeof(LocalContext),
    .priv_class     = &local_class,
    .config_probe   = local_probe,
};
