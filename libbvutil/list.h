/*************************************************************************
    > File Name: list.h
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月15日 星期三 15时38分02秒
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

#ifndef BVUTIL_LIST_H
#define BVUTIL_LIST_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _BVList BVList;

typedef int (*BVListCompareFunc) (void *, void *);

typedef int (*BVListFreeFunc)(void *);

struct _BVList {
    BVList *prev;
    BVList *next;
};

BVList *bv_list_alloc(void);

int bv_list_init(BVList **list);

int bv_list_empty(BVList *list);

BVList *bv_list_append(BVList *list, void *data);

BVList *bv_list_insert_before(BVList *list, BVList *sibling, void *data);

BVList *bv_list_insert_after(BVList *list, BVList *sibling, void *data);

BVList *bv_list_find(BVList *list, void *data, BVListCompareFunc func);

void *bv_list_get_data(BVList *list, BVList *node);

BVList *bv_list_push(BVList *list, void *data);

void *bv_list_pull(BVList *list);

int bv_list_delete(BVList *list, BVList *node);

int bv_list_free(BVList *list, BVList *node, BVListFreeFunc func);

int bv_list_size(BVList *list);

int bv_list_destroy(BVList **list, BVListFreeFunc func);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: BVUTIL_LIST_H */
