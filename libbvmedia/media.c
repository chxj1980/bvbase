/*************************************************************************
	> File Name: media.c
	> Author: albertfang
	> Mail: fang.qi@besovideo.com 
	> Created Time: 2014年12月30日 星期二 19时18分36秒
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
 * Copyright (C) albert@BesoVideo, 2014
 */

#include "bvmedia.h"
#include <libbvutil/bvstring.h>

BVInputMedia *bv_input_media_find(const char *short_name)
{
    BVInputMedia *fmt = NULL;
    while ((fmt = bv_input_media_next(fmt)))
        if (bv_match_name(short_name, fmt->name))
            return fmt;
    return NULL;
}

static int bv_match_ext(const char *filename, const char *extensions)
{
    const char *ext, *p;
    char ext1[32], *q;

    if (!filename)
        return 0;

    ext = strrchr(filename, '.');
    if (ext) {
        ext++;
        p = extensions;
        for (;;) {
            q = ext1;
            while (*p != '\0' && *p != ','  && q - ext1 < sizeof(ext1) - 1)
                *q++ = *p++;
            *q = '\0';
            if (!bv_strcasecmp(ext1, ext))
                return 1;
            if (*p == '\0')
                break;
            p++;
        }
    }
    return 0;
}

BVOutputMedia *bv_output_media_guess(const char *short_name, const char *filename,
                                const char *mime_type)
{
    BVOutputMedia *fmt = NULL, *fmt_found;
    int score_max, score;

    /* Find the proper file type. */
    fmt_found = NULL;
    score_max = 0;
    while ((fmt = bv_output_media_next(fmt))) {
        score = 0;
        if (fmt->name && short_name && bv_match_name(short_name, fmt->name))
            score += 100;
        if (fmt->mime_type && mime_type && !strcmp(fmt->mime_type, mime_type))
            score += 10;
        if (filename && fmt->extensions &&
            bv_match_ext(filename, fmt->extensions)) {
            score += 5;
        }
        if (score > score_max) {
            score_max = score;
            fmt_found = fmt;
        }
    }
    return fmt_found;
}

static int input_media_open_internal(BVMediaContext **fmt, const char *url, BVInputMedia *media, BVDictionary **options)
{
    return 0;
}

int bv_input_media_open(BVMediaContext **fmt, const BVChannel *channel, const char *url,
        BVInputMedia *media, BVDictionary **options)
{
    return 0;
}
