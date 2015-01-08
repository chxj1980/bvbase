/*************************************************************************
    > File Name: bvurl.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年01月08日 星期四 22时11分15秒
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

#include "libbvutil/bvstring.h"
#include "libbvutil/dict.h"
#include "libbvutil/opt.h"
#include "libbvutil/time.h"
#include "os_support.h"
#if BV_CONFIG_NETWORK
#include "network.h"
#endif

#include "bvio.h"
#include "bvurl.h"

static BVURLProtocol *first_protocol = NULL;

BVURLProtocol *bv_url_protocol_next(const BVURLProtocol *prev)
{
    return prev ? prev->next : first_protocol;
}

/** @name Logging context. */
/*@{*/
static const char *urlcontext_to_name(void *ptr)
{
    BVURLContext *h = (BVURLContext *)ptr;
    if (h->prot)
        return h->prot->name;
    else
        return "NULL";
}

static void *urlcontext_child_next(void *obj, void *prev)
{
    BVURLContext *h = obj;
    if (!prev && h->priv_data && h->prot->priv_class)
        return h->priv_data;
    return NULL;
}

static const BVClass *urlcontext_child_class_next(const BVClass *prev)
{
    BVURLProtocol *p = NULL;

    /* find the protocol that corresponds to prev */
    while (prev && (p = bv_url_protocol_next(p)))
        if (p->priv_class == prev)
            break;

    /* find next protocol with priv options */
    while (p = bv_url_protocol_next(p))
        if (p->priv_class)
            return p->priv_class;
    return NULL;
}

static const BVOption options[] = { { NULL } };
const BVClass bv_url_context_class = {
    .class_name       = "BVURLContext",
    .item_name        = urlcontext_to_name,
    .option           = options,
    .version          = LIBBVUTIL_VERSION_INT,
    .child_next       = urlcontext_child_next,
    .child_class_next = urlcontext_child_class_next,
};
/*@}*/

const char *avio_enum_protocols(void **opaque, int output)
{
    BVURLProtocol *p;
    *opaque = bv_url_protocol_next(*opaque);
    if (!(p = *opaque))
        return NULL;
    if ((output && p->url_write) || (!output && p->url_read))
        return p->name;
    return avio_enum_protocols(opaque, output);
}

int bv_url_register_protocol(BVURLProtocol *protocol)
{
    BVURLProtocol **p;
    p = &first_protocol;
    while (*p)
        p = &(*p)->next;
    *p             = protocol;
    protocol->next = NULL;
    return 0;
}

static int url_alloc_for_protocol(BVURLContext **puc, BVURLProtocol *up,
                                  const char *filename, int flags,
                                  const BVIOInterruptCB *int_cb)
{
    BVURLContext *uc;
    int err;

#if BV_CONFIG_NETWORK
    if (up->flags & BV_URL_PROTOCOL_FLAG_NETWORK && !bb_network_init())
        return BVERROR(EIO);
#endif
    if ((flags & BV_IO_FLAG_READ) && !up->url_read) {
        bv_log(NULL, BV_LOG_ERROR,
               "Impossible to open the '%s' protocol for reading\n", up->name);
        return BVERROR(EIO);
    }
    if ((flags & BV_IO_FLAG_WRITE) && !up->url_write) {
        bv_log(NULL, BV_LOG_ERROR,
               "Impossible to open the '%s' protocol for writing\n", up->name);
        return BVERROR(EIO);
    }
    uc = bv_mallocz(sizeof(BVURLContext) + strlen(filename) + 1);
    if (!uc) {
        err = BVERROR(ENOMEM);
        goto fail;
    }
    uc->bv_class = &bv_url_context_class;
    uc->filename = (char *)&uc[1];
    strcpy(uc->filename, filename);
    uc->prot            = up;
    uc->flags           = flags;
    uc->is_streamed     = 0; /* default = not streamed */
    uc->max_packet_size = 0; /* default: stream file */
    if (up->priv_data_size) {
        uc->priv_data = bv_mallocz(up->priv_data_size);
        if (!uc->priv_data) {
            err = BVERROR(ENOMEM);
            goto fail;
        }
        if (up->priv_class) {
            int proto_len= strlen(up->name);
            char *start = strchr(uc->filename, ',');
            *(const BVClass **)uc->priv_data = up->priv_class;
            bv_opt_set_defaults(uc->priv_data);
            if(!strncmp(up->name, uc->filename, proto_len) && uc->filename + proto_len == start){
                int ret= 0;
                char *p= start;
                char sep= *++p;
                char *key, *val;
                p++;
                while(ret >= 0 && (key= strchr(p, sep)) && p<key && (val = strchr(key+1, sep))){
                    *val= *key= 0;
                    ret= bv_opt_set(uc->priv_data, p, key+1, 0);
                    if (ret == BVERROR_OPTION_NOT_FOUND)
                        bv_log(uc, BV_LOG_ERROR, "Key '%s' not found.\n", p);
                    *val= *key= sep;
                    p= val+1;
                }
                if(ret<0 || p!=key){
                    bv_log(uc, BV_LOG_ERROR, "Error parsing options string %s\n", start);
                    bv_freep(&uc->priv_data);
                    bv_freep(&uc);
                    err = BVERROR(EINVAL);
                    goto fail;
                }
                memmove(start, key+1, strlen(key));
            }
        }
    }
    if (int_cb)
        uc->interrupt_callback = *int_cb;

    *puc = uc;
    return 0;
fail:
    *puc = NULL;
    if (uc)
        bv_freep(&uc->priv_data);
    bv_freep(&uc);
#if BV_CONFIG_NETWORK
    if (up->flags & BV_URL_PROTOCOL_FLAG_NETWORK)
        bb_network_close();
#endif
    return err;
}

int bv_url_connect(BVURLContext *uc, BVDictionary **options)
{
    int err =
        uc->prot->url_open2 ? uc->prot->url_open2(uc,
                                                  uc->filename,
                                                  uc->flags,
                                                  options) :
        uc->prot->url_open(uc, uc->filename, uc->flags);
    if (err)
        return err;
    uc->is_connected = 1;
    /* We must be careful here as bv_url_seek() could be slow,
     * for example for http */
    if ((uc->flags & BV_IO_FLAG_WRITE) || !strcmp(uc->prot->name, "file"))
        if (!uc->is_streamed && bv_url_seek(uc, 0, SEEK_SET) < 0)
            uc->is_streamed = 1;
    return 0;
}

#define URL_SCHEME_CHARS                        \
    "abcdefghijklmnopqrstuvwxyz"                \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                \
    "0123456789+-."

static BVURLProtocol *url_find_protocol(const char *filename)
{
    BVURLProtocol *up = NULL;
    char proto_str[128], proto_nested[128], *ptr;
    size_t proto_len = strspn(filename, URL_SCHEME_CHARS);

    if (filename[proto_len] != ':' &&
        (filename[proto_len] != ',' || !strchr(filename + proto_len + 1, ':')) ||
        is_dos_path(filename))
        strcpy(proto_str, "file");
    else
        bv_strlcpy(proto_str, filename,
                   BBMIN(proto_len + 1, sizeof(proto_str)));

    if ((ptr = strchr(proto_str, ',')))
        *ptr = '\0';
    bv_strlcpy(proto_nested, proto_str, sizeof(proto_nested));
    if ((ptr = strchr(proto_nested, '+')))
        *ptr = '\0';

    while (up = bv_url_protocol_next(up)) {
        if (!strcmp(proto_str, up->name))
            break;
        if (up->flags & BV_URL_PROTOCOL_FLAG_NESTED_SCHEME &&
            !strcmp(proto_nested, up->name))
            break;
    }

    return up;
}

int bv_url_alloc(BVURLContext **puc, const char *filename, int flags,
                const BVIOInterruptCB *int_cb)
{
    BVURLProtocol *p = NULL;

    if (!first_protocol) {
        bv_log(NULL, BV_LOG_WARNING, "No URL Protocols are registered. "
                                     "Missing call to bv_register_all()?\n");
    }

    p = url_find_protocol(filename);
    if (p)
       return url_alloc_for_protocol(puc, p, filename, flags, int_cb);

    *puc = NULL;
    if (bv_strstart(filename, "https:", NULL))
        bv_log(NULL, BV_LOG_WARNING, "https protocol not found, recompile with openssl or gnutls enabled.\n");
    return BVERROR_PROTOCOL_NOT_FOUND;
}

int bv_url_open(BVURLContext **puc, const char *filename, int flags,
               const BVIOInterruptCB *int_cb, BVDictionary **options)
{
    int ret = bv_url_alloc(puc, filename, flags, int_cb);
    if (ret < 0)
        return ret;
    if (options && (*puc)->prot->priv_class &&
        (ret = bv_opt_set_dict((*puc)->priv_data, options)) < 0)
        goto fail;
    if ((ret = bv_opt_set_dict(*puc, options)) < 0)
        goto fail;
    ret = bv_url_connect(*puc, options);
    if (!ret)
        return 0;
fail:
    bv_url_close(*puc);
    *puc = NULL;
    return ret;
}

static inline int retry_transfer_wrapper(BVURLContext *h, uint8_t *buf,
                                         int size, int size_min,
                                         int (*transfer_func)(BVURLContext *h,
                                                              uint8_t *buf,
                                                              int size))
{
    int ret, len;
    int fast_retries = 5;
    int64_t wait_since = 0;

    len = 0;
    while (len < size_min) {
        if (bv_check_interrupt(&h->interrupt_callback))
            return BVERROR_EXIT;
        ret = transfer_func(h, buf + len, size - len);
        if (ret == BVERROR(EINTR))
            continue;
        if (h->flags & BV_IO_FLAG_NONBLOCK)
            return ret;
        if (ret == BVERROR(EAGAIN)) {
            ret = 0;
            if (fast_retries) {
                fast_retries--;
            } else {
                if (h->rw_timeout) {
                    if (!wait_since)
                        wait_since = bv_gettime_relative();
                    else if (bv_gettime_relative() > wait_since + h->rw_timeout)
                        return BVERROR(EIO);
                }
                bv_usleep(1000);
            }
        } else if (ret < 1)
            return (ret < 0 && ret != BVERROR_EOF) ? ret : len;
        if (ret)
            fast_retries = BBMAX(fast_retries, 2);
        len += ret;
    }
    return len;
}

int bv_url_read(BVURLContext *h, unsigned char *buf, int size)
{
    if (!(h->flags & BV_IO_FLAG_READ))
        return BVERROR(EIO);
    return retry_transfer_wrapper(h, buf, size, 1, h->prot->url_read);
}

int bv_url_read_complete(BVURLContext *h, unsigned char *buf, int size)
{
    if (!(h->flags & BV_IO_FLAG_READ))
        return BVERROR(EIO);
    return retry_transfer_wrapper(h, buf, size, size, h->prot->url_read);
}

int bv_url_write(BVURLContext *h, const unsigned char *buf, int size)
{
    if (!(h->flags & BV_IO_FLAG_WRITE))
        return BVERROR(EIO);
    /* avoid sending too big packets */
    if (h->max_packet_size && size > h->max_packet_size)
        return BVERROR(EIO);

    return retry_transfer_wrapper(h, (unsigned char *)buf, size, size, (void*)h->prot->url_write);
}

int64_t bv_url_seek(BVURLContext *h, int64_t pos, int whence)
{
    int64_t ret;

    if (!h->prot->url_seek)
        return BVERROR(ENOSYS);
    ret = h->prot->url_seek(h, pos, whence & ~BV_SEEK_FORCE);
    return ret;
}

int bv_url_closep(BVURLContext **hh)
{
    BVURLContext *h= *hh;
    int ret = 0;
    if (!h)
        return 0;     /* can happen when bv_url_open fails */

    if (h->is_connected && h->prot->url_close)
        ret = h->prot->url_close(h);
#if BV_CONFIG_NETWORK
    if (h->prot->flags & BV_URL_PROTOCOL_FLAG_NETWORK)
        bb_network_close();
#endif
    if (h->prot->priv_data_size) {
        if (h->prot->priv_class)
            bv_opt_free(h->priv_data);
        bv_freep(&h->priv_data);
    }
    bv_freep(hh);
    return ret;
}

int bv_url_close(BVURLContext *h)
{
    return bv_url_closep(&h);
}


const char *avio_find_protocol_name(const char *url)
{
    BVURLProtocol *p = url_find_protocol(url);

    return p ? p->name : NULL;
}

int avio_check(const char *url, int flags)
{
    BVURLContext *h;
    int ret = bv_url_alloc(&h, url, flags, NULL);
    if (ret < 0)
        return ret;

    if (h->prot->url_check) {
        ret = h->prot->url_check(h, flags);
    } else {
        ret = bv_url_connect(h, NULL);
        if (ret >= 0)
            ret = flags;
    }

    bv_url_close(h);
    return ret;
}

int64_t bv_url_size(BVURLContext *h)
{
    int64_t pos, size;

    size = bv_url_seek(h, 0, BV_SEEK_SIZE);
    if (size < 0) {
        pos = bv_url_seek(h, 0, SEEK_CUR);
        if ((size = bv_url_seek(h, -1, SEEK_END)) < 0)
            return size;
        size++;
        bv_url_seek(h, pos, SEEK_SET);
    }
    return size;
}

int bv_url_get_file_handle(BVURLContext *h)
{
    if (!h->prot->url_get_file_handle)
        return -1;
    return h->prot->url_get_file_handle(h);
}

int bv_url_get_multi_file_handle(BVURLContext *h, int **handles, int *numhandles)
{
    if (!h->prot->url_get_multi_file_handle) {
        if (!h->prot->url_get_file_handle)
            return BVERROR(ENOSYS);
        *handles = bv_malloc(sizeof(**handles));
        if (!*handles)
            return BVERROR(ENOMEM);
        *numhandles = 1;
        *handles[0] = h->prot->url_get_file_handle(h);
        return 0;
    }
    return h->prot->url_get_multi_file_handle(h, handles, numhandles);
}

int bv_url_shutdown(BVURLContext *h, int flags)
{
    if (!h->prot->url_shutdown)
        return BVERROR(EINVAL);
    return h->prot->url_shutdown(h, flags);
}

int bv_check_interrupt(BVIOInterruptCB *cb)
{
    int ret;
    if (cb && cb->callback && (ret = cb->callback(cb->opaque)))
        return ret;
    return 0;
}
