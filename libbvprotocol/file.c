/*************************************************************************
    > File Name: file.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月04日 星期日 10时29分44秒
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

#include <unistd.h>
#include <libbvutil/bvstring.h>

#include "config.h"

#include <fcntl.h>
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <stdlib.h>
#include "os_support.h"

#include "bvurl.h"
#include "bvio.h"

typedef struct FileContext {
    const BVClass *class;
    int fd;
    int trunc;
    int blocksize;
} FileContext;

static const BVOption file_options[] = {
    { "truncate", "truncate existing files on write", offsetof(FileContext, trunc), BV_OPT_TYPE_INT, { .i64 = 1 }, 0, 1, BV_OPT_FLAG_ENCODING_PARAM },
    { "blocksize", "set I/O operation maximum block size", offsetof(FileContext, blocksize), BV_OPT_TYPE_INT, { .i64 = INT_MAX }, 1, INT_MAX, BV_OPT_FLAG_ENCODING_PARAM },
    { NULL }
};

static const BVClass file_class = {
    .class_name = "file",
    .item_name  = bv_default_item_name,
    .option     = file_options,
    .version    = LIBBVUTIL_VERSION_INT,
};

static int file_read(BVURLContext *h, void *buf, size_t size)
{
    FileContext *c = h->priv_data;
    int r;
    size = FFMIN(size, c->blocksize);
    r = read(c->fd, buf, size);
    return (-1 == r)?BVERROR(errno):r;
}

static int file_write(BVURLContext *h, const void *buf, size_t size)
{
    FileContext *c = h->priv_data;
    int r;
    size = FFMIN(size, c->blocksize);
    r = write(c->fd, buf, size);
    return (-1 == r)?BVERROR(errno):r;
}

static int file_get_handle(BVURLContext *h)
{
    FileContext *c = h->priv_data;
    return c->fd;
}

static int file_check(BVURLContext *h, int mask)
{
    int ret = 0;
    const char *filename = h->filename;
    bv_strstart(filename, "file:", &filename);

    {
#if HAVE_ACCESS && defined(R_OK)
    if (access(filename, F_OK) < 0)
        return BVERROR(errno);
    if (mask&BV_IO_FLAG_READ)
        if (access(filename, R_OK) >= 0)
            ret |= BV_IO_FLAG_READ;
    if (mask&BV_IO_FLAG_WRITE)
        if (access(filename, W_OK) >= 0)
            ret |= BV_IO_FLAG_WRITE;
#else
    struct stat st;
    ret = stat(filename, &st);
    if (ret < 0)
        return BVERROR(errno);

    ret |= st.st_mode&S_IRUSR ? mask&BV_IO_FLAG_READ  : 0;
    ret |= st.st_mode&S_IWUSR ? mask&BV_IO_FLAG_WRITE : 0;
#endif
    }
    return ret;
}

static int file_open(BVURLContext *h, const char *filename, int flags)
{
    FileContext *c = h->priv_data;
    int access;
    int fd;
    struct stat st;

    bv_strstart(filename, "file:", &filename);

    if (flags & BV_IO_FLAG_WRITE && flags & BV_IO_FLAG_READ) {
        access = O_CREAT | O_RDWR;
        if (c->trunc)
            access |= O_TRUNC;
    } else if (flags & BV_IO_FLAG_WRITE) {
        access = O_CREAT | O_WRONLY;
        if (c->trunc)
            access |= O_TRUNC;
    } else {
        access = O_RDONLY;
    }
#ifdef O_BINARY
    access |= O_BINARY;
#endif
    fd = bvpriv_open(filename, access, 0666);
    if (fd == -1)
        return BVERROR(errno);
    c->fd = fd;

//    h->is_streamed = !fstat(fd, &st) && S_ISFIFO(st.st_mode);

    return 0;
}

/* XXX: use llseek */
static int64_t file_seek(BVURLContext *h, int64_t pos, int whence)
{
    FileContext *c = h->priv_data;
    int64_t ret;
#if 0
    if (whence == BVSEEK_SIZE) {
        struct stat st;
        ret = fstat(c->fd, &st);
        return ret < 0 ? BVERROR(errno) : (S_ISFIFO(st.st_mode) ? 0 : st.st_size);
    }
#endif
    ret = lseek(c->fd, pos, whence);

    return ret < 0 ? BVERROR(errno) : ret;
}

static int file_close(BVURLContext *h)
{
    FileContext *c = h->priv_data;
    return close(c->fd);
}

BVURLProtocol bv_file_protocol = {
    .name                = "file",
    .url_open            = file_open,
    .url_read            = file_read,
    .url_write           = file_write,
    .url_seek            = file_seek,
    .url_close           = file_close,
    .url_fd              = file_get_handle,
    .url_check           = file_check,
    .priv_data_size      = sizeof(FileContext),
    .priv_class          = &file_class,
};

