/*
 * Copyright (c) 2000, 2001, 2002 Fabrice Bellard
 * Copyright (c) 2007 Mans Rullgard
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

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "common.h"
#include "mem.h"
#include "bvassert.h"
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

int bv_strnsub(const char *haystack, const char *needle, size_t hay_length)
{
    int sub = 0;
    char *p = (char *)haystack;
    for (; ; ) {
       p = bv_strnstr(p, needle, strlen(p));
       if (!p)
           break;
       sub ++;
       p += strlen(needle);
    }
    return sub;
}

char *bv_strsub(const char *haystack, const char *needle, int sub)
{
    int lsub = 0;
    char *p = (char *)haystack;
    for (; ; ) {
       p = bv_strnstr(p, needle, strlen(p));
       if (!p)
           break;
       lsub ++;
       p += strlen(needle);
       if (lsub == sub)
           return p;
    }
    return NULL;
}

size_t bv_strlcpy(char *dst, const char *src, size_t size)
{
    size_t len = 0;
    if (!src) {
        dst[0] = '\0';
        return len;
    }
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

size_t bv_sprintf(char *dst, size_t size, const char *fmt, ...)
{
    va_list va;
    int len;

    va_start(va, fmt);
    len = vsnprintf(dst, size, fmt, va);
    va_end(va);
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

char *bv_sreplace(const char *haystack, const char *term, const char *needle)
{
    char *p = NULL;
    const char *q = NULL;
    size_t len = 0;
    if (!haystack || !term || !needle)
        return NULL;
    len = strlen(haystack) - strlen(term) + strlen(needle);
    p = bv_mallocz(len + 1);
    if (!p)
        return NULL;
    q = bv_stristr(haystack, term);
    if (!q) {
        goto error;
    }

    bv_strlcpy(p, haystack, q - haystack + 1);
    bv_strlcatf(p, len + 1, "%s%s", needle, q + strlen(term));
    return p;
error:
    bv_freep(p);
    return NULL;
}

char *bv_sinsert(const char *haystack, const char *term, const char *needle)
{
    char *p = NULL;
    const char *q = NULL;
    size_t len = 0;
    if (!haystack || !term || !needle)
        return NULL;
    len = strlen(haystack) + strlen(needle);
    p = bv_mallocz(len + 1);
    if (!p)
        return NULL;
    q = bv_stristr(haystack, term);
    if (!q) {
        goto error;
    }
    q += strlen(term);
    bv_strlcpy(p, haystack, q  - haystack + 1);
    bv_strlcatf(p, len + 1, "%s%s", needle, q);
    return p;
error:
    bv_freep(p);
    return NULL;
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

#if BV_HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');

    p = BBMAX3(p, q, d);
#endif

    if (!p)
        return path;

    return p + 1;
}

const char *bv_dirname(char *path)
{
    char *p = strrchr(path, '/');

#if BV_HAVE_DOS_PATHS
    char *q = strrchr(path, '\\');
    char *d = strchr(path, ':');

    d = d ? d + 1 : d;

    p = BBMAX3(p, q, d);
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
        len = BBMAX(p - names, namelen);
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

void bv_url_split(char *proto,         int proto_size,
                  char *authorization, int authorization_size,
                  char *hostname,      int hostname_size,
                  int *port_ptr,
                  char *path,          int path_size,
                  const char *url)
{
    const char *p, *ls, *ls2, *at, *at2, *col, *brk;

    if (port_ptr)
        *port_ptr = -1;
    if (proto_size > 0)
        proto[0] = 0;
    if (authorization_size > 0)
        authorization[0] = 0;
    if (hostname_size > 0)
        hostname[0] = 0;
    if (path_size > 0)
        path[0] = 0;

    /* parse protocol */
    if ((p = strchr(url, ':'))) {
        bv_strlcpy(proto, url, BBMIN(proto_size, p + 1 - url));
        p++; /* skip ':' */
        if (*p == '/')
            p++;
        if (*p == '/')
            p++;
    } else {
        /* no protocol means plain filename */
        bv_strlcpy(path, url, path_size);
        return;
    }

    /* separate path from hostname */
    ls = strchr(p, '/');
    ls2 = strchr(p, '?');
    if (!ls)
        ls = ls2;
    else if (ls && ls2)
        ls = BBMIN(ls, ls2);
    if (ls)
        bv_strlcpy(path, ls, path_size);
    else
        ls = &p[strlen(p)];  // XXX

    /* the rest is hostname, use that to parse auth/port */
    if (ls != p) {
        /* authorization (user[:pass]@hostname) */
        at2 = p;
        while ((at = strchr(p, '@')) && at < ls) {
            bv_strlcpy(authorization, at2,
                       BBMIN(authorization_size, at + 1 - at2));
            p = at + 1; /* skip '@' */
        }

        if (*p == '[' && (brk = strchr(p, ']')) && brk < ls) {
            /* [host]:port */
            bv_strlcpy(hostname, p + 1,
                       BBMIN(hostname_size, brk - p));
            if (brk[1] == ':' && port_ptr)
                *port_ptr = atoi(brk + 2);
        } else if ((col = strchr(p, ':')) && col < ls) {
            bv_strlcpy(hostname, p,
                       BBMIN(col + 1 - p, hostname_size));
            if (port_ptr)
                *port_ptr = atoi(col + 1);
        } else
            bv_strlcpy(hostname, p,
                       BBMIN(ls + 1 - p, hostname_size));
    }

}

int bv_url_join(char *str, int size, const char *proto,
                const char *authorization, const char *hostname,
                int port, const char *fmt, ...)
{
#if 0
#if BV_CONFIG_NETWORK
    struct addrinfo hints = { 0 }, *ai;
#endif

    str[0] = '\0';
    if (proto)
        bv_strlcatf(str, size, "%s://", proto);
    if (authorization && authorization[0])
        bv_strlcatf(str, size, "%s@", authorization);
#if BV_CONFIG_NETWORK && defined(AF_INET6)
    /* Determine if hostname is a numerical IPv6 address,
     * properly escape it within [] in that case. */
    hints.ai_flags = AI_NUMERICHOST;
    if (!getaddrinfo(hostname, NULL, &hints, &ai)) {
        if (ai->ai_family == AF_INET6) {
            bv_strlcat(str, "[", size);
            bv_strlcat(str, hostname, size);
            bv_strlcat(str, "]", size);
        } else {
            bv_strlcat(str, hostname, size);
        }
        freeaddrinfo(ai);
    } else
#endif
        /* Not an IPv6 address, just output the plain string. */
        bv_strlcat(str, hostname, size);

    if (port >= 0)
        bv_strlcatf(str, size, ":%d", port);
    if (fmt) {
        va_list vl;
        int len = strlen(str);

        va_start(vl, fmt);
        vsnprintf(str + len, size > len ? size - len : 0, fmt, vl);
        va_end(vl);
    }
    return strlen(str);
#else
    return 0;
#endif
}

void bv_make_absolute_url(char *buf, int size, const char *base,
                          const char *rel)
{
    char *sep, *path_query;
    /* Absolute path, relative to the current server */
    if (base && strstr(base, "://") && rel[0] == '/') {
        if (base != buf)
            bv_strlcpy(buf, base, size);
        sep = strstr(buf, "://");
        if (sep) {
            /* Take scheme from base url */
            if (rel[1] == '/') {
                sep[1] = '\0';
            } else {
                /* Take scheme and host from base url */
                sep += 3;
                sep = strchr(sep, '/');
                if (sep)
                    *sep = '\0';
            }
        }
        bv_strlcat(buf, rel, size);
        return;
    }
    /* If rel actually is an absolute url, just copy it */
    if (!base || strstr(rel, "://") || rel[0] == '/') {
        bv_strlcpy(buf, rel, size);
        return;
    }
    if (base != buf)
        bv_strlcpy(buf, base, size);

    /* Strip off any query string from base */
    path_query = strchr(buf, '?');
    if (path_query)
        *path_query = '\0';

    /* Is relative path just a new query part? */
    if (rel[0] == '?') {
        bv_strlcat(buf, rel, size);
        return;
    }

    /* Remove the file name from the base url */
    sep = strrchr(buf, '/');
    if (sep)
        sep[1] = '\0';
    else
        buf[0] = '\0';
    while (bv_strstart(rel, "../", NULL) && sep) {
        /* Remove the path delimiter at the end */
        sep[0] = '\0';
        sep = strrchr(buf, '/');
        /* If the next directory name to pop off is "..", break here */
        if (!strcmp(sep ? &sep[1] : buf, "..")) {
            /* Readd the slash we just removed */
            bv_strlcat(buf, "/", size);
            break;
        }
        /* Cut off the directory name */
        if (sep)
            sep[1] = '\0';
        else
            buf[0] = '\0';
        rel += 3;
    }
    bv_strlcat(buf, rel, size);
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

    const char *str = "rtsp://192.168.6.149/onvif/device_services";
    char *t = NULL;
    printf("Testing bv_get_token()\n");
    for (i = 0; i < BV_ARRAY_ELEMS(strings); i++) {
        const char *p = strings[i];
        char *q;
        printf("|%s|", p);
        q = bv_get_token(&p, ":");
        printf(" -> |%s|", q);
        printf(" + |%s|\n", p);
        bv_free(q);
    }
    printf("source %s\n", str);
    t = bv_sreplace(str, "rtsp", "http"); 
    if (t) {
        printf("%s\n", t);
        bv_free(t);
    }

    t = bv_sreplace(str, "onvif", "hisi"); 
    if (t) {
        printf("%s\n", t);
        bv_free(t);
    }
    t = bv_sreplace(str, "services", "daemon"); 
    if (t) {
        printf("%s\n", t);
        bv_free(t);
    }

    t = bv_sinsert(str, "rtsp", "/http"); 
    if (t) {
        printf("%s\n", t);
        bv_free(t);
    }

    t = bv_sinsert(str, "onvif", "/hisi"); 
    if (t) {
        printf("%s\n", t);
        bv_free(t);
    }
    t = bv_sinsert(str, "services", "/daemon"); 
    if (t) {
        printf("%s\n", t);
        bv_free(t);
    }

    return 0;
}

#endif /* TEST */
