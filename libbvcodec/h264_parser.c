/*************************************************************************
    > File Name: h264_parser.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年03月23日 星期一 11时28分34秒
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

#line 25 "h264_parser.c"

#include "bvcodec.h"
#include "h264.h"

typedef struct H264ParserContext {
    const BVClass *bv_class;
    uint8_t *buffer;
    uint8_t *buffer_ptr;
    uint8_t *buffer_end;
    uint32_t buffer_size;
    uint32_t size;
    uint8_t *sps[MAX_SPS_COUNT];
    uint8_t *pps[MAX_PPS_COUNT];
    uint8_t sps_num;
    uint8_t pps_num;
} H264ParserContext;

static int h264_parser_init(BVCodecParserContext *s)
{
    H264ParserContext *p = s->priv_data;
    p->buffer_size = MAX_MBPAIR_SIZE;
    bv_log(s, BV_LOG_DEBUG, "h264_parser_init\n");
    if (!(p->buffer = bv_mallocz(p->buffer_size))) {
        bv_log(s, BV_LOG_ERROR, "malloc H264 parser buffer error\n");
        return BVERROR(ENOMEM);
    }
    p->buffer_ptr = p->buffer;
    p->buffer_end = p->buffer + p->buffer_size;
    p->size = 0;
    return 0;
}

static int h264_parser_exit(BVCodecParserContext *s)
{
    H264ParserContext *p = s->priv_data;
    if (p->buffer) {
        bv_free(p->buffer);
    }
    bv_log(s, BV_LOG_DEBUG, "h264_parser_exit\n");
    return 0;
}

static int h264_find_start_code(const uint8_t *data, int size)
{
    int i = 0;
    if (size <= 4) 
        return -1;
    for (i = 0; i < size - 4; i++) {
        if ((data[i] == 0) && (data[i + 1] == 0) && (data[i + 2] == 0) && (data[i + 3] == 1)) {
            return i;
        }
    }
    return -1;
}

static int h264_find_nal_end(const uint8_t *data, int size)
{
    return 0;
}
/**
 *  FIXME
 *  RTP h264 组包
 */
static int h264_parser_parse(BVCodecParserContext *s, BVCodecContext *codec, const uint8_t *data_in, int data_in_size, const uint8_t **data_out, int data_out_size)
{
    H264ParserContext *p = s->priv_data;
    int stata = 0;
    int pos = 0;
    int end_pos = 0;
    int nalu_type = 0;
    if (codec && codec->extradata) {
        //parse BVCodecContext extradata
        s->codec = codec;
    //    h264_decode_extradata_size(s, codec->extradata, codec->extradata_size);
    }
    while (1) {
        pos = h264_find_start_code(data_in + end_pos, data_in_size - end_pos);
        if (pos < 0) {
            bv_log(s, BV_LOG_ERROR, "find start code error\n");
            break;
        }
        nalu_type = data_in[end_pos + pos + 4] & 0x1F;
        if (nalu_type == NAL_SPS) {
            bv_log(s, BV_LOG_INFO, "find sps info\n");
        } else if (nalu_type == NAL_PPS) {
            bv_log(s, BV_LOG_INFO, "find pps info\n");
        } else if (nalu_type == NAL_SEI) {
            bv_log(s, BV_LOG_INFO, "find sei info\n");
        }
        pos += 4;
        end_pos += h264_find_start_code(data_in + end_pos + pos, data_in_size - end_pos - pos);
        end_pos += pos;
    }
    if (end_pos <= 0) {
        return BVERROR(EINVAL);
    }
    return 0;
}

BVCodecParser bv_h264_parser = {
    .codec_ids          = { BV_CODEC_ID_H264 },
    .priv_data_size     = sizeof(H264ParserContext),
    .parser_init        = h264_parser_init,
    .parser_exit        = h264_parser_exit,
    .parser_parse       = h264_parser_parse,
};
