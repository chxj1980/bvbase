/*************************************************************************
    > File Name: version.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年12月15日 星期一 10时46分54秒
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

#ifndef BV_SERVER_VERSION_H
#define BV_SERVER_VERSION_H

#include <libbvutil/version.h>

#define LIBBVSERVER_VERSION_MAJOR 0
#define LIBBVSERVER_VERSION_MINOR  0
#define LIBBVSERVER_VERSION_MICRO 1

#define LIBSERVER_VERSION_INT BV_VERSION_INT(LIBBVSERVER_VERSION_MAJOR, \
                                               LIBBVSERVER_VERSION_MINOR, \
                                               LIBBVSERVER_VERSION_MICRO)
#define LIBSERVER_VERSION     BV_VERSION(LIBBVSERVER_VERSION_MAJOR, \
                                           LIBBVSERVER_VERSION_MINOR, \
                                           LIBBVSERVER_VERSION_MICRO)
#define LIBSERVER_BUILD       LIBBVSERVER_VERSION_INT

#define LIBSERVER_IDENT       "Lbvsvr" BV_STRINGIFY(LIBBVSERVER_VERSION)


#endif /* end of include guard: BV_SERVER_VERSION_H */

