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

static int h264_parser_nal_units(BVCodecParserContext *s, BVCodecContext *codec, const uint8_t *data_in, int data_in_size)
{
    H264ParserContext *p = s->priv_data; 
    int nalu_type = 0;
    int nalu_size = 0;
    const uint8_t *buffer = data_in;
    const uint8_t *buffer_end = data_in + data_in_size;
    const uint8_t *start = buffer;
    const uint8_t *end = buffer;

    while (end <= buffer_end) {
        if ((start[0] == 0) && (start[1] == 0 ) && (start[2] == 0) && (start[3] == 1)) {
            end = start + 4;
            nalu_type = start[4] & 0x1F;
            if (nalu_type == NAL_SPS) {
                bv_log(s, BV_LOG_DEBUG, "find sps info\n");
            } else if (nalu_type == NAL_PPS) {
                bv_log(s, BV_LOG_DEBUG, "find pps info\n");
            } else if (nalu_type == NAL_SEI) {
                bv_log(s, BV_LOG_DEBUG, "find sei info\n");
            }else if (nalu_type == NAL_IDR_SLICE){
                bv_log(s, BV_LOG_DEBUG, "find idr info\n");
            } else {
                bv_log(s, BV_LOG_WARNING, "Not Support NAL Type\n");
            }

            while (end <= buffer_end) {
                if ((end[0] == 0) && (end[1] == 0) && (end[2] == 0) && (end[3] == 1)) {
                    int len = 0;
                    nalu_size = end - start; 
                    len = p->buffer_end - p->buffer_ptr;
                    len = BBMIN(len, nalu_size);
                    memcpy(p->buffer_ptr, start, len);
                    p->buffer_ptr += len;
                    start = end;
                    break;
                } else {
                    end ++;
                }
            }
        } else {
            start ++;
        }
    }

    return 0;
}
/**
 *  FIXME
 *  RTP h264 组包
 */
static int h264_parser_parse(BVCodecParserContext *s, BVCodecContext *codec, const uint8_t *data_in, int data_in_size, const uint8_t **data_out, int *data_out_size)
{
    H264ParserContext *p = s->priv_data;
    int size = 0;
    if (codec && codec->extradata) {
        //parse BVCodecContext extradata
        s->codec = codec;
    //    h264_decode_extradata_size(s, codec->extradata, codec->extradata_size);
    }

    if (h264_parser_nal_units(s, codec, data_in, data_in_size) < 0) {
        return BVERROR(EIO);
    }
    if (p->buffer_ptr != p->buffer) {
        if (codec->extradata)
            bv_freep(&codec->extradata);
        size = p->buffer_ptr - p->buffer;
        codec->extradata = bv_mallocz(size);
        if (codec->extradata) {
            codec->extradata_size = size;
            memcpy(codec->extradata, p->buffer, size);
        } else {
            return BVERROR(ENOMEM);
        }
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
