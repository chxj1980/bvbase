/*************************************************************************
    > File Name: exDecode.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月17日 星期二 13时14分34秒
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

#include <libbvmedia/bvmedia.h>
#include <libbvutil/bvutil.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>


int main(int argc, const char *argv[])
{
    BVMediaContext *mc = NULL; 
    BVDictionary *opn = NULL;
    BVStream *st = NULL;

    bv_media_register_all();
    bv_log_set_level(BV_LOG_DEBUG);
    bv_dict_set(&opn, "vtoken", "2/0", 0); 
    bv_dict_set(&opn, "atoken", "2/0", 0);

    if (bv_output_media_open(&mc, NULL, "dav", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open output media error\n");
        bv_dict_free(&opn);
        return 0;
    }
    st = bv_stream_new(mc, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_VIDEO;
    st = bv_stream_new(mc, NULL);
    st->codec->codec_type = BV_MEDIA_TYPE_AUDIO;

    if (bv_output_media_write_header(mc, NULL) < 0) {
        bv_log(mc, BV_LOG_ERROR, "write header error\n");
        goto close;
    }
close:
    bv_dict_free(&opn);
    bv_output_media_write_trailer(mc);
    bv_output_media_close(&mc);
    return 0;
}
