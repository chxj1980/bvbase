/*************************************************************************
    > File Name: exList.c
    > Author: albertfang
    > Mail: fang.qi@besovideo.com 
    > Created Time: 2015年04月15日 星期三 18时08分54秒
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

#include <libbvutil/bvutil.h>
#include <libbvutil/list.h>
#include <libbvutil/bvstring.h>

typedef struct _Test {
    char name[20];
    int age;
    char *t;
} Test;

static int free_node(void *a)
{
    bv_free(((Test *)a)->t);
    ((Test *)a)->t = NULL;
    return 0;
}

int find_node(void *a, void *b)
{
    if (strcmp(((Test *)a)->name, ((Test *)b)->name) == 0) 
        return 0;
    return -1;
}

int find_node_all(void *a, void *b)
{
    if (((Test *)b)->age > 30)
        return 0;
    return -1;
}

#define NUM 20

int main(int argc, const char *argv[])
{
    int i = 0;
    Test t[NUM];
    int size = 0;
    //BVList *list = bv_list_alloc();
    BVList *list = NULL;
    BVList *new_list = NULL;
    BVList *node = NULL;

    bv_list_init(&list);
    
    for (i = 0; i < NUM; i++) {
        bv_sprintf(t[i].name, 20, "test_%d", i);
        t[i].age = i + 20;
        t[i].t = bv_mallocz(20);
        bv_sprintf(t[i].t, 20, "test_%d", i);
#if 1
        node = bv_list_insert_after(list, node, &t[i]);
#else
        node = bv_list_push(list, &t[i]);
#endif
        if (node == NULL) {
            bv_log(NULL, BV_LOG_ERROR, "append node error\n");
        }
    }
    for (i = 0; i < NUM; i++) {
        node = bv_list_find(list, &t[i], find_node);
        if (node == NULL) {
            bv_log(NULL, BV_LOG_ERROR, "append node error\n");
        } else {
            Test *p = bv_list_get_data(list, node);
            bv_log(NULL, BV_LOG_ERROR, "data name %s age %d\n", p->name, p->age);
        }
    }
    bv_log(NULL, BV_LOG_ERROR, "===================================\n");
    node = list->next;
    while (node != list) {
        Test *p = bv_list_get_data(list, node);
        bv_log(NULL, BV_LOG_ERROR, "data name %s age %d\n", p->name, p->age);
        node = node->next;
    }
    bv_log(NULL, BV_LOG_ERROR, "===================================\n");
    size = bv_list_size(list);
    bv_log(NULL, BV_LOG_ERROR, "list size %d\n", size);
    Test *tt = bv_list_get_position_data(list, 19);
    if (tt) {
        bv_log(NULL, BV_LOG_ERROR, "pos data name %s age %d\n", tt->name, tt->age);
    }
    node = bv_list_find(list, &t[10], find_node);
    if (node == NULL) {
        bv_log(NULL, BV_LOG_ERROR, "append node error\n");
    } else {
        Test *p = bv_list_get_data(list, node);
        bv_log(NULL, BV_LOG_ERROR, "data name %s age %d\n", p->name, p->age);
        bv_list_free(list, node, free_node);
    }
    size = bv_list_size(list);
    bv_log(NULL, BV_LOG_ERROR, "list size %d\n", size);
    for (i = 0; i < size; i++) {
        Test *p = bv_list_pull(list);
        if (p) {
            bv_log(NULL, BV_LOG_ERROR, "data name %s age %d\n", p->name, p->age);
            free_node(p);
        }
    }
    size = bv_list_size(list);
    bv_log(NULL, BV_LOG_ERROR, "list size %d\n", size);
    bv_list_destroy(&list, free_node);
    return 0;
}
