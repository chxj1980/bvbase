/*************************************************************************
    > File Name: hisvmd.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月13日 星期五 16时28分11秒
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

#line 25 "hisvmd.c"

/**
 *  video move detect 视频移动侦测
 *
 */

BVInputMedia bv_hisvmd_demuxer = {
    .name               = "hisvmd",
#if 0
    .priv_data_size     = sizeof(HisAVEContext),
    .read_probe         = his_probe,
    .read_header        = his_read_header,
    .read_packet        = his_read_packet,
    .read_close         = his_read_close,
    .media_control      = his_media_control,
    .flags              = BV_MEDIA_FLAGS_NOFILE,
    .priv_class         = &his_class,
#endif
};
