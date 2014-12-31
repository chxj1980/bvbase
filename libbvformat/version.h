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
 * You should hbve received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) albert@BesoVideo, 2014
 */

#ifndef BV_FORMAT_VERSION_H
#define BV_FORMAT_VERSION_H

#define LIBBVFORMAT_VERSION_MAJOR 0
#define LIBBVFORMAT_VERSION_MINOR  0
#define LIBBVFORMAT_VERSION_MICRO 1

#define LIBFORMAT_VERSION_INT BV_VERSION_INT(LIBBVFORMAT_VERSION_MAJOR, \
                                               LIBBVFORMAT_VERSION_MINOR, \
                                               LIBBVFORMAT_VERSION_MICRO)
#define LIBFORMAT_VERSION     BV_VERSION(LIBBVFORMAT_VERSION_MAJOR, \
                                           LIBBVFORMAT_VERSION_MINOR, \
                                           LIBBVFORMAT_VERSION_MICRO)
#define LIBFORMAT_BUILD       LIBBVFORMAT_VERSION_INT

#define LIBFORMAT_IDENT       "Lbvfmt" BV_STRINGIFY(LIBBVFORMAT_VERSION)


#endif /* end of include guard: BV_FORMAT_VERSION_H */

