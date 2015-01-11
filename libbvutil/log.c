/*
 * log functions
 * Copyright (c) 2003 Michel Bardiaux
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
 * logging functions
 */

#include "config.h"

#if BV_HAVE_UNISTD_H
#include <unistd.h>
#endif
#if BV_HAVE_IO_H
#include <io.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include "bvutil.h"
#include "bprint.h"
#include "common.h"
#include "internal.h"
#include "log.h"

#if BV_HAVE_PTHREADS
#include "thread.h"
static BVMutex mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#define LINE_SZ 1024

static int bv_log_level = BV_LOG_INFO;
static int flags;

#if defined(_WIN32) && !defined(__MINGW32CE__) && BV_HAVE_SETCONSOLETEXTATTRIBUTE
#include <windows.h>
static const uint8_t color[16 + BV_CLASS_CATEGORY_NB] = {
    [BV_LOG_PANIC  /8] = 12,
    [BV_LOG_FATAL  /8] = 12,
    [BV_LOG_ERROR  /8] = 12,
    [BV_LOG_WARNING/8] = 14,
    [BV_LOG_INFO   /8] =  7,
    [BV_LOG_VERBOSE/8] = 10,
    [BV_LOG_DEBUG  /8] = 10,
    [16+BV_CLASS_CATEGORY_NA              ] =  7,
    [16+BV_CLASS_CATEGORY_INPUT           ] = 13,
    [16+BV_CLASS_CATEGORY_OUTPUT          ] =  5,
    [16+BV_CLASS_CATEGORY_MUXER           ] = 13,
    [16+BV_CLASS_CATEGORY_DEMUXER         ] =  5,
    [16+BV_CLASS_CATEGORY_ENCODER         ] = 11,
    [16+BV_CLASS_CATEGORY_DECODER         ] =  3,
    [16+BV_CLASS_CATEGORY_FILTER          ] = 10,
    [16+BV_CLASS_CATEGORY_BITSTREAM_FILTER] =  9,
    [16+BV_CLASS_CATEGORY_SWSCALER        ] =  7,
    [16+BV_CLASS_CATEGORY_SWRESAMPLER     ] =  7,
    [16+BV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT ] = 13,
    [16+BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT  ] = 5,
    [16+BV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT ] = 13,
    [16+BV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT  ] = 5,
    [16+BV_CLASS_CATEGORY_DEVICE_OUTPUT       ] = 13,
    [16+BV_CLASS_CATEGORY_DEVICE_INPUT        ] = 5,
};

static int16_t background, attr_orig;
static HANDLE con;
#else

static const uint32_t color[16 + BV_CLASS_CATEGORY_NB] = {
    [BV_LOG_PANIC  /8] =  52 << 16 | 196 << 8 | 0x41,
    [BV_LOG_FATAL  /8] = 208 <<  8 | 0x41,
    [BV_LOG_ERROR  /8] = 196 <<  8 | 0x11,
    [BV_LOG_WARNING/8] = 226 <<  8 | 0x03,
    [BV_LOG_INFO   /8] = 253 <<  8 | 0x09,
    [BV_LOG_VERBOSE/8] =  40 <<  8 | 0x02,
    [BV_LOG_DEBUG  /8] =  34 <<  8 | 0x02,
    [16+BV_CLASS_CATEGORY_NA              ] = 250 << 8 | 0x09,
    [16+BV_CLASS_CATEGORY_INPUT           ] = 219 << 8 | 0x15,
    [16+BV_CLASS_CATEGORY_OUTPUT          ] = 201 << 8 | 0x05,
    [16+BV_CLASS_CATEGORY_MUXER           ] = 213 << 8 | 0x15,
    [16+BV_CLASS_CATEGORY_DEMUXER         ] = 207 << 8 | 0x05,
    [16+BV_CLASS_CATEGORY_ENCODER         ] =  51 << 8 | 0x16,
    [16+BV_CLASS_CATEGORY_DECODER         ] =  39 << 8 | 0x06,
    [16+BV_CLASS_CATEGORY_FILTER          ] = 155 << 8 | 0x12,
    [16+BV_CLASS_CATEGORY_BITSTREAM_FILTER] = 192 << 8 | 0x14,
    [16+BV_CLASS_CATEGORY_SWSCALER        ] = 153 << 8 | 0x14,
    [16+BV_CLASS_CATEGORY_SWRESAMPLER     ] = 147 << 8 | 0x14,
    [16+BV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT ] = 213 << 8 | 0x15,
    [16+BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT  ] = 207 << 8 | 0x05,
    [16+BV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT ] = 213 << 8 | 0x15,
    [16+BV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT  ] = 207 << 8 | 0x05,
    [16+BV_CLASS_CATEGORY_DEVICE_OUTPUT       ] = 213 << 8 | 0x15,
    [16+BV_CLASS_CATEGORY_DEVICE_INPUT        ] = 207 << 8 | 0x05,
};

#endif
static int use_color = -1;

static void check_color_terminal(void)
{
#if defined(_WIN32) && !defined(__MINGW32CE__) && BV_HAVE_SETCONSOLETEXTATTRIBUTE
    CONSOLE_SCREEN_BUFFER_INFO con_info;
    con = GetStdHandle(STD_ERROR_HANDLE);
    use_color = (con != INVALID_HANDLE_VALUE) && !getenv("NO_COLOR") &&
                !getenv("BV_LOG_FORCE_NOCOLOR");
    if (use_color) {
        GetConsoleScreenBufferInfo(con, &con_info);
        attr_orig  = con_info.wAttributes;
        background = attr_orig & 0xF0;
    }
#elif BV_HAVE_ISATTY
    char *term = getenv("TERM");
    use_color = !getenv("NO_COLOR") && !getenv("BV_LOG_FORCE_NOCOLOR") &&
                (getenv("TERM") && isatty(2) || getenv("BV_LOG_FORCE_COLOR"));
    if (   getenv("BV_LOG_FORCE_256COLOR")
        || (term && strstr(term, "256color")))
        use_color *= 256;
#else
    use_color = getenv("BV_LOG_FORCE_COLOR") && !getenv("NO_COLOR") &&
               !getenv("BV_LOG_FORCE_NOCOLOR");
#endif
}

static void colored_fputs(int level, int tint, const char *str)
{
    int local_use_color;
    if (!*str)
        return;

    if (use_color < 0)
        check_color_terminal();

    if (level == BV_LOG_INFO/8) local_use_color = 0;
    else                        local_use_color = use_color;

#if defined(_WIN32) && !defined(__MINGW32CE__) && BV_HAVE_SETCONSOLETEXTATTRIBUTE
    if (local_use_color)
        SetConsoleTextAttribute(con, background | color[level]);
    fputs(str, stderr);
    if (local_use_color)
        SetConsoleTextAttribute(con, attr_orig);
#else
    if (local_use_color == 1) {
        fprintf(stderr,
                "\033[%d;3%dm%s\033[0m",
                (color[level] >> 4) & 15,
                color[level] & 15,
                str);
    } else if (tint && use_color == 256) {
        fprintf(stderr,
                "\033[48;5;%dm\033[38;5;%dm%s\033[0m",
                (color[level] >> 16) & 0xff,
                tint,
                str);
    } else if (local_use_color == 256) {
        fprintf(stderr,
                "\033[48;5;%dm\033[38;5;%dm%s\033[0m",
                (color[level] >> 16) & 0xff,
                (color[level] >> 8) & 0xff,
                str);
    } else
        fputs(str, stderr);
#endif

}

const char *bv_default_item_name(void *ptr)
{
    return (*(BVClass **) ptr)->class_name;
}

BVClassCategory bv_default_get_category(void *ptr)
{
    return (*(BVClass **) ptr)->category;
}

static void sanitize(uint8_t *line){
    while(*line){
        if(*line < 0x08 || (*line > 0x0D && *line < 0x20))
            *line='?';
        line++;
    }
}

static int get_category(void *ptr){
    BVClass *avc = *(BVClass **) ptr;
    if(    !avc
        || (avc->version&0xFF)<100
        ||  avc->version < (51 << 16 | 59 << 8)
        ||  avc->category >= BV_CLASS_CATEGORY_NB) return BV_CLASS_CATEGORY_NA + 16;

    if(avc->get_category)
        return avc->get_category(ptr) + 16;

    return avc->category + 16;
}

static const char *get_level_str(int level)
{
    switch (level) {
    case BV_LOG_QUIET:
        return "quiet";
    case BV_LOG_DEBUG:
        return "debug";
    case BV_LOG_VERBOSE:
        return "verbose";
    case BV_LOG_INFO:
        return "info";
    case BV_LOG_WARNING:
        return "warning";
    case BV_LOG_ERROR:
        return "error";
    case BV_LOG_FATAL:
        return "fatal";
    case BV_LOG_PANIC:
        return "panic";
    default:
        return "";
    }
}

static void format_line(void *avcl, int level, const char *fmt, va_list vl,
                        BVBPrint part[4], int *print_prefix, int type[2])
{
    BVClass* avc = avcl ? *(BVClass **) avcl : NULL;
    bv_bprint_init(part+0, 0, 1);
    bv_bprint_init(part+1, 0, 1);
    bv_bprint_init(part+2, 0, 1);
    bv_bprint_init(part+3, 0, 65536);

    if(type) type[0] = type[1] = BV_CLASS_CATEGORY_NA + 16;
    if (*print_prefix && avc) {
        if (avc->parent_log_context_offset) {
            BVClass** parent = *(BVClass ***) (((uint8_t *) avcl) +
                                   avc->parent_log_context_offset);
            if (parent && *parent) {
                bv_bprintf(part+0, "[%s @ %p] ",
                         (*parent)->item_name(parent), parent);
                if(type) type[0] = get_category(parent);
            }
        }
        bv_bprintf(part+1, "[%s @ %p] ",
                 avc->item_name(avcl), avcl);
        if(type) type[1] = get_category(avcl);

        if (flags & BV_LOG_PRINT_LEVEL)
            bv_bprintf(part+2, "[%s] ", get_level_str(level));
    }

    bv_vbprintf(part+3, fmt, vl);

    if(*part[0].str || *part[1].str || *part[2].str || *part[3].str) {
        char lastc = part[3].len && part[3].len <= part[3].size ? part[3].str[part[3].len - 1] : 0;
        *print_prefix = lastc == '\n' || lastc == '\r';
    }
}

void bv_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix)
{
    BVBPrint part[4];
    format_line(ptr, level, fmt, vl, part, print_prefix, NULL);
    snprintf(line, line_size, "%s%s%s%s", part[0].str, part[1].str, part[2].str, part[3].str);
    bv_bprint_finalize(part+3, NULL);
}

void bv_log_default_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    static int print_prefix = 1;
    static int count;
    static char prev[LINE_SZ];
    BVBPrint part[4];
    char line[LINE_SZ];
    static int is_atty;
    int type[2];
    unsigned tint = 0;

    if (level >= 0) {
        tint = level & 0xff00;
        level &= 0xff;
    }

    if (level > bv_log_level)
        return;
#if BV_HAVE_PTHREADS
    bv_mutex_lock(&mutex);
#endif

    format_line(ptr, level, fmt, vl, part, &print_prefix, type);
    snprintf(line, sizeof(line), "%s%s%s%s", part[0].str, part[1].str, part[2].str, part[3].str);

#if BV_HAVE_ISATTY
    if (!is_atty)
        is_atty = isatty(2) ? 1 : -1;
#endif

    if (print_prefix && (flags & BV_LOG_SKIP_REPEATED) && !strcmp(line, prev) &&
        *line && line[strlen(line) - 1] != '\r'){
        count++;
        if (is_atty == 1)
            fprintf(stderr, "    Last message repeated %d times\r", count);
        goto end;
    }
    if (count > 0) {
        fprintf(stderr, "    Last message repeated %d times\n", count);
        count = 0;
    }
    strcpy(prev, line);
    sanitize(part[0].str);
    colored_fputs(type[0], 0, part[0].str);
    sanitize(part[1].str);
    colored_fputs(type[1], 0, part[1].str);
    sanitize(part[2].str);
    colored_fputs(bv_clip(level >> 3, 0, 6), tint >> 8, part[2].str);
    sanitize(part[3].str);
    colored_fputs(bv_clip(level >> 3, 0, 6), tint >> 8, part[3].str);
end:
    bv_bprint_finalize(part+3, NULL);
#if BV_HAVE_PTHREADS
    bv_mutex_unlock(&mutex);
#endif
}

static void (*bv_log_callback)(void*, int, const char*, va_list) =
    bv_log_default_callback;

void bv_log(void* avcl, int level, const char *fmt, ...)
{
    BVClass* avc = avcl ? *(BVClass **) avcl : NULL;
    va_list vl;
    va_start(vl, fmt);
    if (avc && avc->version >= (50 << 16 | 15 << 8 | 2) &&
        avc->log_level_offset_offset && level >= BV_LOG_FATAL)
        level += *(int *) (((uint8_t *) avcl) + avc->log_level_offset_offset);
    bv_vlog(avcl, level, fmt, vl);
    va_end(vl);
}

void bv_vlog(void* avcl, int level, const char *fmt, va_list vl)
{
    void (*log_callback)(void*, int, const char*, va_list) = bv_log_callback;
    if (log_callback)
        log_callback(avcl, level, fmt, vl);
}

int bv_log_get_level(void)
{
    return bv_log_level;
}

void bv_log_set_level(int level)
{
    bv_log_level = level;
}

void bv_log_set_flags(int arg)
{
    flags = arg;
}

int bv_log_get_flags(void)
{
    return flags;
}

void bv_log_set_callback(void (*callback)(void*, int, const char*, va_list))
{
    bv_log_callback = callback;
}

static void missing_feature_sample(int sample, void *avc, const char *msg,
                                   va_list argument_list)
{
    bv_vlog(avc, BV_LOG_WARNING, msg, argument_list);
    bv_log(avc, BV_LOG_WARNING, " is not implemented. Update your BVBase "
           "version to the newest one from Git. If the problem still "
           "occurs, it means that your file has a feature which has not "
           "been implemented.\n");
    if (sample)
        bv_log(avc, BV_LOG_WARNING, "If you want to help, upload a sample "
               "of this file to ftp://upload.ffmpeg.org/incoming/ "
               "and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)\n");
}

void bvpriv_request_sample(void *avc, const char *msg, ...)
{
    va_list argument_list;

    va_start(argument_list, msg);
    missing_feature_sample(1, avc, msg, argument_list);
    va_end(argument_list);
}

void bvpriv_report_missing_feature(void *avc, const char *msg, ...)
{
    va_list argument_list;

    va_start(argument_list, msg);
    missing_feature_sample(0, avc, msg, argument_list);
    va_end(argument_list);
}

#ifdef TEST
// LCOV_EXCL_START
#include <string.h>

int main(int argc, char **argv)
{
    int i;
    bv_log_set_level(BV_LOG_DEBUG);
    for (use_color=0; use_color<=256; use_color = 255*use_color+1) {
        bv_log(NULL, BV_LOG_FATAL, "use_color: %d\n", use_color);
        for (i = BV_LOG_DEBUG; i>=BV_LOG_QUIET; i-=8) {
            bv_log(NULL, i, " %d", i);
            bv_log(NULL, BV_LOG_INFO, "e ");
            bv_log(NULL, i + 256*123, "C%d", i);
            bv_log(NULL, BV_LOG_INFO, "e");
        }
        bv_log(NULL, BV_LOG_PANIC, "\n");
    }
    return 0;
}
// LCOV_EXCL_STOP
#endif
