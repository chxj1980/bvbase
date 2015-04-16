/*************************************************************************
    > File Name: exUrl.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月23日 星期五 17时40分37秒
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

#include <libbvprotocol/bvio.h>
#include <libbvprotocol/bvurl.h>
#include <libbvutil/bvutil.h>
#include <libbvutil/log.h>
#include <libbvutil/opt.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, const char *argv[])
{
    BVIOContext *s = NULL; 
    BVDictionary *opn = NULL;
    char filename[128] = { 0 };
    time_t start_time = time(NULL);
    char buf[1024] ;
    int size;
    bv_log_set_level(BV_LOG_DEBUG);
    bv_protocol_register_all();
#if 0
    bv_dict_set_int(&opn, "file_type", 1, 0);
    bv_dict_set_int(&opn, "storage_type", 2, 0);
    bv_dict_set_int(&opn, "channel_num", 2, 0);
    sprintf(filename, "%s", "bvfs://00_20120412_031132_");
    sprintf(filename + strlen(filename), "%ld.dav", time(NULL));
    if (bv_url_open(&s, filename, BV_IO_FLAG_WRITE, NULL, &opn)) {
        bv_dict_free(&opn);
        bv_log(NULL, BV_LOG_ERROR, "open files error\n");
        return -1;
    }
    while (time(NULL) - start_time < 300) {
        if (bv_url_write(s, (const uint8_t *)buf, 1024) != 1024) {
            bv_log(s, BV_LOG_ERROR, "write file error\n");
            goto closed;
        }
        usleep(100);
    }
#endif 
    sprintf(filename, "%s", "bvfs:///02_00/pic/00_20150317_131125.jpg");
    if (bv_io_open(&s, filename, BV_IO_FLAG_READ, NULL, NULL) < 0 ) {
        bv_log(NULL, BV_LOG_ERROR, "open files error\n");
        return -1;
    }
    int fd = open("a.jpg", O_RDWR|O_CREAT, 0755);
    if (fd < 0) {
        bv_log(s, BV_LOG_ERROR, "open file error\n");
    }

    while (!bv_io_feof(s)) {
        if ((size = bv_io_read(s, buf, 1024)) <= 0) {
            break;
        } else {
            bv_log(s, BV_LOG_ERROR, "read size %d\n", size);
            if (fd >= 0) {
                write(fd, buf, size); 
            }
        }
    }
    if (fd >= 0) {
        close(fd);
    }
closed:
    //bv_dict_free(&opn);
    bv_io_close(s);
    return 0;
}
