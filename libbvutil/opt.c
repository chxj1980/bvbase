/*
 * BVOptions
 * Copyright (c) 2005 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * BVOptions
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#include "bvutil.h"
#include "bvstring.h"
#include "channel_layout.h"
#include "common.h"
#include "opt.h"
#include "eval.h"
#include "dict.h"
#include "log.h"
#include "parseutils.h"
#include "pixdesc.h"
#include "mathematics.h"
#include "samplefmt.h"
#include "bprint.h"

#include <float.h>

#if FF_API_OLD_BVOPTIONS
const BVOption *bv_next_option(FF_CONST_BVUTIL55 void *obj, const BVOption *last)
{
    return bv_opt_next(obj, last);
}
#endif

const BVOption *bv_opt_next(FF_CONST_BVUTIL55 void *obj, const BVOption *last)
{
    const BVClass *class;
    if (!obj)
        return NULL;
    class = *(const BVClass**)obj;
    if (!last && class && class->option && class->option[0].name)
        return class->option;
    if (last && last[1].name)
        return ++last;
    return NULL;
}

static int read_number(const BVOption *o, const void *dst, double *num, int *den, int64_t *intnum)
{
    switch (o->type) {
    case BV_OPT_TYPE_FLAGS:     *intnum = *(unsigned int*)dst;return 0;
    case BV_OPT_TYPE_PIXEL_FMT:
    case BV_OPT_TYPE_SAMPLE_FMT:
    case BV_OPT_TYPE_INT:       *intnum = *(int         *)dst;return 0;
    case BV_OPT_TYPE_CHANNEL_LAYOUT:
    case BV_OPT_TYPE_DURATION:
    case BV_OPT_TYPE_INT64:     *intnum = *(int64_t     *)dst;return 0;
    case BV_OPT_TYPE_FLOAT:     *num    = *(float       *)dst;return 0;
    case BV_OPT_TYPE_DOUBLE:    *num    = *(double      *)dst;return 0;
    case BV_OPT_TYPE_RATIONAL:  *intnum = ((BVRational*)dst)->num;
                                *den    = ((BVRational*)dst)->den;
                                                        return 0;
    case BV_OPT_TYPE_CONST:     *num    = o->default_val.dbl; return 0;
    }
    return BVERROR(EINVAL);
}

static int write_number(void *obj, const BVOption *o, void *dst, double num, int den, int64_t intnum)
{
    if (o->type != BV_OPT_TYPE_FLAGS &&
        (o->max * den < num * intnum || o->min * den > num * intnum)) {
        bv_log(obj, BV_LOG_ERROR, "Value %f for parameter '%s' out of range [%g - %g]\n",
               num*intnum/den, o->name, o->min, o->max);
        return BVERROR(ERANGE);
    }
    if (o->type == BV_OPT_TYPE_FLAGS) {
        double d = num*intnum/den;
        if (d < -1.5 || d > 0xFFFFFFFF+0.5 || (llrint(d*256) & 255)) {
            bv_log(obj, BV_LOG_ERROR,
                   "Value %f for parameter '%s' is not a valid set of 32bit integer flags\n",
                   num*intnum/den, o->name);
            return BVERROR(ERANGE);
        }
    }

    switch (o->type) {
    case BV_OPT_TYPE_FLAGS:
    case BV_OPT_TYPE_PIXEL_FMT:
    case BV_OPT_TYPE_SAMPLE_FMT:
    case BV_OPT_TYPE_INT:   *(int       *)dst= llrint(num/den)*intnum; break;
    case BV_OPT_TYPE_DURATION:
    case BV_OPT_TYPE_CHANNEL_LAYOUT:
    case BV_OPT_TYPE_INT64: *(int64_t   *)dst= llrint(num/den)*intnum; break;
    case BV_OPT_TYPE_FLOAT: *(float     *)dst= num*intnum/den;         break;
    case BV_OPT_TYPE_DOUBLE:*(double    *)dst= num*intnum/den;         break;
    case BV_OPT_TYPE_RATIONAL:
        if ((int)num == num) *(BVRational*)dst= (BVRational){num*intnum, den};
        else                 *(BVRational*)dst= bv_d2q(num*intnum/den, 1<<24);
        break;
    default:
        return BVERROR(EINVAL);
    }
    return 0;
}

static int hexchar2int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int set_string_binary(void *obj, const BVOption *o, const char *val, uint8_t **dst)
{
    int *lendst = (int *)(dst + 1);
    uint8_t *bin, *ptr;
    int len;

    bv_freep(dst);
    *lendst = 0;

    if (!val || !(len = strlen(val)))
        return 0;

    if (len & 1)
        return BVERROR(EINVAL);
    len /= 2;

    ptr = bin = bv_malloc(len);
    while (*val) {
        int a = hexchar2int(*val++);
        int b = hexchar2int(*val++);
        if (a < 0 || b < 0) {
            bv_free(bin);
            return BVERROR(EINVAL);
        }
        *ptr++ = (a << 4) | b;
    }
    *dst = bin;
    *lendst = len;

    return 0;
}

static int set_string(void *obj, const BVOption *o, const char *val, uint8_t **dst)
{
    bv_freep(dst);
    *dst = bv_strdup(val);
    return *dst ? 0 : BVERROR(ENOMEM);
}

#define DEFAULT_NUMVAL(opt) ((opt->type == BV_OPT_TYPE_INT64 || \
                              opt->type == BV_OPT_TYPE_CONST || \
                              opt->type == BV_OPT_TYPE_FLAGS || \
                              opt->type == BV_OPT_TYPE_INT) ? \
                             opt->default_val.i64 : opt->default_val.dbl)

static int set_string_number(void *obj, void *target_obj, const BVOption *o, const char *val, void *dst)
{
    int ret = 0;
    int num, den;
    char c;

    if (sscanf(val, "%d%*1[:/]%d%c", &num, &den, &c) == 2) {
        if ((ret = write_number(obj, o, dst, 1, den, num)) >= 0)
            return ret;
        ret = 0;
    }

    for (;;) {
        int i = 0;
        char buf[256];
        int cmd = 0;
        double d;
        int64_t intnum = 1;

        if (o->type == BV_OPT_TYPE_FLAGS) {
            if (*val == '+' || *val == '-')
                cmd = *(val++);
            for (; i < sizeof(buf) - 1 && val[i] && val[i] != '+' && val[i] != '-'; i++)
                buf[i] = val[i];
            buf[i] = 0;
        }

        {
            const BVOption *o_named = bv_opt_find(target_obj, i ? buf : val, o->unit, 0, 0);
            int res;
            int ci = 0;
            double const_values[64];
            const char * const_names[64];
            if (o_named && o_named->type == BV_OPT_TYPE_CONST)
                d = DEFAULT_NUMVAL(o_named);
            else {
                if (o->unit) {
                    for (o_named = NULL; o_named = bv_opt_next(target_obj, o_named); ) {
                        if (o_named->type == BV_OPT_TYPE_CONST &&
                            o_named->unit &&
                            !strcmp(o_named->unit, o->unit)) {
                            if (ci + 6 >= FF_ARRAY_ELEMS(const_values)) {
                                bv_log(obj, BV_LOG_ERROR, "const_values array too small for %s\n", o->unit);
                                return BVERROR_PATCHWELCOME;
                            }
                            const_names [ci  ] = o_named->name;
                            const_values[ci++] = DEFAULT_NUMVAL(o_named);
                        }
                    }
                }
                const_names [ci  ] = "default";
                const_values[ci++] = DEFAULT_NUMVAL(o);
                const_names [ci  ] = "max";
                const_values[ci++] = o->max;
                const_names [ci  ] = "min";
                const_values[ci++] = o->min;
                const_names [ci  ] = "none";
                const_values[ci++] = 0;
                const_names [ci  ] = "all";
                const_values[ci++] = ~0;
                const_names [ci] = NULL;
                const_values[ci] = 0;

                res = bv_expr_parse_and_eval(&d, i ? buf : val, const_names,
                                            const_values, NULL, NULL, NULL, NULL, NULL, 0, obj);
                if (res < 0) {
                    bv_log(obj, BV_LOG_ERROR, "Unable to parse option value \"%s\"\n", val);
                    return res;
                }
            }
        }
        if (o->type == BV_OPT_TYPE_FLAGS) {
            read_number(o, dst, NULL, NULL, &intnum);
            if      (cmd == '+') d = intnum | (int64_t)d;
            else if (cmd == '-') d = intnum &~(int64_t)d;
        }

        if ((ret = write_number(obj, o, dst, d, 1, 1)) < 0)
            return ret;
        val += i;
        if (!i || !*val)
            return 0;
    }

    return 0;
}

static int set_string_image_size(void *obj, const BVOption *o, const char *val, int *dst)
{
    int ret;

    if (!val || !strcmp(val, "none")) {
        dst[0] =
        dst[1] = 0;
        return 0;
    }
    ret = bv_parse_video_size(dst, dst + 1, val);
    if (ret < 0)
        bv_log(obj, BV_LOG_ERROR, "Unable to parse option value \"%s\" as image size\n", val);
    return ret;
}

static int set_string_video_rate(void *obj, const BVOption *o, const char *val, BVRational *dst)
{
    int ret;
    if (!val) {
        ret = BVERROR(EINVAL);
    } else {
        ret = bv_parse_video_rate(dst, val);
    }
    if (ret < 0)
        bv_log(obj, BV_LOG_ERROR, "Unable to parse option value \"%s\" as video rate\n", val);
    return ret;
}

static int set_string_color(void *obj, const BVOption *o, const char *val, uint8_t *dst)
{
    int ret;

    if (!val) {
        return 0;
    } else {
        ret = bv_parse_color(dst, val, -1, obj);
        if (ret < 0)
            bv_log(obj, BV_LOG_ERROR, "Unable to parse option value \"%s\" as color\n", val);
        return ret;
    }
    return 0;
}

static int set_string_fmt(void *obj, const BVOption *o, const char *val, uint8_t *dst,
                          int fmt_nb, int ((*get_fmt)(const char *)), const char *desc)
{
    int fmt, min, max;

    if (!val || !strcmp(val, "none")) {
        fmt = -1;
    } else {
        fmt = get_fmt(val);
        if (fmt == -1) {
            char *tail;
            fmt = strtol(val, &tail, 0);
            if (*tail || (unsigned)fmt >= fmt_nb) {
                bv_log(obj, BV_LOG_ERROR,
                       "Unable to parse option value \"%s\" as %s\n", val, desc);
                return BVERROR(EINVAL);
            }
        }
    }

    min = FFMAX(o->min, -1);
    max = FFMIN(o->max, fmt_nb-1);

    // hack for compatibility with old ffmpeg
    if(min == 0 && max == 0) {
        min = -1;
        max = fmt_nb-1;
    }

    if (fmt < min || fmt > max) {
        bv_log(obj, BV_LOG_ERROR,
               "Value %d for parameter '%s' out of %s format range [%d - %d]\n",
               fmt, o->name, desc, min, max);
        return BVERROR(ERANGE);
    }

    *(int *)dst = fmt;
    return 0;
}

static int set_string_pixel_fmt(void *obj, const BVOption *o, const char *val, uint8_t *dst)
{
    return set_string_fmt(obj, o, val, dst,
                          BV_PIX_FMT_NB, bv_get_pix_fmt, "pixel format");
}

static int set_string_sample_fmt(void *obj, const BVOption *o, const char *val, uint8_t *dst)
{
    return set_string_fmt(obj, o, val, dst,
                          BV_SAMPLE_FMT_NB, bv_get_sample_fmt, "sample format");
}

#if FF_API_OLD_BVOPTIONS
int bv_set_string3(void *obj, const char *name, const char *val, int alloc, const BVOption **o_out)
{
    const BVOption *o = bv_opt_find(obj, name, NULL, 0, 0);
    if (o_out)
        *o_out = o;
    return bv_opt_set(obj, name, val, 0);
}
#endif

int bv_opt_set(void *obj, const char *name, const char *val, int search_flags)
{
    int ret = 0;
    void *dst, *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (!val && (o->type != BV_OPT_TYPE_STRING &&
                 o->type != BV_OPT_TYPE_PIXEL_FMT && o->type != BV_OPT_TYPE_SAMPLE_FMT &&
                 o->type != BV_OPT_TYPE_IMAGE_SIZE && o->type != BV_OPT_TYPE_VIDEO_RATE &&
                 o->type != BV_OPT_TYPE_DURATION && o->type != BV_OPT_TYPE_COLOR &&
                 o->type != BV_OPT_TYPE_CHANNEL_LAYOUT))
        return BVERROR(EINVAL);

    if (o->flags & BV_OPT_FLAG_READONLY)
        return BVERROR(EINVAL);

    dst = ((uint8_t*)target_obj) + o->offset;
    switch (o->type) {
    case BV_OPT_TYPE_STRING:   return set_string(obj, o, val, dst);
    case BV_OPT_TYPE_BINARY:   return set_string_binary(obj, o, val, dst);
    case BV_OPT_TYPE_FLAGS:
    case BV_OPT_TYPE_INT:
    case BV_OPT_TYPE_INT64:
    case BV_OPT_TYPE_FLOAT:
    case BV_OPT_TYPE_DOUBLE:
    case BV_OPT_TYPE_RATIONAL: return set_string_number(obj, target_obj, o, val, dst);
    case BV_OPT_TYPE_IMAGE_SIZE: return set_string_image_size(obj, o, val, dst);
    case BV_OPT_TYPE_VIDEO_RATE: return set_string_video_rate(obj, o, val, dst);
    case BV_OPT_TYPE_PIXEL_FMT:  return set_string_pixel_fmt(obj, o, val, dst);
    case BV_OPT_TYPE_SAMPLE_FMT: return set_string_sample_fmt(obj, o, val, dst);
    case BV_OPT_TYPE_DURATION:
        if (!val) {
            *(int64_t *)dst = 0;
            return 0;
        } else {
            if ((ret = bv_parse_time(dst, val, 1)) < 0)
                bv_log(obj, BV_LOG_ERROR, "Unable to parse option value \"%s\" as duration\n", val);
            return ret;
        }
        break;
    case BV_OPT_TYPE_COLOR:      return set_string_color(obj, o, val, dst);
    case BV_OPT_TYPE_CHANNEL_LAYOUT:
        if (!val || !strcmp(val, "none")) {
            *(int64_t *)dst = 0;
        } else {
#if FF_API_GET_CHANNEL_LAYOUT_COMPAT
            int64_t cl = ff_get_channel_layout(val, 0);
#else
            int64_t cl = bv_get_channel_layout(val);
#endif
            if (!cl) {
                bv_log(obj, BV_LOG_ERROR, "Unable to parse option value \"%s\" as channel layout\n", val);
                ret = BVERROR(EINVAL);
            }
            *(int64_t *)dst = cl;
            return ret;
        }
        break;
    }

    bv_log(obj, BV_LOG_ERROR, "Invalid option type.\n");
    return BVERROR(EINVAL);
}

#define OPT_EVAL_NUMBER(name, opttype, vartype)\
    int bv_opt_eval_ ## name(void *obj, const BVOption *o, const char *val, vartype *name ## _out)\
    {\
        if (!o || o->type != opttype || o->flags & BV_OPT_FLAG_READONLY)\
            return BVERROR(EINVAL);\
        return set_string_number(obj, obj, o, val, name ## _out);\
    }

OPT_EVAL_NUMBER(flags,  BV_OPT_TYPE_FLAGS,    int)
OPT_EVAL_NUMBER(int,    BV_OPT_TYPE_INT,      int)
OPT_EVAL_NUMBER(int64,  BV_OPT_TYPE_INT64,    int64_t)
OPT_EVAL_NUMBER(float,  BV_OPT_TYPE_FLOAT,    float)
OPT_EVAL_NUMBER(double, BV_OPT_TYPE_DOUBLE,   double)
OPT_EVAL_NUMBER(q,      BV_OPT_TYPE_RATIONAL, BVRational)

static int set_number(void *obj, const char *name, double num, int den, int64_t intnum,
                                  int search_flags)
{
    void *dst, *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;

    if (o->flags & BV_OPT_FLAG_READONLY)
        return BVERROR(EINVAL);

    dst = ((uint8_t*)target_obj) + o->offset;
    return write_number(obj, o, dst, num, den, intnum);
}

#if FF_API_OLD_BVOPTIONS
const BVOption *bv_set_double(void *obj, const char *name, double n)
{
    const BVOption *o = bv_opt_find(obj, name, NULL, 0, 0);
    if (set_number(obj, name, n, 1, 1, 0) < 0)
        return NULL;
    return o;
}

const BVOption *bv_set_q(void *obj, const char *name, BVRational n)
{
    const BVOption *o = bv_opt_find(obj, name, NULL, 0, 0);
    if (set_number(obj, name, n.num, n.den, 1, 0) < 0)
        return NULL;
    return o;
}

const BVOption *bv_set_int(void *obj, const char *name, int64_t n)
{
    const BVOption *o = bv_opt_find(obj, name, NULL, 0, 0);
    if (set_number(obj, name, 1, 1, n, 0) < 0)
        return NULL;
    return o;
}
#endif

int bv_opt_set_int(void *obj, const char *name, int64_t val, int search_flags)
{
    return set_number(obj, name, 1, 1, val, search_flags);
}

int bv_opt_set_double(void *obj, const char *name, double val, int search_flags)
{
    return set_number(obj, name, val, 1, 1, search_flags);
}

int bv_opt_set_q(void *obj, const char *name, BVRational val, int search_flags)
{
    return set_number(obj, name, val.num, val.den, 1, search_flags);
}

int bv_opt_set_bin(void *obj, const char *name, const uint8_t *val, int len, int search_flags)
{
    void *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    uint8_t *ptr;
    uint8_t **dst;
    int *lendst;

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;

    if (o->type != BV_OPT_TYPE_BINARY || o->flags & BV_OPT_FLAG_READONLY)
        return BVERROR(EINVAL);

    ptr = len ? bv_malloc(len) : NULL;
    if (len && !ptr)
        return BVERROR(ENOMEM);

    dst = (uint8_t **)(((uint8_t *)target_obj) + o->offset);
    lendst = (int *)(dst + 1);

    bv_free(*dst);
    *dst = ptr;
    *lendst = len;
    if (len)
        memcpy(ptr, val, len);

    return 0;
}

int bv_opt_set_image_size(void *obj, const char *name, int w, int h, int search_flags)
{
    void *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != BV_OPT_TYPE_IMAGE_SIZE) {
        bv_log(obj, BV_LOG_ERROR,
               "The value set by option '%s' is not an image size.\n", o->name);
        return BVERROR(EINVAL);
    }
    if (w<0 || h<0) {
        bv_log(obj, BV_LOG_ERROR,
               "Invalid negative size value %dx%d for size '%s'\n", w, h, o->name);
        return BVERROR(EINVAL);
    }
    *(int *)(((uint8_t *)target_obj)             + o->offset) = w;
    *(int *)(((uint8_t *)target_obj+sizeof(int)) + o->offset) = h;
    return 0;
}

int bv_opt_set_video_rate(void *obj, const char *name, BVRational val, int search_flags)
{
    void *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != BV_OPT_TYPE_VIDEO_RATE) {
        bv_log(obj, BV_LOG_ERROR,
               "The value set by option '%s' is not a video rate.\n", o->name);
        return BVERROR(EINVAL);
    }
    if (val.num <= 0 || val.den <= 0)
        return BVERROR(EINVAL);
    return set_number(obj, name, val.num, val.den, 1, search_flags);
}

static int set_format(void *obj, const char *name, int fmt, int search_flags,
                      enum BVOptionType type, const char *desc, int nb_fmts)
{
    void *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0,
                                     search_flags, &target_obj);
    int min, max;

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != type) {
        bv_log(obj, BV_LOG_ERROR,
               "The value set by option '%s' is not a %s format", name, desc);
        return BVERROR(EINVAL);
    }

    min = FFMAX(o->min, -1);
    max = FFMIN(o->max, nb_fmts-1);

    if (fmt < min || fmt > max) {
        bv_log(obj, BV_LOG_ERROR,
               "Value %d for parameter '%s' out of %s format range [%d - %d]\n",
               fmt, name, desc, min, max);
        return BVERROR(ERANGE);
    }
    *(int *)(((uint8_t *)target_obj) + o->offset) = fmt;
    return 0;
}

int bv_opt_set_pixel_fmt(void *obj, const char *name, enum BVPixelFormat fmt, int search_flags)
{
    return set_format(obj, name, fmt, search_flags, BV_OPT_TYPE_PIXEL_FMT, "pixel", BV_PIX_FMT_NB);
}

int bv_opt_set_sample_fmt(void *obj, const char *name, enum BVSampleFormat fmt, int search_flags)
{
    return set_format(obj, name, fmt, search_flags, BV_OPT_TYPE_SAMPLE_FMT, "sample", BV_SAMPLE_FMT_NB);
}

int bv_opt_set_channel_layout(void *obj, const char *name, int64_t cl, int search_flags)
{
    void *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != BV_OPT_TYPE_CHANNEL_LAYOUT) {
        bv_log(obj, BV_LOG_ERROR,
               "The value set by option '%s' is not a channel layout.\n", o->name);
        return BVERROR(EINVAL);
    }
    *(int64_t *)(((uint8_t *)target_obj) + o->offset) = cl;
    return 0;
}

#if FF_API_OLD_BVOPTIONS
/**
 *
 * @param buf a buffer which is used for returning non string values as strings, can be NULL
 * @param buf_len allocated length in bytes of buf
 */
const char *bv_get_string(void *obj, const char *name, const BVOption **o_out, char *buf, int buf_len)
{
    const BVOption *o = bv_opt_find(obj, name, NULL, 0, BV_OPT_SEARCH_CHILDREN);
    void *dst;
    uint8_t *bin;
    int len, i;
    if (!o)
        return NULL;
    if (o->type != BV_OPT_TYPE_STRING && (!buf || !buf_len))
        return NULL;

    dst= ((uint8_t*)obj) + o->offset;
    if (o_out) *o_out= o;

    switch (o->type) {
    case BV_OPT_TYPE_FLAGS:     snprintf(buf, buf_len, "0x%08X",*(int    *)dst);break;
    case BV_OPT_TYPE_INT:       snprintf(buf, buf_len, "%d" , *(int    *)dst);break;
    case BV_OPT_TYPE_INT64:     snprintf(buf, buf_len, "%"PRId64, *(int64_t*)dst);break;
    case BV_OPT_TYPE_FLOAT:     snprintf(buf, buf_len, "%f" , *(float  *)dst);break;
    case BV_OPT_TYPE_DOUBLE:    snprintf(buf, buf_len, "%f" , *(double *)dst);break;
    case BV_OPT_TYPE_RATIONAL:  snprintf(buf, buf_len, "%d/%d", ((BVRational*)dst)->num, ((BVRational*)dst)->den);break;
    case BV_OPT_TYPE_CONST:     snprintf(buf, buf_len, "%f" , o->default_val.dbl);break;
    case BV_OPT_TYPE_STRING:    return *(void**)dst;
    case BV_OPT_TYPE_BINARY:
        len = *(int*)(((uint8_t *)dst) + sizeof(uint8_t *));
        if (len >= (buf_len + 1)/2) return NULL;
        bin = *(uint8_t**)dst;
        for (i = 0; i < len; i++) snprintf(buf + i*2, 3, "%02X", bin[i]);
        break;
    default: return NULL;
    }
    return buf;
}
#endif

int bv_opt_set_dict_val(void *obj, const char *name, const BVDictionary *val, int search_flags)
{
    void *target_obj;
    BVDictionary **dst;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->flags & BV_OPT_FLAG_READONLY)
        return BVERROR(EINVAL);

    dst = (BVDictionary **)(((uint8_t *)target_obj) + o->offset);
    bv_dict_free(dst);
    bv_dict_copy(dst, val, 0);

    return 0;
}

int bv_opt_get(void *obj, const char *name, int search_flags, uint8_t **out_val)
{
    void *dst, *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    uint8_t *bin, buf[128];
    int len, i, ret;
    int64_t i64;

    if (!o || !target_obj || (o->offset<=0 && o->type != BV_OPT_TYPE_CONST))
        return BVERROR_OPTION_NOT_FOUND;

    dst = (uint8_t*)target_obj + o->offset;

    buf[0] = 0;
    switch (o->type) {
    case BV_OPT_TYPE_FLAGS:     ret = snprintf(buf, sizeof(buf), "0x%08X",  *(int    *)dst);break;
    case BV_OPT_TYPE_INT:       ret = snprintf(buf, sizeof(buf), "%d" ,     *(int    *)dst);break;
    case BV_OPT_TYPE_INT64:     ret = snprintf(buf, sizeof(buf), "%"PRId64, *(int64_t*)dst);break;
    case BV_OPT_TYPE_FLOAT:     ret = snprintf(buf, sizeof(buf), "%f" ,     *(float  *)dst);break;
    case BV_OPT_TYPE_DOUBLE:    ret = snprintf(buf, sizeof(buf), "%f" ,     *(double *)dst);break;
    case BV_OPT_TYPE_VIDEO_RATE:
    case BV_OPT_TYPE_RATIONAL:  ret = snprintf(buf, sizeof(buf), "%d/%d",   ((BVRational*)dst)->num, ((BVRational*)dst)->den);break;
    case BV_OPT_TYPE_CONST:     ret = snprintf(buf, sizeof(buf), "%f" ,     o->default_val.dbl);break;
    case BV_OPT_TYPE_STRING:
        if (*(uint8_t**)dst)
            *out_val = bv_strdup(*(uint8_t**)dst);
        else
            *out_val = bv_strdup("");
        return *out_val ? 0 : BVERROR(ENOMEM);
    case BV_OPT_TYPE_BINARY:
        len = *(int*)(((uint8_t *)dst) + sizeof(uint8_t *));
        if ((uint64_t)len*2 + 1 > INT_MAX)
            return BVERROR(EINVAL);
        if (!(*out_val = bv_malloc(len*2 + 1)))
            return BVERROR(ENOMEM);
        if (!len) {
            *out_val[0] = '\0';
            return 0;
        }
        bin = *(uint8_t**)dst;
        for (i = 0; i < len; i++)
            snprintf(*out_val + i*2, 3, "%02X", bin[i]);
        return 0;
    case BV_OPT_TYPE_IMAGE_SIZE:
        ret = snprintf(buf, sizeof(buf), "%dx%d", ((int *)dst)[0], ((int *)dst)[1]);
        break;
    case BV_OPT_TYPE_PIXEL_FMT:
        ret = snprintf(buf, sizeof(buf), "%s", (char *)bv_x_if_null(bv_get_pix_fmt_name(*(enum BVPixelFormat *)dst), "none"));
        break;
    case BV_OPT_TYPE_SAMPLE_FMT:
        ret = snprintf(buf, sizeof(buf), "%s", (char *)bv_x_if_null(bv_get_sample_fmt_name(*(enum BVSampleFormat *)dst), "none"));
        break;
    case BV_OPT_TYPE_DURATION:
        i64 = *(int64_t *)dst;
        ret = snprintf(buf, sizeof(buf), "%"PRIi64":%02d:%02d.%06d",
                       i64 / 3600000000, (int)((i64 / 60000000) % 60),
                       (int)((i64 / 1000000) % 60), (int)(i64 % 1000000));
        break;
    case BV_OPT_TYPE_COLOR:
        ret = snprintf(buf, sizeof(buf), "0x%02x%02x%02x%02x",
                       (int)((uint8_t *)dst)[0], (int)((uint8_t *)dst)[1],
                       (int)((uint8_t *)dst)[2], (int)((uint8_t *)dst)[3]);
        break;
    case BV_OPT_TYPE_CHANNEL_LAYOUT:
        i64 = *(int64_t *)dst;
        ret = snprintf(buf, sizeof(buf), "0x%"PRIx64, i64);
        break;
    default:
        return BVERROR(EINVAL);
    }

    if (ret >= sizeof(buf))
        return BVERROR(EINVAL);
    *out_val = bv_strdup(buf);
    return *out_val ? 0 : BVERROR(ENOMEM);
}

static int get_number(void *obj, const char *name, const BVOption **o_out, double *num, int *den, int64_t *intnum,
                      int search_flags)
{
    void *dst, *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        goto error;

    dst = ((uint8_t*)target_obj) + o->offset;

    if (o_out) *o_out= o;

    return read_number(o, dst, num, den, intnum);

error:
    *den=*intnum=0;
    return -1;
}

#if FF_API_OLD_BVOPTIONS
double bv_get_double(void *obj, const char *name, const BVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;

    if (get_number(obj, name, o_out, &num, &den, &intnum, 0) < 0)
        return NAN;
    return num*intnum/den;
}

BVRational bv_get_q(void *obj, const char *name, const BVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;

    if (get_number(obj, name, o_out, &num, &den, &intnum, 0) < 0)
        return (BVRational){0, 0};
    if (num == 1.0 && (int)intnum == intnum)
        return (BVRational){intnum, den};
    else
        return bv_d2q(num*intnum/den, 1<<24);
}

int64_t bv_get_int(void *obj, const char *name, const BVOption **o_out)
{
    int64_t intnum=1;
    double num=1;
    int den=1;

    if (get_number(obj, name, o_out, &num, &den, &intnum, 0) < 0)
        return -1;
    return num*intnum/den;
}
#endif

int bv_opt_get_int(void *obj, const char *name, int search_flags, int64_t *out_val)
{
    int64_t intnum = 1;
    double     num = 1;
    int   ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;
    *out_val = num*intnum/den;
    return 0;
}

int bv_opt_get_double(void *obj, const char *name, int search_flags, double *out_val)
{
    int64_t intnum = 1;
    double     num = 1;
    int   ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;
    *out_val = num*intnum/den;
    return 0;
}

int bv_opt_get_q(void *obj, const char *name, int search_flags, BVRational *out_val)
{
    int64_t intnum = 1;
    double     num = 1;
    int   ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;

    if (num == 1.0 && (int)intnum == intnum)
        *out_val = (BVRational){intnum, den};
    else
        *out_val = bv_d2q(num*intnum/den, 1<<24);
    return 0;
}

int bv_opt_get_image_size(void *obj, const char *name, int search_flags, int *w_out, int *h_out)
{
    void *dst, *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != BV_OPT_TYPE_IMAGE_SIZE) {
        bv_log(obj, BV_LOG_ERROR,
               "The value for option '%s' is not an image size.\n", name);
        return BVERROR(EINVAL);
    }

    dst = ((uint8_t*)target_obj) + o->offset;
    if (w_out) *w_out = *(int *)dst;
    if (h_out) *h_out = *((int *)dst+1);
    return 0;
}

int bv_opt_get_video_rate(void *obj, const char *name, int search_flags, BVRational *out_val)
{
    int64_t intnum = 1;
    double     num = 1;
    int   ret, den = 1;

    if ((ret = get_number(obj, name, NULL, &num, &den, &intnum, search_flags)) < 0)
        return ret;

    if (num == 1.0 && (int)intnum == intnum)
        *out_val = (BVRational){intnum, den};
    else
        *out_val = bv_d2q(num*intnum/den, 1<<24);
    return 0;
}

static int get_format(void *obj, const char *name, int search_flags, int *out_fmt,
                      enum BVOptionType type, const char *desc)
{
    void *dst, *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != type) {
        bv_log(obj, BV_LOG_ERROR,
               "The value for option '%s' is not a %s format.\n", desc, name);
        return BVERROR(EINVAL);
    }

    dst = ((uint8_t*)target_obj) + o->offset;
    *out_fmt = *(int *)dst;
    return 0;
}

int bv_opt_get_pixel_fmt(void *obj, const char *name, int search_flags, enum BVPixelFormat *out_fmt)
{
    return get_format(obj, name, search_flags, out_fmt, BV_OPT_TYPE_PIXEL_FMT, "pixel");
}

int bv_opt_get_sample_fmt(void *obj, const char *name, int search_flags, enum BVSampleFormat *out_fmt)
{
    return get_format(obj, name, search_flags, out_fmt, BV_OPT_TYPE_SAMPLE_FMT, "sample");
}

int bv_opt_get_channel_layout(void *obj, const char *name, int search_flags, int64_t *cl)
{
    void *dst, *target_obj;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);
    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != BV_OPT_TYPE_CHANNEL_LAYOUT) {
        bv_log(obj, BV_LOG_ERROR,
               "The value for option '%s' is not a channel layout.\n", name);
        return BVERROR(EINVAL);
    }

    dst = ((uint8_t*)target_obj) + o->offset;
    *cl = *(int64_t *)dst;
    return 0;
}

int bv_opt_get_dict_val(void *obj, const char *name, int search_flags, BVDictionary **out_val)
{
    void *target_obj;
    BVDictionary *src;
    const BVOption *o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target_obj);

    if (!o || !target_obj)
        return BVERROR_OPTION_NOT_FOUND;
    if (o->type != BV_OPT_TYPE_DICT)
        return BVERROR(EINVAL);

    src = *(BVDictionary **)(((uint8_t *)target_obj) + o->offset);
    bv_dict_copy(out_val, src, 0);

    return 0;
}

int bv_opt_flag_is_set(void *obj, const char *field_name, const char *flag_name)
{
    const BVOption *field = bv_opt_find(obj, field_name, NULL, 0, 0);
    const BVOption *flag  = bv_opt_find(obj, flag_name,
                                        field ? field->unit : NULL, 0, 0);
    int64_t res;

    if (!field || !flag || flag->type != BV_OPT_TYPE_CONST ||
        bv_opt_get_int(obj, field_name, 0, &res) < 0)
        return 0;
    return res & flag->default_val.i64;
}

static void log_value(void *bv_log_obj, int level, double d)
{
    if      (d == INT_MAX) {
        bv_log(bv_log_obj, level, "INT_MAX");
    } else if (d == INT_MIN) {
        bv_log(bv_log_obj, level, "INT_MIN");
    } else if (d == UINT32_MAX) {
        bv_log(bv_log_obj, level, "UINT32_MAX");
    } else if (d == (double)INT64_MAX) {
        bv_log(bv_log_obj, level, "I64_MAX");
    } else if (d == INT64_MIN) {
        bv_log(bv_log_obj, level, "I64_MIN");
    } else if (d == FLT_MAX) {
        bv_log(bv_log_obj, level, "FLT_MAX");
    } else if (d == FLT_MIN) {
        bv_log(bv_log_obj, level, "FLT_MIN");
    } else if (d == -FLT_MAX) {
        bv_log(bv_log_obj, level, "-FLT_MAX");
    } else if (d == -FLT_MIN) {
        bv_log(bv_log_obj, level, "-FLT_MIN");
    } else if (d == DBL_MAX) {
        bv_log(bv_log_obj, level, "DBL_MAX");
    } else if (d == DBL_MIN) {
        bv_log(bv_log_obj, level, "DBL_MIN");
    } else if (d == -DBL_MAX) {
        bv_log(bv_log_obj, level, "-DBL_MAX");
    } else if (d == -DBL_MIN) {
        bv_log(bv_log_obj, level, "-DBL_MIN");
    } else {
        bv_log(bv_log_obj, level, "%g", d);
    }
}

static void opt_list(void *obj, void *bv_log_obj, const char *unit,
                     int req_flags, int rej_flags)
{
    const BVOption *opt=NULL;
    BVOptionRanges *r;
    int i;

    while ((opt = bv_opt_next(obj, opt))) {
        if (!(opt->flags & req_flags) || (opt->flags & rej_flags))
            continue;

        /* Don't print CONST's on level one.
         * Don't print anything but CONST's on level two.
         * Only print items from the requested unit.
         */
        if (!unit && opt->type==BV_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type!=BV_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type==BV_OPT_TYPE_CONST && strcmp(unit, opt->unit))
            continue;
        else if (unit && opt->type == BV_OPT_TYPE_CONST)
            bv_log(bv_log_obj, BV_LOG_INFO, "     %-15s ", opt->name);
        else
            bv_log(bv_log_obj, BV_LOG_INFO, "  %s%-17s ",
                   (opt->flags & BV_OPT_FLAG_FILTERING_PARAM) ? "" : "-",
                   opt->name);

        switch (opt->type) {
            case BV_OPT_TYPE_FLAGS:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<flags>");
                break;
            case BV_OPT_TYPE_INT:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<int>");
                break;
            case BV_OPT_TYPE_INT64:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<int64>");
                break;
            case BV_OPT_TYPE_DOUBLE:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<double>");
                break;
            case BV_OPT_TYPE_FLOAT:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<float>");
                break;
            case BV_OPT_TYPE_STRING:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<string>");
                break;
            case BV_OPT_TYPE_RATIONAL:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<rational>");
                break;
            case BV_OPT_TYPE_BINARY:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<binary>");
                break;
            case BV_OPT_TYPE_IMAGE_SIZE:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<image_size>");
                break;
            case BV_OPT_TYPE_VIDEO_RATE:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<video_rate>");
                break;
            case BV_OPT_TYPE_PIXEL_FMT:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<pix_fmt>");
                break;
            case BV_OPT_TYPE_SAMPLE_FMT:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<sample_fmt>");
                break;
            case BV_OPT_TYPE_DURATION:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<duration>");
                break;
            case BV_OPT_TYPE_COLOR:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<color>");
                break;
            case BV_OPT_TYPE_CHANNEL_LAYOUT:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "<channel_layout>");
                break;
            case BV_OPT_TYPE_CONST:
            default:
                bv_log(bv_log_obj, BV_LOG_INFO, "%-12s ", "");
                break;
        }
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_ENCODING_PARAM) ? 'E' : '.');
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_DECODING_PARAM) ? 'D' : '.');
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_FILTERING_PARAM)? 'F' : '.');
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_VIDEO_PARAM   ) ? 'V' : '.');
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_AUDIO_PARAM   ) ? 'A' : '.');
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_SUBTITLE_PARAM) ? 'S' : '.');
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_EXPORT)         ? 'X' : '.');
        bv_log(bv_log_obj, BV_LOG_INFO, "%c", (opt->flags & BV_OPT_FLAG_READONLY)       ? 'R' : '.');

        if (opt->help)
            bv_log(bv_log_obj, BV_LOG_INFO, " %s", opt->help);

        if (bv_opt_query_ranges(&r, obj, opt->name, BV_OPT_SEARCH_FAKE_OBJ) >= 0) {
            switch (opt->type) {
            case BV_OPT_TYPE_INT:
            case BV_OPT_TYPE_INT64:
            case BV_OPT_TYPE_DOUBLE:
            case BV_OPT_TYPE_FLOAT:
            case BV_OPT_TYPE_RATIONAL:
                for (i = 0; i < r->nb_ranges; i++) {
                    bv_log(bv_log_obj, BV_LOG_INFO, " (from ");
                    log_value(bv_log_obj, BV_LOG_INFO, r->range[i]->value_min);
                    bv_log(bv_log_obj, BV_LOG_INFO, " to ");
                    log_value(bv_log_obj, BV_LOG_INFO, r->range[i]->value_max);
                    bv_log(bv_log_obj, BV_LOG_INFO, ")");
                }
                break;
            }
            bv_opt_freep_ranges(&r);
        }

        if (opt->type != BV_OPT_TYPE_CONST  &&
            opt->type != BV_OPT_TYPE_BINARY &&
                !((opt->type == BV_OPT_TYPE_COLOR      ||
                   opt->type == BV_OPT_TYPE_IMAGE_SIZE ||
                   opt->type == BV_OPT_TYPE_STRING     ||
                   opt->type == BV_OPT_TYPE_VIDEO_RATE) &&
                  !opt->default_val.str)) {
            bv_log(bv_log_obj, BV_LOG_INFO, " (default ");
            switch (opt->type) {
            case BV_OPT_TYPE_FLAGS:
                bv_log(bv_log_obj, BV_LOG_INFO, "%"PRIX64, opt->default_val.i64);
                break;
            case BV_OPT_TYPE_DURATION:
            case BV_OPT_TYPE_INT:
            case BV_OPT_TYPE_INT64:
                log_value(bv_log_obj, BV_LOG_INFO, opt->default_val.i64);
                break;
            case BV_OPT_TYPE_DOUBLE:
            case BV_OPT_TYPE_FLOAT:
                log_value(bv_log_obj, BV_LOG_INFO, opt->default_val.dbl);
                break;
            case BV_OPT_TYPE_RATIONAL: {
                BVRational q = bv_d2q(opt->default_val.dbl, INT_MAX);
                bv_log(bv_log_obj, BV_LOG_INFO, "%d/%d", q.num, q.den); }
                break;
            case BV_OPT_TYPE_PIXEL_FMT:
                bv_log(bv_log_obj, BV_LOG_INFO, "%s", (char *)bv_x_if_null(bv_get_pix_fmt_name(opt->default_val.i64), "none"));
                break;
            case BV_OPT_TYPE_SAMPLE_FMT:
                bv_log(bv_log_obj, BV_LOG_INFO, "%s", (char *)bv_x_if_null(bv_get_sample_fmt_name(opt->default_val.i64), "none"));
                break;
            case BV_OPT_TYPE_COLOR:
            case BV_OPT_TYPE_IMAGE_SIZE:
            case BV_OPT_TYPE_STRING:
            case BV_OPT_TYPE_VIDEO_RATE:
                bv_log(bv_log_obj, BV_LOG_INFO, "\"%s\"", opt->default_val.str);
                break;
            case BV_OPT_TYPE_CHANNEL_LAYOUT:
                bv_log(bv_log_obj, BV_LOG_INFO, "0x%"PRIx64, opt->default_val.i64);
                break;
            }
            bv_log(bv_log_obj, BV_LOG_INFO, ")");
        }

        bv_log(bv_log_obj, BV_LOG_INFO, "\n");
        if (opt->unit && opt->type != BV_OPT_TYPE_CONST) {
            opt_list(obj, bv_log_obj, opt->unit, req_flags, rej_flags);
        }
    }
}

int bv_opt_show2(void *obj, void *bv_log_obj, int req_flags, int rej_flags)
{
    if (!obj)
        return -1;

    bv_log(bv_log_obj, BV_LOG_INFO, "%s BVOptions:\n", (*(BVClass**)obj)->class_name);

    opt_list(obj, bv_log_obj, NULL, req_flags, rej_flags);

    return 0;
}

void bv_opt_set_defaults(void *s)
{
#if FF_API_OLD_BVOPTIONS
    bv_opt_set_defaults2(s, 0, 0);
}

void bv_opt_set_defaults2(void *s, int mask, int flags)
{
#endif
    const BVOption *opt = NULL;
    while ((opt = bv_opt_next(s, opt))) {
        void *dst = ((uint8_t*)s) + opt->offset;
#if FF_API_OLD_BVOPTIONS
        if ((opt->flags & mask) != flags)
            continue;
#endif

        if (opt->flags & BV_OPT_FLAG_READONLY)
            continue;

        switch (opt->type) {
            case BV_OPT_TYPE_CONST:
                /* Nothing to be done here */
            break;
            case BV_OPT_TYPE_FLAGS:
            case BV_OPT_TYPE_INT:
            case BV_OPT_TYPE_INT64:
            case BV_OPT_TYPE_DURATION:
            case BV_OPT_TYPE_CHANNEL_LAYOUT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
            break;
            case BV_OPT_TYPE_DOUBLE:
            case BV_OPT_TYPE_FLOAT: {
                double val;
                val = opt->default_val.dbl;
                write_number(s, opt, dst, val, 1, 1);
            }
            break;
            case BV_OPT_TYPE_RATIONAL: {
                BVRational val;
                val = bv_d2q(opt->default_val.dbl, INT_MAX);
                write_number(s, opt, dst, 1, val.den, val.num);
            }
            break;
            case BV_OPT_TYPE_COLOR:
                set_string_color(s, opt, opt->default_val.str, dst);
                break;
            case BV_OPT_TYPE_STRING:
                set_string(s, opt, opt->default_val.str, dst);
                break;
            case BV_OPT_TYPE_IMAGE_SIZE:
                set_string_image_size(s, opt, opt->default_val.str, dst);
                break;
            case BV_OPT_TYPE_VIDEO_RATE:
                set_string_video_rate(s, opt, opt->default_val.str, dst);
                break;
            case BV_OPT_TYPE_PIXEL_FMT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
                break;
            case BV_OPT_TYPE_SAMPLE_FMT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
                break;
            case BV_OPT_TYPE_BINARY:
                set_string_binary(s, opt, opt->default_val.str, dst);
                break;
            case BV_OPT_TYPE_DICT:
                /* Cannot set defaults for these types */
            break;
            default:
                bv_log(s, BV_LOG_DEBUG, "BVOption type %d of option %s not implemented yet\n", opt->type, opt->name);
        }
    }
}

/**
 * Store the value in the field in ctx that is named like key.
 * ctx must be an BVClass context, storing is done using BVOptions.
 *
 * @param buf the string to parse, buf will be updated to point at the
 * separator just after the parsed key/value pair
 * @param key_val_sep a 0-terminated list of characters used to
 * separate key from value
 * @param pairs_sep a 0-terminated list of characters used to separate
 * two pairs from each other
 * @return 0 if the key/value pair has been successfully parsed and
 * set, or a negative value corresponding to an BVERROR code in case
 * of error:
 * BVERROR(EINVAL) if the key/value pair cannot be parsed,
 * the error code issued by bv_opt_set() if the key/value pair
 * cannot be set
 */
static int parse_key_value_pair(void *ctx, const char **buf,
                                const char *key_val_sep, const char *pairs_sep)
{
    char *key = bv_get_token(buf, key_val_sep);
    char *val;
    int ret;

    if (!key)
        return BVERROR(ENOMEM);

    if (*key && strspn(*buf, key_val_sep)) {
        (*buf)++;
        val = bv_get_token(buf, pairs_sep);
        if (!val) {
            bv_freep(&key);
            return BVERROR(ENOMEM);
        }
    } else {
        bv_log(ctx, BV_LOG_ERROR, "Missing key or no key/value separator found after key '%s'\n", key);
        bv_free(key);
        return BVERROR(EINVAL);
    }

    bv_log(ctx, BV_LOG_DEBUG, "Setting entry with key '%s' to value '%s'\n", key, val);

    ret = bv_opt_set(ctx, key, val, BV_OPT_SEARCH_CHILDREN);
    if (ret == BVERROR_OPTION_NOT_FOUND)
        bv_log(ctx, BV_LOG_ERROR, "Key '%s' not found.\n", key);

    bv_free(key);
    bv_free(val);
    return ret;
}

int bv_set_options_string(void *ctx, const char *opts,
                          const char *key_val_sep, const char *pairs_sep)
{
    int ret, count = 0;

    if (!opts)
        return 0;

    while (*opts) {
        if ((ret = parse_key_value_pair(ctx, &opts, key_val_sep, pairs_sep)) < 0)
            return ret;
        count++;

        if (*opts)
            opts++;
    }

    return count;
}

#define WHITESPACES " \n\t"

static int is_key_char(char c)
{
    return (unsigned)((c | 32) - 'a') < 26 ||
           (unsigned)(c - '0') < 10 ||
           c == '-' || c == '_' || c == '/' || c == '.';
}

/**
 * Read a key from a string.
 *
 * The key consists of is_key_char characters and must be terminated by a
 * character from the delim string; spaces are ignored.
 *
 * @return  0 for success (even with ellipsis), <0 for failure
 */
static int get_key(const char **ropts, const char *delim, char **rkey)
{
    const char *opts = *ropts;
    const char *key_start, *key_end;

    key_start = opts += strspn(opts, WHITESPACES);
    while (is_key_char(*opts))
        opts++;
    key_end = opts;
    opts += strspn(opts, WHITESPACES);
    if (!*opts || !strchr(delim, *opts))
        return BVERROR(EINVAL);
    opts++;
    if (!(*rkey = bv_malloc(key_end - key_start + 1)))
        return BVERROR(ENOMEM);
    memcpy(*rkey, key_start, key_end - key_start);
    (*rkey)[key_end - key_start] = 0;
    *ropts = opts;
    return 0;
}

int bv_opt_get_key_value(const char **ropts,
                         const char *key_val_sep, const char *pairs_sep,
                         unsigned flags,
                         char **rkey, char **rval)
{
    int ret;
    char *key = NULL, *val;
    const char *opts = *ropts;

    if ((ret = get_key(&opts, key_val_sep, &key)) < 0 &&
        !(flags & BV_OPT_FLAG_IMPLICIT_KEY))
        return BVERROR(EINVAL);
    if (!(val = bv_get_token(&opts, pairs_sep))) {
        bv_free(key);
        return BVERROR(ENOMEM);
    }
    *ropts = opts;
    *rkey  = key;
    *rval  = val;
    return 0;
}

int bv_opt_set_from_string(void *ctx, const char *opts,
                           const char *const *shorthand,
                           const char *key_val_sep, const char *pairs_sep)
{
    int ret, count = 0;
    const char *dummy_shorthand = NULL;
    char *bv_uninit(parsed_key), *bv_uninit(value);
    const char *key;

    if (!opts)
        return 0;
    if (!shorthand)
        shorthand = &dummy_shorthand;

    while (*opts) {
        ret = bv_opt_get_key_value(&opts, key_val_sep, pairs_sep,
                                   *shorthand ? BV_OPT_FLAG_IMPLICIT_KEY : 0,
                                   &parsed_key, &value);
        if (ret < 0) {
            if (ret == BVERROR(EINVAL))
                bv_log(ctx, BV_LOG_ERROR, "No option name near '%s'\n", opts);
            else
                bv_log(ctx, BV_LOG_ERROR, "Unable to parse '%s': %s\n", opts,
                       bv_err2str(ret));
            return ret;
        }
        if (*opts)
            opts++;
        if (parsed_key) {
            key = parsed_key;
            while (*shorthand) /* discard all remaining shorthand */
                shorthand++;
        } else {
            key = *(shorthand++);
        }

        bv_log(ctx, BV_LOG_DEBUG, "Setting '%s' to value '%s'\n", key, value);
        if ((ret = bv_opt_set(ctx, key, value, 0)) < 0) {
            if (ret == BVERROR_OPTION_NOT_FOUND)
                bv_log(ctx, BV_LOG_ERROR, "Option '%s' not found\n", key);
            bv_free(value);
            bv_free(parsed_key);
            return ret;
        }

        bv_free(value);
        bv_free(parsed_key);
        count++;
    }
    return count;
}

void bv_opt_free(void *obj)
{
    const BVOption *o = NULL;
    while ((o = bv_opt_next(obj, o))) {
        switch (o->type) {
        case BV_OPT_TYPE_STRING:
        case BV_OPT_TYPE_BINARY:
            bv_freep((uint8_t *)obj + o->offset);
            break;

        case BV_OPT_TYPE_DICT:
            bv_dict_free((BVDictionary **)(((uint8_t *)obj) + o->offset));
            break;

        default:
            break;
        }
    }
}

int bv_opt_set_dict2(void *obj, BVDictionary **options, int search_flags)
{
    BVDictionaryEntry *t = NULL;
    BVDictionary    *tmp = NULL;
    int ret = 0;

    if (!options)
        return 0;

    while ((t = bv_dict_get(*options, "", t, BV_DICT_IGNORE_SUFFIX))) {
        ret = bv_opt_set(obj, t->key, t->value, search_flags);
        if (ret == BVERROR_OPTION_NOT_FOUND)
            bv_dict_set(&tmp, t->key, t->value, 0);
        else if (ret < 0) {
            bv_log(obj, BV_LOG_ERROR, "Error setting option %s to value %s.\n", t->key, t->value);
            break;
        }
        ret = 0;
    }
    bv_dict_free(options);
    *options = tmp;
    return ret;
}

int bv_opt_set_dict(void *obj, BVDictionary **options)
{
    return bv_opt_set_dict2(obj, options, 0);
}

const BVOption *bv_opt_find(void *obj, const char *name, const char *unit,
                            int opt_flags, int search_flags)
{
    return bv_opt_find2(obj, name, unit, opt_flags, search_flags, NULL);
}

const BVOption *bv_opt_find2(void *obj, const char *name, const char *unit,
                             int opt_flags, int search_flags, void **target_obj)
{
    const BVClass  *c;
    const BVOption *o = NULL;

    if(!obj)
        return NULL;

    c= *(BVClass**)obj;

    if (!c)
        return NULL;

    if (search_flags & BV_OPT_SEARCH_CHILDREN) {
        if (search_flags & BV_OPT_SEARCH_FAKE_OBJ) {
            const BVClass *child = NULL;
            while (child = bv_opt_child_class_next(c, child))
                if (o = bv_opt_find2(&child, name, unit, opt_flags, search_flags, NULL))
                    return o;
        } else {
            void *child = NULL;
            while (child = bv_opt_child_next(obj, child))
                if (o = bv_opt_find2(child, name, unit, opt_flags, search_flags, target_obj))
                    return o;
        }
    }

    while (o = bv_opt_next(obj, o)) {
        if (!strcmp(o->name, name) && (o->flags & opt_flags) == opt_flags &&
            ((!unit && o->type != BV_OPT_TYPE_CONST) ||
             (unit  && o->type == BV_OPT_TYPE_CONST && o->unit && !strcmp(o->unit, unit)))) {
            if (target_obj) {
                if (!(search_flags & BV_OPT_SEARCH_FAKE_OBJ))
                    *target_obj = obj;
                else
                    *target_obj = NULL;
            }
            return o;
        }
    }
    return NULL;
}

void *bv_opt_child_next(void *obj, void *prev)
{
    const BVClass *c = *(BVClass**)obj;
    if (c->child_next)
        return c->child_next(obj, prev);
    return NULL;
}

const BVClass *bv_opt_child_class_next(const BVClass *parent, const BVClass *prev)
{
    if (parent->child_class_next)
        return parent->child_class_next(prev);
    return NULL;
}

void *bv_opt_ptr(const BVClass *class, void *obj, const char *name)
{
    const BVOption *opt= bv_opt_find2(&class, name, NULL, 0, BV_OPT_SEARCH_FAKE_OBJ, NULL);
    if(!opt)
        return NULL;
    return (uint8_t*)obj + opt->offset;
}

static int opt_size(enum BVOptionType type)
{
    switch(type) {
    case BV_OPT_TYPE_INT:
    case BV_OPT_TYPE_FLAGS:     return sizeof(int);
    case BV_OPT_TYPE_DURATION:
    case BV_OPT_TYPE_CHANNEL_LAYOUT:
    case BV_OPT_TYPE_INT64:     return sizeof(int64_t);
    case BV_OPT_TYPE_DOUBLE:    return sizeof(double);
    case BV_OPT_TYPE_FLOAT:     return sizeof(float);
    case BV_OPT_TYPE_STRING:    return sizeof(uint8_t*);
    case BV_OPT_TYPE_VIDEO_RATE:
    case BV_OPT_TYPE_RATIONAL:  return sizeof(BVRational);
    case BV_OPT_TYPE_BINARY:    return sizeof(uint8_t*) + sizeof(int);
    case BV_OPT_TYPE_IMAGE_SIZE:return sizeof(int[2]);
    case BV_OPT_TYPE_PIXEL_FMT: return sizeof(enum BVPixelFormat);
    case BV_OPT_TYPE_SAMPLE_FMT:return sizeof(enum BVSampleFormat);
    case BV_OPT_TYPE_COLOR:     return 4;
    }
    return 0;
}

int bv_opt_copy(void *dst, FF_CONST_BVUTIL55 void *src)
{
    const BVOption *o = NULL;
    const BVClass *c;
    int ret = 0;

    if (!src)
        return 0;

    c = *(BVClass**)src;
    if (*(BVClass**)dst && c != *(BVClass**)dst)
        return BVERROR(EINVAL);

    while ((o = bv_opt_next(src, o))) {
        void *field_dst = ((uint8_t*)dst) + o->offset;
        void *field_src = ((uint8_t*)src) + o->offset;
        uint8_t **field_dst8 = (uint8_t**)field_dst;
        uint8_t **field_src8 = (uint8_t**)field_src;

        if (o->type == BV_OPT_TYPE_STRING) {
            if (*field_dst8 != *field_src8)
                bv_freep(field_dst8);
            *field_dst8 = bv_strdup(*field_src8);
            if (*field_src8 && !*field_dst8)
                ret = BVERROR(ENOMEM);
        } else if (o->type == BV_OPT_TYPE_BINARY) {
            int len = *(int*)(field_src8 + 1);
            if (*field_dst8 != *field_src8)
                bv_freep(field_dst8);
            *field_dst8 = bv_memdup(*field_src8, len);
            if (len && !*field_dst8) {
                ret = BVERROR(ENOMEM);
                len = 0;
            }
            *(int*)(field_dst8 + 1) = len;
        } else if (o->type == BV_OPT_TYPE_CONST) {
            // do nothing
        } else if (o->type == BV_OPT_TYPE_DICT) {
            BVDictionary **sdict = (BVDictionary **) field_src;
            BVDictionary **ddict = (BVDictionary **) field_dst;
            if (*sdict != *ddict)
                bv_dict_free(ddict);
            *ddict = NULL;
            bv_dict_copy(ddict, *sdict, 0);
            if (bv_dict_count(*sdict) != bv_dict_count(*ddict))
                ret = BVERROR(ENOMEM);
        } else {
            memcpy(field_dst, field_src, opt_size(o->type));
        }
    }
    return ret;
}

int bv_opt_query_ranges(BVOptionRanges **ranges_arg, void *obj, const char *key, int flags)
{
    int ret;
    const BVClass *c = *(BVClass**)obj;
    int (*callback)(BVOptionRanges **, void *obj, const char *key, int flags) = NULL;

    if (c->version > (52 << 16 | 11 << 8))
        callback = c->query_ranges;

    if (!callback)
        callback = bv_opt_query_ranges_default;

    ret = callback(ranges_arg, obj, key, flags);
    if (ret >= 0) {
        if (!(flags & BV_OPT_MULTI_COMPONENT_RANGE))
            ret = 1;
        (*ranges_arg)->nb_components = ret;
    }
    return ret;
}

int bv_opt_query_ranges_default(BVOptionRanges **ranges_arg, void *obj, const char *key, int flags)
{
    BVOptionRanges *ranges = bv_mallocz(sizeof(*ranges));
    BVOptionRange **range_array = bv_mallocz(sizeof(void*));
    BVOptionRange *range = bv_mallocz(sizeof(*range));
    const BVOption *field = bv_opt_find(obj, key, NULL, 0, flags);
    int ret;

    *ranges_arg = NULL;

    if (!ranges || !range || !range_array || !field) {
        ret = BVERROR(ENOMEM);
        goto fail;
    }

    ranges->range = range_array;
    ranges->range[0] = range;
    ranges->nb_ranges = 1;
    ranges->nb_components = 1;
    range->is_range = 1;
    range->value_min = field->min;
    range->value_max = field->max;

    switch (field->type) {
    case BV_OPT_TYPE_INT:
    case BV_OPT_TYPE_INT64:
    case BV_OPT_TYPE_PIXEL_FMT:
    case BV_OPT_TYPE_SAMPLE_FMT:
    case BV_OPT_TYPE_FLOAT:
    case BV_OPT_TYPE_DOUBLE:
    case BV_OPT_TYPE_DURATION:
    case BV_OPT_TYPE_COLOR:
    case BV_OPT_TYPE_CHANNEL_LAYOUT:
        break;
    case BV_OPT_TYPE_STRING:
        range->component_min = 0;
        range->component_max = 0x10FFFF; // max unicode value
        range->value_min = -1;
        range->value_max = INT_MAX;
        break;
    case BV_OPT_TYPE_RATIONAL:
        range->component_min = INT_MIN;
        range->component_max = INT_MAX;
        break;
    case BV_OPT_TYPE_IMAGE_SIZE:
        range->component_min = 0;
        range->component_max = INT_MAX/128/8;
        range->value_min = 0;
        range->value_max = INT_MAX/8;
        break;
    case BV_OPT_TYPE_VIDEO_RATE:
        range->component_min = 1;
        range->component_max = INT_MAX;
        range->value_min = 1;
        range->value_max = INT_MAX;
        break;
    default:
        ret = BVERROR(ENOSYS);
        goto fail;
    }

    *ranges_arg = ranges;
    return 1;
fail:
    bv_free(ranges);
    bv_free(range);
    bv_free(range_array);
    return ret;
}

void bv_opt_freep_ranges(BVOptionRanges **rangesp)
{
    int i;
    BVOptionRanges *ranges = *rangesp;

    if (!ranges)
        return;

    for (i = 0; i < ranges->nb_ranges * ranges->nb_components; i++) {
        BVOptionRange *range = ranges->range[i];
        if (range) {
            bv_freep(&range->str);
            bv_freep(&ranges->range[i]);
        }
    }
    bv_freep(&ranges->range);
    bv_freep(rangesp);
}

int bv_opt_is_set_to_default(void *obj, const BVOption *o)
{
    int64_t i64;
    double d, d2;
    float f;
    BVRational q;
    int ret, w, h;
    char *str;
    void *dst;

    if (!o || !obj)
        return BVERROR(EINVAL);

    dst = ((uint8_t*)obj) + o->offset;

    switch (o->type) {
    case BV_OPT_TYPE_CONST:
        return 1;
    case BV_OPT_TYPE_FLAGS:
    case BV_OPT_TYPE_PIXEL_FMT:
    case BV_OPT_TYPE_SAMPLE_FMT:
    case BV_OPT_TYPE_INT:
    case BV_OPT_TYPE_CHANNEL_LAYOUT:
    case BV_OPT_TYPE_DURATION:
    case BV_OPT_TYPE_INT64:
        read_number(o, dst, NULL, NULL, &i64);
        return o->default_val.i64 == i64;
    case BV_OPT_TYPE_STRING:
        str = *(char **)dst;
        if (str == o->default_val.str) //2 NULLs
            return 1;
        if (!str || !o->default_val.str) //1 NULL
            return 0;
        return !strcmp(str, o->default_val.str);
    case BV_OPT_TYPE_DOUBLE:
        read_number(o, dst, &d, NULL, NULL);
        return o->default_val.dbl == d;
    case BV_OPT_TYPE_FLOAT:
        read_number(o, dst, &d, NULL, NULL);
        f = o->default_val.dbl;
        d2 = f;
        return d2 == d;
    case BV_OPT_TYPE_RATIONAL:
        q = bv_d2q(o->default_val.dbl, INT_MAX);
        return !bv_cmp_q(*(BVRational*)dst, q);
    case BV_OPT_TYPE_BINARY: {
        struct {
            uint8_t *data;
            int size;
        } tmp = {0};
        int opt_size = *(int *)((void **)dst + 1);
        void *opt_ptr = *(void **)dst;
        if (!opt_size && (!o->default_val.str || !strlen(o->default_val.str)))
            return 1;
        if (!opt_size ||  !o->default_val.str || !strlen(o->default_val.str ))
            return 0;
        if (opt_size != strlen(o->default_val.str) / 2)
            return 0;
        ret = set_string_binary(NULL, NULL, o->default_val.str, &tmp.data);
        if (!ret)
            ret = !memcmp(opt_ptr, tmp.data, tmp.size);
        bv_free(tmp.data);
        return ret;
    }
    case BV_OPT_TYPE_DICT:
        /* Binary and dict have not default support yet. Any pointer is not default. */
        return !!(*(void **)dst);
    case BV_OPT_TYPE_IMAGE_SIZE:
        if (!o->default_val.str || !strcmp(o->default_val.str, "none"))
            w = h = 0;
        else if ((ret = bv_parse_video_size(&w, &h, o->default_val.str)) < 0)
            return ret;
        return (w == *(int *)dst) && (h == *((int *)dst+1));
    case BV_OPT_TYPE_VIDEO_RATE:
        q = (BVRational){0, 0};
        if (o->default_val.str) {
            if ((ret = bv_parse_video_rate(&q, o->default_val.str)) < 0)
                return ret;
        }
        return !bv_cmp_q(*(BVRational*)dst, q);
    case BV_OPT_TYPE_COLOR: {
        uint8_t color[4] = {0, 0, 0, 0};
        if (o->default_val.str) {
            if ((ret = bv_parse_color(color, o->default_val.str, -1, NULL)) < 0)
                return ret;
        }
        return !memcmp(color, dst, sizeof(color));
    }
    default:
        bv_log(obj, BV_LOG_WARNING, "Not supported option type: %d, option name: %s\n", o->type, o->name);
        break;
    }
    return BVERROR_PATCHWELCOME;
}

int bv_opt_is_set_to_default_by_name(void *obj, const char *name, int search_flags)
{
    const BVOption *o;
    void *target;
    if (!obj)
        return BVERROR(EINVAL);
    o = bv_opt_find2(obj, name, NULL, 0, search_flags, &target);
    if (!o)
        return BVERROR_OPTION_NOT_FOUND;
    return bv_opt_is_set_to_default(target, o);
}

int bv_opt_serialize(void *obj, int opt_flags, int flags, char **buffer,
                     const char key_val_sep, const char pairs_sep)
{
    const BVOption *o = NULL;
    uint8_t *buf;
    BVBPrint bprint;
    int ret, cnt = 0;
    const char special_chars[] = {pairs_sep, key_val_sep, '\0'};

    if (pairs_sep == '\0' || key_val_sep == '\0' || pairs_sep == key_val_sep ||
        pairs_sep == '\\' || key_val_sep == '\\') {
        bv_log(obj, BV_LOG_ERROR, "Invalid separator(s) found.");
        return BVERROR(EINVAL);
    }

    if (!obj || !buffer)
        return BVERROR(EINVAL);

    *buffer = NULL;
    bv_bprint_init(&bprint, 64, BV_BPRINT_SIZE_UNLIMITED);

    while (o = bv_opt_next(obj, o)) {
        if (o->type == BV_OPT_TYPE_CONST)
            continue;
        if ((flags & BV_OPT_SERIALIZE_OPT_FLAGS_EXACT) && o->flags != opt_flags)
            continue;
        else if (((o->flags & opt_flags) != opt_flags))
            continue;
        if (flags & BV_OPT_SERIALIZE_SKIP_DEFAULTS && bv_opt_is_set_to_default(obj, o) > 0)
            continue;
        if ((ret = bv_opt_get(obj, o->name, 0, &buf)) < 0) {
            bv_bprint_finalize(&bprint, NULL);
            return ret;
        }
        if (buf) {
            if (cnt++)
                bv_bprint_append_data(&bprint, &pairs_sep, 1);
            bv_bprint_escape(&bprint, o->name, special_chars, BV_ESCAPE_MODE_BACKSLASH, 0);
            bv_bprint_append_data(&bprint, &key_val_sep, 1);
            bv_bprint_escape(&bprint, buf, special_chars, BV_ESCAPE_MODE_BACKSLASH, 0);
            bv_freep(&buf);
        }
    }
    bv_bprint_finalize(&bprint, buffer);
    return 0;
}

#ifdef TEST

typedef struct TestContext
{
    const BVClass *class;
    int num;
    int toggle;
    char *string;
    int flags;
    BVRational rational;
    BVRational video_rate;
    int w, h;
    enum BVPixelFormat pix_fmt;
    enum BVSampleFormat sample_fmt;
    int64_t duration;
    uint8_t color[4];
    int64_t channel_layout;
    void *binary;
    int binary_size;
    void *binary1;
    int binary_size1;
    void *binary2;
    int binary_size2;
    int64_t num64;
    float flt;
    double dbl;
    char *escape;
} TestContext;

#define OFFSET(x) offsetof(TestContext, x)

#define TEST_FLAG_COOL 01
#define TEST_FLAG_LAME 02
#define TEST_FLAG_MU   04

static const BVOption test_options[]= {
{"num",      "set num",        OFFSET(num),      BV_OPT_TYPE_INT,      {.i64 = 0},       0,        100                 },
{"toggle",   "set toggle",     OFFSET(toggle),   BV_OPT_TYPE_INT,      {.i64 = 1},       0,        1                   },
{"rational", "set rational",   OFFSET(rational), BV_OPT_TYPE_RATIONAL, {.dbl = 1},       0,        10                  },
{"string",   "set string",     OFFSET(string),   BV_OPT_TYPE_STRING,   {.str = "default"}, CHAR_MIN, CHAR_MAX          },
{"escape",   "set escape str", OFFSET(escape),   BV_OPT_TYPE_STRING,   {.str = "\\=,"}, CHAR_MIN, CHAR_MAX             },
{"flags",    "set flags",      OFFSET(flags),    BV_OPT_TYPE_FLAGS,    {.i64 = 1},       0,        INT_MAX, 0, "flags" },
{"cool",     "set cool flag ", 0,                BV_OPT_TYPE_CONST,    {.i64 = TEST_FLAG_COOL}, INT_MIN,  INT_MAX, 0, "flags" },
{"lame",     "set lame flag ", 0,                BV_OPT_TYPE_CONST,    {.i64 = TEST_FLAG_LAME}, INT_MIN,  INT_MAX, 0, "flags" },
{"mu",       "set mu flag ",   0,                BV_OPT_TYPE_CONST,    {.i64 = TEST_FLAG_MU},   INT_MIN,  INT_MAX, 0, "flags" },
{"size",     "set size",       OFFSET(w),        BV_OPT_TYPE_IMAGE_SIZE,{.str="200x300"},             0,        0                   },
{"pix_fmt",  "set pixfmt",     OFFSET(pix_fmt),  BV_OPT_TYPE_PIXEL_FMT, {.i64 = BV_PIX_FMT_0BGR}, -1, INT_MAX},
{"sample_fmt", "set samplefmt", OFFSET(sample_fmt), BV_OPT_TYPE_SAMPLE_FMT, {.i64 = BV_SAMPLE_FMT_S16}, -1, INT_MAX},
{"video_rate", "set videorate", OFFSET(video_rate), BV_OPT_TYPE_VIDEO_RATE,  {.str = "25"}, 0,     0                   },
{"duration", "set duration",   OFFSET(duration), BV_OPT_TYPE_DURATION, {.i64 = 1000}, 0, INT64_MAX},
{"color", "set color",   OFFSET(color), BV_OPT_TYPE_COLOR, {.str = "pink"}, 0, 0},
{"cl", "set channel layout", OFFSET(channel_layout), BV_OPT_TYPE_CHANNEL_LAYOUT, {.i64 = BV_CH_LAYOUT_HEXAGONAL}, 0, INT64_MAX},
{"bin", "set binary value",    OFFSET(binary),   BV_OPT_TYPE_BINARY,   {.str="62696e00"}, 0,        0 },
{"bin1", "set binary value",   OFFSET(binary1),  BV_OPT_TYPE_BINARY,   {.str=NULL},       0,        0 },
{"bin2", "set binary value",   OFFSET(binary2),  BV_OPT_TYPE_BINARY,   {.str=""},         0,        0 },
{"num64",    "set num 64bit",  OFFSET(num64),    BV_OPT_TYPE_INT64,    {.i64 = 1},        0,        100 },
{"flt",      "set float",      OFFSET(flt),      BV_OPT_TYPE_FLOAT,    {.dbl = 1.0/3},    0,        100 },
{"dbl",      "set double",     OFFSET(dbl),      BV_OPT_TYPE_DOUBLE,   {.dbl = 1.0/3},    0,        100 },
{NULL},
};

static const char *test_get_name(void *ctx)
{
    return "test";
}

static const BVClass test_class = {
    "TestContext",
    test_get_name,
    test_options
};

int main(void)
{
    int i;

    printf("Testing default values\n");
    {
        TestContext test_ctx = { 0 };
        test_ctx.class = &test_class;
        bv_opt_set_defaults(&test_ctx);

        printf("num=%d\n", test_ctx.num);
        printf("toggle=%d\n", test_ctx.toggle);
        printf("string=%s\n", test_ctx.string);
        printf("escape=%s\n", test_ctx.escape);
        printf("flags=%d\n", test_ctx.flags);
        printf("rational=%d/%d\n", test_ctx.rational.num, test_ctx.rational.den);
        printf("video_rate=%d/%d\n", test_ctx.video_rate.num, test_ctx.video_rate.den);
        printf("width=%d height=%d\n", test_ctx.w, test_ctx.h);
        printf("pix_fmt=%s\n", bv_get_pix_fmt_name(test_ctx.pix_fmt));
        printf("sample_fmt=%s\n", bv_get_sample_fmt_name(test_ctx.sample_fmt));
        printf("duration=%"PRId64"\n", test_ctx.duration);
        printf("color=%d %d %d %d\n", test_ctx.color[0], test_ctx.color[1], test_ctx.color[2], test_ctx.color[3]);
        printf("channel_layout=%"PRId64"=%"PRId64"\n", test_ctx.channel_layout, (int64_t)BV_CH_LAYOUT_HEXAGONAL);
        if (test_ctx.binary)
            printf("binary=%x %x %x %x\n", ((uint8_t*)test_ctx.binary)[0], ((uint8_t*)test_ctx.binary)[1], ((uint8_t*)test_ctx.binary)[2], ((uint8_t*)test_ctx.binary)[3]);
        printf("binary_size=%d\n", test_ctx.binary_size);
        printf("num64=%"PRId64"\n", test_ctx.num64);
        printf("flt=%.6f\n", test_ctx.flt);
        printf("dbl=%.6f\n", test_ctx.dbl);
        bv_opt_free(&test_ctx);
    }

    printf("\nTesting bv_opt_is_set_to_default()\n");
    {
        int ret;
        TestContext test_ctx = { 0 };
        const BVOption *o = NULL;
        test_ctx.class = &test_class;

        bv_log_set_level(BV_LOG_QUIET);

        while (o = bv_opt_next(&test_ctx, o)) {
            ret = bv_opt_is_set_to_default_by_name(&test_ctx, o->name, 0);
            printf("name:%10s default:%d error:%s\n", o->name, !!ret, ret < 0 ? bv_err2str(ret) : "");
        }
        bv_opt_set_defaults(&test_ctx);
        while (o = bv_opt_next(&test_ctx, o)) {
            ret = bv_opt_is_set_to_default_by_name(&test_ctx, o->name, 0);
            printf("name:%10s default:%d error:%s\n", o->name, !!ret, ret < 0 ? bv_err2str(ret) : "");
        }
        bv_opt_free(&test_ctx);
    }

    printf("\nTest bv_opt_serialize()\n");
    {
        TestContext test_ctx = { 0 };
        char *buf;
        test_ctx.class = &test_class;

        bv_log_set_level(BV_LOG_QUIET);

        bv_opt_set_defaults(&test_ctx);
        if (bv_opt_serialize(&test_ctx, 0, 0, &buf, '=', ',') >= 0) {
            printf("%s\n", buf);
            bv_opt_free(&test_ctx);
            memset(&test_ctx, 0, sizeof(test_ctx));
            test_ctx.class = &test_class;
            bv_set_options_string(&test_ctx, buf, "=", ",");
            bv_free(buf);
            if (bv_opt_serialize(&test_ctx, 0, 0, &buf, '=', ',') >= 0) {
                printf("%s\n", buf);
                bv_free(buf);
            }
        }
        bv_opt_free(&test_ctx);
    }

    printf("\nTesting bv_set_options_string()\n");
    {
        TestContext test_ctx = { 0 };
        static const char * const options[] = {
            "",
            ":",
            "=",
            "foo=:",
            ":=foo",
            "=foo",
            "foo=",
            "foo",
            "foo=val",
            "foo==val",
            "toggle=:",
            "string=:",
            "toggle=1 : foo",
            "toggle=100",
            "toggle==1",
            "flags=+mu-lame : num=42: toggle=0",
            "num=42 : string=blahblah",
            "rational=0 : rational=1/2 : rational=1/-1",
            "rational=-1/0",
            "size=1024x768",
            "size=pal",
            "size=bogus",
            "pix_fmt=yuv420p",
            "pix_fmt=2",
            "pix_fmt=bogus",
            "sample_fmt=s16",
            "sample_fmt=2",
            "sample_fmt=bogus",
            "video_rate=pal",
            "video_rate=25",
            "video_rate=30000/1001",
            "video_rate=30/1.001",
            "video_rate=bogus",
            "duration=bogus",
            "duration=123.45",
            "duration=1\\:23\\:45.67",
            "color=blue",
            "color=0x223300",
            "color=0x42FF07AA",
            "cl=stereo+downmix",
            "cl=foo",
            "bin=boguss",
            "bin=111",
            "bin=ffff",
            "num64=bogus",
            "num64=44",
            "num64=44.4",
            "num64=-1",
            "num64=101",
            "flt=bogus",
            "flt=2",
            "flt=2.2",
            "flt=-1",
            "flt=101",
            "dbl=bogus",
            "dbl=2",
            "dbl=2.2",
            "dbl=-1",
            "dbl=101",
        };

        test_ctx.class = &test_class;
        bv_opt_set_defaults(&test_ctx);

        bv_log_set_level(BV_LOG_QUIET);

        for (i=0; i < FF_ARRAY_ELEMS(options); i++) {
            bv_log(&test_ctx, BV_LOG_DEBUG, "Setting options string '%s'\n", options[i]);
            if (bv_set_options_string(&test_ctx, options[i], "=", ":") < 0)
                printf("Error '%s'\n", options[i]);
            else
                printf("OK    '%s'\n", options[i]);
        }
        bv_opt_free(&test_ctx);
    }

    printf("\nTesting bv_opt_set_from_string()\n");
    {
        TestContext test_ctx = { 0 };
        static const char * const options[] = {
            "",
            "5",
            "5:hello",
            "5:hello:size=pal",
            "5:size=pal:hello",
            ":",
            "=",
            " 5 : hello : size = pal ",
            "a_very_long_option_name_that_will_need_to_be_ellipsized_around_here=42"
        };
        static const char * const shorthand[] = { "num", "string", NULL };

        test_ctx.class = &test_class;
        bv_opt_set_defaults(&test_ctx);

        bv_log_set_level(BV_LOG_QUIET);

        for (i=0; i < FF_ARRAY_ELEMS(options); i++) {
            bv_log(&test_ctx, BV_LOG_DEBUG, "Setting options string '%s'\n", options[i]);
            if (bv_opt_set_from_string(&test_ctx, options[i], shorthand, "=", ":") < 0)
                printf("Error '%s'\n", options[i]);
            else
                printf("OK    '%s'\n", options[i]);
        }
        bv_opt_free(&test_ctx);
    }

    return 0;
}

#endif
