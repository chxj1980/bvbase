/*
 * Copyright (c) 2000, 2001, 2002 Fabrice Bellard
 * Copyright (c) 2007 Mans Rullgard
 *
 * This file is part of BVbase.
 *
 * BVbase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVbase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVbase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "common.h"
#include "mem.h"
#include "avassert.h"
#include "bvstring.h"
#include "bprint.h"

int bv_strstart(const char *str, const char *pfx, const char **ptr)
{
    while (*pfx && *pfx == *str) {
        pfx++;
        str++;
    }
    if (!*pfx && ptr)
        *ptr = str;
    return !*pfx;
}

int bv_stristart(const char *str, const char *pfx, const char **ptr)
{
    while (*pfx && bv_toupper((unsigned)*pfx) == bv_toupper((unsigned)*str)) {
        pfx++;
        str++;
    }
    if (!*pfx && ptr)
        *ptr = str;
    return !*pfx;
}

char *bv_stristr(const char *s1, const char *s2)
{
    if (!*s2)
        return (char*)(intptr_t)s1;

    do
        if (bv_stristart(s1, s2, NULL))
            return (char*)(intptr_t)s1;
    while (*s1++);

    return NULL;
}

char *bv_strnstr(const char *haystack, const char *needle, size_t hay_length)
{
    size_t needle_len = strlen(needle);
    if (!needle_len)
        return (char*)haystack;
    while (hay_length >= needle_len) {
        hay_length--;
        if (!memcmp(haystack, needle, needle_len))
            return (char*)haystack;
        haystack++;
    }
    return NULL;
}

size_t bv_strlcpy(char *dst, const char *src, size_t size)
{
    size_t len = 0;
    while (++len < size && *src)
        *dst++ = *src++;
    if (len <= size)
        *dst = 0;
    return len + strlen(src) - 1;
}

size_t bv_strlcat(char *dst, const char *src, size_t size)
{
    size_t len = strlen(dst);
    if (size <= len + 1)
        return len + strlen(src);
    return len + bv_strlcpy(dst + len, src, size - len);
}

size_t bv_strlcatf(char *dst, size_t size, const char *fmt, ...)
{
    int len = strlen(dst);
    va_list vl;

    va_start(vl, fmt);
    len += vsnprintf(dst + len, size > len ? size - len : 0, fmt, vl);
    va_end(vl);

    return len;
}

char *bv_asprintf(const char *fmt, ...)
{
    char *p = NULL;
    va_list va;
    int len;

    va_start(va, fmt);
    len = vsnprintf(NULL, 0, fmt, va);
    va_end(va);
    if (len < 0)
        goto end;

    p = bv_malloc(len + 1);
    if (!p)
        goto end;

    va_start(va, fmt);
    len = vsnprintf(p, len + 1, fmt, va);
    va_end(va);
    if (len < 0)
        bv_freep(&p);

end:
    return p;
}

char *bv_d2str(double d)
{
    char *str = bv_malloc(16);
    if (str)
        snprintf(str, 16, "%f", d);
    return str;
}

#define WHITESPACES " \n\t"

char *bv_get_token(const char **buf, const char *term)
{
    char *out     = bv_malloc(strlen(*buf) + 1);
    char *ret     = out, *end = out;
    const char *p = *buf;
    if (!out)
        return NULL;
    p += strspn(p, WHITESPACES);

    while (*p && !strspn(p, term)) {
        char c = *p++;
        if (c == '\\' && *p) {
            *out++ = *p++;
            end    = out;
        } else if (c == '\'') {
            while (*p && *p != '\'')
                *out++ = *p++;
            if (*p) {
                p++;
                end = out;
            }
        } else {
            *out++ = c;
        }
    }

    do
        *out-- = 0;
    while (out >= end && strspn(out, WHITESPACES));

    *buf = p;

    return ret;
}

char *bv_strtok(char *s, const char *delim, char **saveptr)
{
    char *tok;

    if (!s && !(s = *saveptr))
        return NULL;

    /* skip leading delimiters */
    s += strspn(s, delim);

    /* s now points to the first non delimiter char, or to the end of the string */
    if (!*s) {
        *saveptr = NULL;
        return NULL;
    }
    tok = s++;

    /* skip non delimiters */
    s += strcspn(s, delim);
    if (*s) {
        *s = 0;
        *saveptr = s+1;
    } else {
        *saveptr = NULL;
    }

    return tok;
}

int bv_strcasecmp(const char *a, const char *b)
{
    uint8_t c1, c2;
    do {
        c1 = bv_tolower(*a++);
        c2 = bv_tolower(*b++);
    } while (c1 && c1 == c2);
    return c1 - c2;
}

int bv_strncasecmp(const char *a, const char *b, size_t n)
{
    const char *end = a + n;
    uint8_t c1, c2;
    do {
        c1 = bv_tolower(*a++);
        c2 = bv_tolower(*b++);
    } while (a < end && c1 && c1 == c2);
    return c1 - c2;
}

const char *bv_basename(const char *path)
{
    char *p = strrchr(path, '/');

#if HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');

    p = FFMAX3(p, q, d);
#endif

    if (!p)
        return path;

    return p + 1;
}

const char *bv_dirname(char *path)
{
    char *p = strrchr(path, '/');

#if HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');

    d = d ? d + 1 : d;

    p = FFMAX3(p, q, d);
#endif

    if (!p)
        return ".";

    *p = '\0';

    return path;
}

int bv_escape(char **dst, const char *src, const char *special_chars,
              enum BVEscapeMode mode, int flags)
{
    BVBPrint dstbuf;

    bv_bprint_init(&dstbuf, 1, BV_BPRINT_SIZE_UNLIMITED);
    bv_bprint_escape(&dstbuf, src, special_chars, mode, flags);

    if (!bv_bprint_is_complete(&dstbuf)) {
        bv_bprint_finalize(&dstbuf, NULL);
        return BVERROR(ENOMEM);
    } else {
        bv_bprint_finalize(&dstbuf, dst);
        return dstbuf.len;
    }
}

int bv_isdigit(int c)
{
    return c >= '0' && c <= '9';
}

int bv_isgraph(int c)
{
    return c > 32 && c < 127;
}

int bv_isspace(int c)
{
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
           c == '\v';
}

int bv_isxdigit(int c)
{
    c = bv_tolower(c);
    return bv_isdigit(c) || (c >= 'a' && c <= 'f');
}

int bv_match_name(const char *name, const char *names)
{
    const char *p;
    int len, namelen;

    if (!name || !names)
        return 0;

    namelen = strlen(name);
    while ((p = strchr(names, ','))) {
        len = FFMAX(p - names, namelen);
        if (!bv_strncasecmp(name, names, len))
            return 1;
        names = p + 1;
    }
    return !bv_strcasecmp(name, names);
}

int bv_utf8_decode(int32_t *codep, const uint8_t **bufp, const uint8_t *buf_end,
                   unsigned int flags)
{
    const uint8_t *p = *bufp;
    uint32_t top;
    uint64_t code;
    int ret = 0, tail_len;
    uint32_t overlong_encoding_mins[6] = {
        0x00000000, 0x00000080, 0x00000800, 0x00010000, 0x00200000, 0x04000000,
    };

    if (p >= buf_end)
        return 0;

    code = *p++;

    /* first sequence byte starts with 10, or is 1111-1110 or 1111-1111,
       which is not admitted */
    if ((code & 0xc0) == 0x80 || code >= 0xFE) {
        ret = BVERROR(EILSEQ);
        goto end;
    }
    top = (code & 128) >> 1;

    tail_len = 0;
    while (code & top) {
        int tmp;
        tail_len++;
        if (p >= buf_end) {
            (*bufp) ++;
            return BVERROR(EILSEQ); /* incomplete sequence */
        }

        /* we assume the byte to be in the form 10xx-xxxx */
        tmp = *p++ - 128;   /* strip leading 1 */
        if (tmp>>6) {
            (*bufp) ++;
            return BVERROR(EILSEQ);
        }
        code = (code<<6) + tmp;
        top <<= 5;
    }
    code &= (top << 1) - 1;

    /* check for overlong encodings */
    bv_assert0(tail_len <= 5);
    if (code < overlong_encoding_mins[tail_len]) {
        ret = BVERROR(EILSEQ);
        goto end;
    }

    if (code >= 1<<31) {
        ret = BVERROR(EILSEQ);  /* out-of-range value */
        goto end;
    }

    *codep = code;

    if (code > 0x10FFFF &&
        !(flags & BV_UTF8_FLAG_ACCEPT_INVALID_BIG_CODES))
        ret = BVERROR(EILSEQ);
    if (code < 0x20 && code != 0x9 && code != 0xA && code != 0xD &&
        flags & BV_UTF8_FLAG_EXCLUDE_XML_INVALID_CONTROL_CODES)
        ret = BVERROR(EILSEQ);
    if (code >= 0xD800 && code <= 0xDFFF &&
        !(flags & BV_UTF8_FLAG_ACCEPT_SURROGATES))
        ret = BVERROR(EILSEQ);
    if ((code == 0xFFFE || code == 0xFFFF) &&
        !(flags & BV_UTF8_FLAG_ACCEPT_NON_CHARACTERS))
        ret = BVERROR(EILSEQ);

end:
    *bufp = p;
    return ret;
}

int bv_match_list(const char *name, const char *list, char separator)
{
    const char *p, *q;

    for (p = name; p && *p; ) {
        for (q = list; q && *q; ) {
            int k;
            for (k = 0; p[k] == q[k] || (p[k]*q[k] == 0 && p[k]+q[k] == separator); k++)
                if (k && (!p[k] || p[k] == separator))
                    return 1;
            q = strchr(q, separator);
            q += !!q;
        }
        p = strchr(p, separator);
        p += !!p;
    }

    return 0;
}

#ifdef TEST

int main(void)
{
    int i;
    static const char * const strings[] = {
        "''",
        "",
        ":",
        "\\",
        "'",
        "    ''    :",
        "    ''  ''  :",
        "foo   '' :",
        "'foo'",
        "foo     ",
        "  '  foo  '  ",
        "foo\\",
        "foo':  blah:blah",
        "foo\\:  blah:blah",
        "foo\'",
        "'foo :  '  :blahblah",
        "\\ :blah",
        "     foo",
        "      foo       ",
        "      foo     \\ ",
        "foo ':blah",
        " foo   bar    :   blahblah",
        "\\f\\o\\o",
        "'foo : \\ \\  '   : blahblah",
        "'\\fo\\o:': blahblah",
        "\\'fo\\o\\:':  foo  '  :blahblah"
    };

    printf("Testing bv_get_token()\n");
    for (i = 0; i < FF_ARRAY_ELEMS(strings); i++) {
        const char *p = strings[i];
        char *q;
        printf("|%s|", p);
        q = bv_get_token(&p, ":");
        printf(" -> |%s|", q);
        printf(" + |%s|\n", p);
        bv_free(q);
    }

    return 0;
}

#endif /* TEST */
