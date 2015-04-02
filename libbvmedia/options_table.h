/*************************************************************************
    > File Name: options_table.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月16日 星期二 11时00分38秒
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

#ifndef BV_OPTIONS_TABLE_H
#define BV_OPTIONS_TABLE_H

#include <limits.h>
#include <libbvutil/opt.h>

#include "bvmedia.h"

#define OFFSET(X) offsetof(BVDeviceContext, X)
#define E   BV_OPT_FLAG_ENCODING_PARAM

static const BVOption media_options[] = {

    {NULL}
};

static const BVOption driver_options[] = {

    {NULL}
};

#endif /* end of include guard: BV_OPTIONS_TABLE_H */
