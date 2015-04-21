/*************************************************************************
    > File Name: version.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月14日 星期二 10时14分12秒
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

#ifndef BV_FILTER_VERSION_H
#define BV_FILTER_VERSION_H

#include <libbvutil/version.h>

#define LIBBVFILTER_VERSION_MAJOR 0
#define LIBBVFILTER_VERSION_MINOR  2
#define LIBBVFILTER_VERSION_MICRO 0

#define LIBFILTER_VERSION_INT BV_VERSION_INT(LIBBVFILTER_VERSION_MAJOR, \
                                               LIBBVFILTER_VERSION_MINOR, \
                                               LIBBVFILTER_VERSION_MICRO)
#define LIBFILTER_VERSION     BV_VERSION(LIBBVFILTER_VERSION_MAJOR, \
                                           LIBBVFILTER_VERSION_MINOR, \
                                           LIBBVFILTER_VERSION_MICRO)
#define LIBFILTER_BUILD       LIBBVFILTER_VERSION_INT

#define LIBFILTER_IDENT       "Lbvf" BV_STRINGIFY(LIBBVFILTER_VERSION)


#endif /* end of include guard: BV_FILTER_VERSION_H */
