/*************************************************************************
    > File Name: allsystems.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月14日 星期日 20时57分24秒
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

#include "bvsystem.h"
#include <config.h>

#define REGISTER_SYSTEM(X, x)                                         \
    {                                                                   \
        extern BVSystem bv_##x##_system;                           \
        if (BV_CONFIG_##X##_SYSTEM)                                      \
            bv_system_register(&bv_##x##_system);                \
    }

void bv_system_register_all(void)
{
    static int initialized;

    if (initialized)
        return;
    initialized = 1;

    REGISTER_SYSTEM(HIS3515, his3515);
}
