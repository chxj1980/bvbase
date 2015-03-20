/*************************************************************************
    > File Name: x86.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月12日 星期四 14时35分06秒
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

#line 25 "x86.c"

#include "bvsystem.h"

typedef struct X86SystemContext {
    const BVClass *bv_class;
} X86SystemContext;

static int x86_system_init(BVSystemContext *s)
{
    bv_log(s, BV_LOG_ERROR, "run here %s %d\n", __FILE__, __LINE__);
    return 0;
}

static int x86_system_exit(BVSystemContext *s)
{
    bv_log(s, BV_LOG_ERROR, "run here %s %d\n", __FILE__, __LINE__);
    return 0;
}

#define OFFSET(x) offsetof(X86SystemContext, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};

static const BVClass x86_class = {
    .class_name         = "x86 system",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_SYSTEM,
};

BVSystem bv_x86_system = {
    .name               = "x86",
    .type               = BV_SYSTEM_TYPE_X86,
    .priv_data_size     = sizeof(X86SystemContext),
    .sys_init           = x86_system_init,
    .sys_exit           = x86_system_exit,
    .priv_class         = &x86_class,
};
