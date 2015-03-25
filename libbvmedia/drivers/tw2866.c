/*************************************************************************
    > File Name: tw2866.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月23日 星期一 15时31分02秒
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

#line 25 "tw2866.c"

#include <libbvmedia/bvmedia.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct Tw2866Driver {
    const BVClass *bv_class;
    int fd;
} Tw2866Driver;

static int tw2866_driver_open(BVMediaDriverContext *s)
{
    Tw2866Driver *driver = s->priv_data;
    bv_log(s, BV_LOG_DEBUG, "run here tw2866 driver open\n");
    driver->fd = open(s->filename, O_RDONLY);
    if (driver->fd < 0) {
        return BVERROR(EIO);
    }
    return 0;
}

static int audio_in_set_volume(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int audio_out_set_volume(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int video_in_set_imaging(BVMediaDriverContext *s, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    return BVERROR(ENOSYS);
}

static int tw2866_driver_control(BVMediaDriverContext *s, enum BVMediaDriverMessageType type, const BVControlPacket *pkt_in, BVControlPacket *pkt_out)
{
    int i = 0;
    struct {
        enum BVMediaDriverMessageType type;
        int (*control)(BVMediaDriverContext *s, const BVControlPacket *, BVControlPacket *);
    } driver_control[] = {
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_IN_VOLUME, audio_in_set_volume},
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_AUDIO_OUT_VOLUME, audio_out_set_volume},
        { BV_MEDIA_DRIVER_MESSAGE_TYPE_VIDEO_IN_IMAGING, video_in_set_imaging},
    };
    for (i = 0; i < BV_ARRAY_ELEMS(driver_control); i++) {
        if (driver_control[i].type == type)
           return driver_control[i].control(s, pkt_in, pkt_out); 
    }

    bv_log(s, BV_LOG_ERROR, "Not Support This command \n");
    return BVERROR(ENOSYS);
}

static int tw2866_driver_close(BVMediaDriverContext *s)
{
    Tw2866Driver *driver = s->priv_data;
    if (driver->fd) {
        close(driver->fd);
    }
    return 0;
}

#define OFFSET(x) offsetof(Tw2866Driver, x)
#define DEC BV_OPT_FLAG_DECODING_PARAM
static const BVOption options[] = {
    { NULL }
};
static const BVClass tw2866_class = {
    .class_name         = "tw2866 driver",
    .item_name          = bv_default_item_name,
    .option             = options,
    .version            = LIBBVUTIL_VERSION_INT,
    .category           = BV_CLASS_CATEGORY_DEVICE,
};

BVMediaDriver bv_tw2866_driver = {
    .name               = "tw2866",
    .type               = BV_MEDIA_DRIVER_TYPE_VIDEO | BV_MEDIA_DRIVER_TYPE_AUDIO,
    .priv_class         = &tw2866_class,
    .priv_data_size     = sizeof(Tw2866Driver),
    .driver_open        = tw2866_driver_open,
    .driver_control     = tw2866_driver_control,
    .driver_close       = tw2866_driver_close,
};
