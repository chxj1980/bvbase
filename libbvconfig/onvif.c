/*************************************************************************
    > File Name: onvif.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年10月30日 星期四 17时58分08秒
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
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvconfig.h"

struct OnvifConfigContext {
    const BVClass *bv_class;
    int timeout;
    char *user;
    char *passwd;
};

#define OFFSET(x) offsetof(struct OnvifConfigContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    {"timeout", "read write time out", OFFSET(timeout), BV_OPT_TYPE_INT, {.i64 =  -500000}, INT_MIN, INT_MAX, DEC},
    {"user", "user name", OFFSET(user), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {"passwd", "user password", OFFSET(passwd), BV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {NULL}
};

static const BVClass onvif_class = {
    .class_name     = "onvif config",
    .item_name      = bv_default_item_name,
    .option         = options,
    .version        = LIBBVUTIL_VERSION_INT,
    .category       = BV_CLASS_CATEGORY_CONFIG,
};

BVConfig bv_onvif_config = {
    .name = "onvif",
    .type = BV_CONFIG_TYPE_ONVIF,
    .flags = BV_CONFIG_FLAGS_NOFILE | BV_CONFIG_FLAGS_NETWORK,
    .priv_data_size = sizeof(struct OnvifConfigContext),
    .priv_class = &onvif_class,
};
