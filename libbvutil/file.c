/*
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

#include "config.h"
#include "file.h"
#include "internal.h"
#include "log.h"
#include "mem.h"
#include <fcntl.h>
#include <sys/stat.h>
#if BV_HAVE_UNISTD_H
#include <unistd.h>
#endif
#if BV_HAVE_IO_H
#include <io.h>
#endif
#if BV_HAVE_MMAP
#include <sys/mman.h>
#elif BV_HAVE_MAPVIEWOFFILE
#include <windows.h>
#endif

typedef struct {
    const BVClass *class;
    int   log_offset;
    void *log_ctx;
} FileLogContext;

static const BVClass file_log_ctx_class = {
    "FILE", bv_default_item_name, NULL, LIBBVUTIL_VERSION_INT,
    offsetof(FileLogContext, log_offset), offsetof(FileLogContext, log_ctx)
};

int bv_file_map(const char *filename, uint8_t **bufptr, size_t *size,
                int log_offset, void *log_ctx)
{
    FileLogContext file_log_ctx = { &file_log_ctx_class, log_offset, log_ctx };
    int err, fd = bvpriv_open(filename, O_RDONLY);
    struct stat st;
    bv_unused void *ptr;
    off_t off_size;
    char errbuf[128];
    *bufptr = NULL;

    if (fd < 0) {
        err = BVERROR(errno);
        bv_strerror(err, errbuf, sizeof(errbuf));
        bv_log(&file_log_ctx, BV_LOG_ERROR, "Cannot read file '%s': %s\n", filename, errbuf);
        return err;
    }

    if (fstat(fd, &st) < 0) {
        err = BVERROR(errno);
        bv_strerror(err, errbuf, sizeof(errbuf));
        bv_log(&file_log_ctx, BV_LOG_ERROR, "Error occurred in fstat(): %s\n", errbuf);
        close(fd);
        return err;
    }

    off_size = st.st_size;
    if (off_size > SIZE_MAX) {
        bv_log(&file_log_ctx, BV_LOG_ERROR,
               "File size for file '%s' is too big\n", filename);
        close(fd);
        return BVERROR(EINVAL);
    }
    *size = off_size;

#if BV_HAVE_MMAP
    ptr = mmap(NULL, *size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        err = BVERROR(errno);
        bv_strerror(err, errbuf, sizeof(errbuf));
        bv_log(&file_log_ctx, BV_LOG_ERROR, "Error occurred in mmap(): %s\n", errbuf);
        close(fd);
        return err;
    }
    *bufptr = ptr;
#elif BV_HAVE_MAPVIEWOFFILE
    {
        HANDLE mh, fh = (HANDLE)_get_osfhandle(fd);

        mh = CreateFileMapping(fh, NULL, PAGE_READONLY, 0, 0, NULL);
        if (!mh) {
            bv_log(&file_log_ctx, BV_LOG_ERROR, "Error occurred in CreateFileMapping()\n");
            close(fd);
            return -1;
        }

        ptr = MapViewOfFile(mh, FILE_MAP_READ, 0, 0, *size);
        CloseHandle(mh);
        if (!ptr) {
            bv_log(&file_log_ctx, BV_LOG_ERROR, "Error occurred in MapViewOfFile()\n");
            close(fd);
            return -1;
        }

        *bufptr = ptr;
    }
#else
    *bufptr = bv_malloc(*size);
    if (!*bufptr) {
        bv_log(&file_log_ctx, BV_LOG_ERROR, "Memory allocation error occurred\n");
        close(fd);
        return BVERROR(ENOMEM);
    }
    read(fd, *bufptr, *size);
#endif

    close(fd);
    return 0;
}

void bv_file_unmap(uint8_t *bufptr, size_t size)
{
#if BV_HAVE_MMAP
    munmap(bufptr, size);
#elif BV_HAVE_MAPVIEWOFFILE
    UnmapViewOfFile(bufptr);
#else
    bv_free(bufptr);
#endif
}

int bv_tempfile(const char *prefix, char **filename, int log_offset, void *log_ctx)
{
    FileLogContext file_log_ctx = { &file_log_ctx_class, log_offset, log_ctx };
    int fd = -1;
#if !BV_HAVE_MKSTEMP
    void *ptr= tempnam(NULL, prefix);
    if(!ptr)
        ptr= tempnam(".", prefix);
    *filename = bv_strdup(ptr);
#undef free
    free(ptr);
#else
    size_t len = strlen(prefix) + 12; /* room for "/tmp/" and "XXXXXX\0" */
    *filename  = bv_malloc(len);
#endif
    /* -----common section-----*/
    if (!*filename) {
        bv_log(&file_log_ctx, BV_LOG_ERROR, "bb_tempfile: Cannot allocate file name\n");
        return BVERROR(ENOMEM);
    }
#if !BV_HAVE_MKSTEMP
#   ifndef O_BINARY
#       define O_BINARY 0
#   endif
#   ifndef O_EXCL
#       define O_EXCL 0
#   endif
    fd = open(*filename, O_RDWR | O_BINARY | O_CREAT | O_EXCL, 0600);
#else
    snprintf(*filename, len, "/tmp/%sXXXXXX", prefix);
    fd = mkstemp(*filename);
#ifdef _WIN32
    if (fd < 0) {
        snprintf(*filename, len, "./%sXXXXXX", prefix);
        fd = mkstemp(*filename);
    }
#endif
#endif
    /* -----common section-----*/
    if (fd < 0) {
        int err = BVERROR(errno);
        bv_log(&file_log_ctx, BV_LOG_ERROR, "bb_tempfile: Cannot open temporary file %s\n", *filename);
        bv_freep(filename);
        return err;
    }
    return fd; /* success */
}

#ifdef TEST

#undef printf

int main(void)
{
    uint8_t *buf;
    size_t size;
    if (bv_file_map("file.c", &buf, &size, 0, NULL) < 0)
        return 1;

    buf[0] = 's';
    printf("%s", buf);
    bv_file_unmap(buf, size);
    return 0;
}
#endif

