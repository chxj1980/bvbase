/*************************************************************************
    > File Name: exDriver.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月24日 星期二 13时19分21秒
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

#include <libbvutil/bvutil.h>
#include <libbvmedia/bvmedia.h>

int main(int argc, const char *argv[])
{
    BVMediaDriverContext *ctx = NULL;
    bv_log_set_level(BV_LOG_DEBUG);

    bv_media_register_all();

    if (bv_media_driver_open(&ctx, "/dev/tw2865dev", "tw2866", NULL, NULL) < 0) {
        bv_log(NULL, BV_LOG_ERROR, "open media driver error\n");
        return BVERROR(EIO);
    }
    bv_log(ctx, BV_LOG_INFO, "run here %s %d %s \n", __FILE__, __LINE__, ctx->driver->name);
    bv_media_driver_close(&ctx);
    return 0;
}
