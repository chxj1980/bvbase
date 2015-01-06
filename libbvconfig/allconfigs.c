/*************************************************************************
    > File Name: allconfigs.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年10月29日 星期三 11时07分20秒
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

#include "config.h"

#define REGISTER_CONFIG(X, x)                                         \
    {                                                                   \
        extern BVConfig bv_##x##_config;                           \
        if (BV_CONFIG_##X##_CONFIG)                                      \
            bv_config_register(&bv_##x##_config);                \
    }

void bv_config_register_all(void)
{
    static int initialized;

    if (initialized)
        return;
    initialized = 1;

    REGISTER_CONFIG(JSON, json);
    REGISTER_CONFIG(ONVIF, onvif);
}
