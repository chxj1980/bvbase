/*************************************************************************
    > File Name: utils.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2014年09月25日 星期四 14时19分40秒
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

#include <libbvutil/bvstring.h>
#include <libbvutil/atomic.h>

#include "bvserver.h"

const char FILE_NAME[] = "utils.c";

static BVServer *first_svr = NULL;
static BVServer **last_svr = &first_svr;

int bv_server_register(BVServer * svr)
{
    BVServer **p = last_svr;
    svr->next = NULL;
    while (*p || bvpriv_atomic_ptr_cas((void *volatile *) p, NULL, svr))
        p = &(*p)->next;
    last_svr = &svr->next;
    return 0;
}

BVServer *bv_server_next(BVServer * svr)
{
    if (svr)
        return svr->next;
    else
        return first_svr;
}

BVServer *bv_server_find_server(enum BVServerType server_type)
{
    BVServer *svr = NULL;
    if (first_svr == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "BVServer Not RegisterAll");
        return NULL;
    }

    while ((svr = bv_server_next(svr))) {
        if (svr->server_type == server_type) {
            return svr;
        }
    }
    return NULL;
}

BVServer *bv_server_find_server_by_name(const char *svr_name)
{
    BVServer *svr = NULL;

    if (first_svr == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "BVServer Not RegisterAll");
        return NULL;
    }

    while ((svr = bv_server_next(svr))) {
        if (strncmp(svr->name, svr_name, strlen(svr->name)) == 0) {
            return svr;
        }
    }
    return NULL;
}

static int init_server(BVServerContext *s, const char *url)
{
    return 0;
}

int bv_server_open(BVServerContext **h, BVServer *svr, const char *url, BVDictionary **options)
{
    BVDictionary *tmp = NULL;
    BVServerContext *s = *h;
    int ret = 0;
    if (!s && !(s = bv_server_context_alloc()))
        return BVERROR(ENOMEM);
    if (!s->bv_class) {
        bv_log(s, BV_LOG_ERROR, "Impossible run here %s %d\n", FILE_NAME, __LINE__);
        return BVERROR(EINVAL);
    }

    if (options)
        bv_dict_copy(&tmp, *options, 0);

    if (bv_opt_set_dict(s, &tmp) < 0)
        goto fail;

    if (svr)
         s->server = svr;
    else
        ret = init_server(s, url);
    if (svr->priv_data_size > 0) {
        s->priv_data = bv_mallocz(svr->priv_data_size);
        if (!s->priv_data)
            goto fail;
        if (svr->priv_class) {
            *(const BVClass **) s->priv_data = svr->priv_class;
            bv_opt_set_defaults(s->priv_data);
            if ((ret = bv_opt_set_dict(s->priv_data, &tmp)) < 0) {
                bv_log(s, BV_LOG_ERROR, "set dict error\n");
                goto fail;
            }
        }
    }
    if (!s->server->svr_open)
        goto fail;
    *h = s;
    if (url)
        bv_strlcpy(s->url, url, sizeof(s->url));

    bv_dict_free(&tmp);
    return s->server->svr_open(s);
fail:
    bv_dict_free(&tmp);
    bv_server_context_free(s);
    *h = NULL;
    return ret;
}

int bv_server_connect(BVServerContext * svrctx)
{

    return 0;
}

int bv_server_disconnect(BVServerContext * svrctx)
{

    return 0;
}

int bv_server_read(BVServerContext *svrctx, BVServerPacket *pkt)
{
    return 0;
}

int bv_server_write(BVServerContext *svrctx, const BVServerPacket *pkt)
{
    if (!svrctx || !pkt) {
        bv_log(NULL, BV_LOG_ERROR, "Param Error");
        return -1;
    }
    if (svrctx->server == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "server error");
        return -1;
    }
    return svrctx->server->svr_write(svrctx, pkt);
}
