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

#include "bvconfig.h"

struct LocalConfigContext {

};

BVConfig bv_local_config = {
    .name = "local",
    .type = BV_CONFIG_TYPE_LOCAL,
    .priv_data_size = sizeof(struct LocalConfigContext),
};
