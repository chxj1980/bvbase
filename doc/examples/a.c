/*************************************************************************
    > File Name: a.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月12日 星期一 12时56分29秒
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

#define BUF_SIZE (1024 * 1024)

int main(int argc, const char *argv[])
{
    BVIOContext *s = NULL;
    BVDictionary *opn = NULL;
    uint8_t buf[BUF_SIZE];
    int read_size = 0;

//    bv_log_set_level(BV_LOG_DEBUG);
    bv_network_init();
    bv_protocol_register_all();
    int fd = open("a.jpg", O_RDWR|O_CREAT, 0755);
    if (fd < 0) {
        bv_log(s, BV_LOG_ERROR, "open file error\n");
    }

    if (argv[1] == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "usage %s url\n", argv[0]);
        return BVERROR(EINVAL);
    }
    while (1) {
        bv_dict_set_int(&opn, "multiple_requests", 1, 0);
        if (bv_io_open(&s, argv[1], BV_IO_FLAG_READ, NULL, &opn) < 0) {
            bv_log(s, BV_LOG_ERROR, "open file error\n");
            return BVERROR(EINVAL);
        }

         read_size = bv_io_read(s, buf, BUF_SIZE);
         if (read_size <= 0) {
            bv_log(s, BV_LOG_ERROR, "read file error\n");
            sleep(1);
//            break;
         } else {
            bv_log(s, BV_LOG_ERROR, "read size %d\n", read_size);
            if (fd >= 0) {
                write(fd, buf, read_size);
            }
         }
        if (fd >= 0) {
            close(fd);
        }
        bv_io_close(s);
    }
    return 0;
}
