/*************************************************************************
    > File Name: list.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月15日 星期三 15时42分20秒
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

#line 25 "list.c"

#include "list.h"
#include "bvutil.h"

typedef struct _BVListBuncket {
    BVList list;
    void *data;
} BVListBuncket;

#define list_to_buncket(LIST) (BV_CONTAINER_OF(LIST, BVListBuncket,list))

BVList *bv_list_alloc(void)
{
    BVListBuncket *list_buncket  = bv_mallocz(sizeof(BVListBuncket));
    if (!list_buncket) {
        bv_log(NULL, BV_LOG_ERROR, "No Mem alloc list node error\n");
        return NULL;
    }
    return (BVList *) &list_buncket->list;
}

int bv_list_init(BVList **list)
{
    BVList *node = NULL;
    if (!list)
        return BVERROR(EINVAL);
    node = *list;
    if (!node && !(node = bv_list_alloc())) {
        return BVERROR(ENOMEM);
    }
    node->prev = node->next = node;
    *list = node;
    return 0;
}

int bv_list_empty(BVList *list)
{
    return list->next == list;
}

BVList *bv_list_insert_before(BVList *list, BVList *sibling, void *data)
{
    BVList *node = NULL;
    if (!list || !data)
        return NULL;

    if (!sibling)
        sibling = list;

    node = bv_list_alloc();
    if (!node)
        return NULL;
    list_to_buncket(node)->data = data;
    sibling->prev->next = node;
    node->prev = sibling->prev;
    node->next = sibling;
    sibling->prev = node;
    return node;
}

BVList *bv_list_insert_after(BVList *list, BVList *sibling, void *data)
{
    if (!sibling)
        sibling = list;
    return bv_list_insert_before(list, sibling->next, data);
}

BVList *bv_list_append(BVList *list, void *data)
{
    return bv_list_insert_before(list, list, data);
}

BVList *bv_list_find(BVList *list, void *data, BVListCompareFunc func)
{
    BVList *node = NULL;
    if (!list || !data || !func)
        return NULL;
    if (bv_list_empty(list)) {
        bv_log(NULL, BV_LOG_ERROR, "list is empty\n");
        return NULL;
    }
    node = list->next;
    while (node != list) {
        if (func(data, ((BVListBuncket *)node)->data) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void *bv_list_get_data(BVList *list, BVList *node)
{
    if (!list || !node)
        return NULL;
    return list_to_buncket(node)->data;
}

BVList *bv_list_push(BVList *list, void *data)
{
    if (!list || !data)
        return NULL;
    return bv_list_insert_after(list, list, data);
}

void *bv_list_pull(BVList *list)
{
    BVList *node = NULL;
    BVListBuncket *buncket = NULL;
    void *data = NULL;
    if (!list || list->prev == list)
        return NULL;
    node = list->prev;
    buncket = list_to_buncket(node);
    data = buncket->data;
    bv_list_delete(list, node);
    bv_freep(&buncket);
    return data;
}

int bv_list_delete(BVList *list, BVList *node)
{
    if (!list || !node)
        return BVERROR(EINVAL);

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node->next = NULL;
    return 0;
}

int bv_list_free(BVList *list, BVList *node, BVListFreeFunc func)
{
    BVListBuncket *buncket = NULL;
    if (!list || !node)
        return BVERROR(EINVAL);
    if (node->prev || node->next)
        bv_list_delete(list, node);
    buncket = list_to_buncket(node);
    if (func) {
        func(buncket->data);
    }
    bv_freep(&buncket);
    return 0;
}

int bv_list_size(BVList *list)
{
    BVList *node = NULL;
    int size = 0;
    if (bv_list_empty(list))
        return 0;
    node = list->next;
    while (node != list) {
        node = node->next; 
        size ++;
    }
    return size;
}

int bv_list_destroy(BVList **list, BVListFreeFunc func)
{
    BVList *llist = NULL;
    BVList *node = NULL;
    BVList *tmp = NULL;
    BVListBuncket *buncket = NULL;
    if (!list || !*list)
        return BVERROR(EINVAL);
    llist = *list;
    node = llist->next;
    while (node != llist) {
        tmp = node->next;
        bv_list_free(llist, node, func);
        node = tmp;
    }
    buncket = list_to_buncket(llist);
    bv_freep(&buncket);
    *list = NULL;
    return 0;
}

