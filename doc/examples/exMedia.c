/*************************************************************************
	> File Name: exMedia.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2015年01月05日 星期一 09时39分09秒
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

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libbvutil/bvutil.h>
#include <libbvmedia/bvmedia.h>

int main(int argc, const char *argv[])
{
    BVMediaContext *mc = NULL;
    BVDictionary *opn = NULL;
    bv_media_register_all(); 
    bv_log_set_level(BV_LOG_ERROR);
#if 1
    bv_dict_set(&opn, "user", "admin", 0);
    bv_dict_set(&opn, "passwd", "123456", 0);
    bv_dict_set(&opn, "token", "MainStream", 0);
    bv_dict_set(&opn, "timeout", "2", 0);
#else
    bv_dict_set(&opn, "token", "000", 0);
#endif
    if (bv_input_media_open(&mc, NULL, "onvifave://192.168.6.149:80", NULL, &opn) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open media error\n");
        return -1;
    }
    int i = 0;
    BVPacket pkt;
    int fd = open("xxx.264", O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open file error\n");
    }
    bv_log(mc, BV_LOG_INFO, "stream count %d\n", mc->nb_streams);
    bv_log(mc, BV_LOG_INFO, "codec time base %d/%d\n", mc->streams[0]->codec->time_base.den, mc->streams[0]->codec->time_base.num);
    bv_log(mc, BV_LOG_INFO, "stream time base %d/%d\n", mc->streams[0]->time_base.den, mc->streams[0]->time_base.num);
    bv_log(mc, BV_LOG_INFO, "gop size %d\n", mc->streams[0]->codec->gop_size);
    bv_log(mc, BV_LOG_INFO, "extradata size %d\n", mc->streams[0]->codec->extradata_size);
    bv_log(mc, BV_LOG_INFO, "codec ID %d\n", mc->streams[0]->codec->codec_id);
    bv_log(mc, BV_LOG_INFO, "video size %dX%d\n", mc->streams[0]->codec->width, mc->streams[0]->codec->height);
    while (i < 300) {
       bv_packet_init(&pkt); 
       if (bv_input_media_read(mc, &pkt) < 0)
           continue;
       bv_log(mc, BV_LOG_DEBUG, "pkt size %d pts %lld strindex %d\n", pkt.size, pkt.pts, pkt.stream_index);
       if (fd >= 0) {
         write(fd, pkt.data, pkt.size);
       }
       i ++;
       bv_packet_free(&pkt);
    }
    close(fd);
    bv_dict_free(&opn);
    bv_input_media_close(&mc);
    return 0;
}
