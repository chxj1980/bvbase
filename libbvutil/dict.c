/*
 * copyright (c) 2009 Michael Niedermayer
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

#include <string.h>

#include "bvstring.h"
#include "dict.h"
#include "internal.h"
#include "mem.h"
#include "bprint.h"

struct BVDictionary {
    int count;
    BVDictionaryEntry *elems;
};

int bv_dict_count(const BVDictionary *m)
{
    return m ? m->count : 0;
}

BVDictionaryEntry *bv_dict_get(const BVDictionary *m, const char *key,
                               const BVDictionaryEntry *prev, int flags)
{
    unsigned int i, j;

    if (!m)
        return NULL;

    if (prev)
        i = prev - m->elems + 1;
    else
        i = 0;

    for (; i < m->count; i++) {
        const char *s = m->elems[i].key;
        if (flags & BV_DICT_MATCH_CASE)
            for (j = 0; s[j] == key[j] && key[j]; j++)
                ;
        else
            for (j = 0; bv_toupper(s[j]) == bv_toupper(key[j]) && key[j]; j++)
                ;
        if (key[j])
            continue;
        if (s[j] && !(flags & BV_DICT_IGNORE_SUFFIX))
            continue;
        return &m->elems[i];
    }
    return NULL;
}

int bv_dict_set(BVDictionary **pm, const char *key, const char *value,
                int flags)
{
    BVDictionary *m = *pm;
    BVDictionaryEntry *tag = bv_dict_get(m, key, NULL, flags);
    char *oldval = NULL;

    if (!m)
        m = *pm = bv_mallocz(sizeof(*m));
    if (!m)
        goto err_out;

    if (tag) {
        if (flags & BV_DICT_DONT_OVERWRITE) {
            if (flags & BV_DICT_DONT_STRDUP_KEY) bv_free((void*)key);
            if (flags & BV_DICT_DONT_STRDUP_VAL) bv_free((void*)value);
            return 0;
        }
        if (flags & BV_DICT_APPEND)
            oldval = tag->value;
        else
            bv_free(tag->value);
        bv_free(tag->key);
        *tag = m->elems[--m->count];
    } else {
        BVDictionaryEntry *tmp = bv_realloc(m->elems,
                                            (m->count + 1) * sizeof(*m->elems));
        if (!tmp)
            goto err_out;
        m->elems = tmp;
    }
    if (value) {
        if (flags & BV_DICT_DONT_STRDUP_KEY)
            m->elems[m->count].key = (char*)(intptr_t)key;
        else
            m->elems[m->count].key = bv_strdup(key);
        if (!m->elems[m->count].key)
            goto err_out;
        if (flags & BV_DICT_DONT_STRDUP_VAL) {
            m->elems[m->count].value = (char*)(intptr_t)value;
        } else if (oldval && flags & BV_DICT_APPEND) {
            int len = strlen(oldval) + strlen(value) + 1;
            char *newval = bv_mallocz(len);
            if (!newval)
                goto err_out;
            bv_strlcat(newval, oldval, len);
            bv_freep(&oldval);
            bv_strlcat(newval, value, len);
            m->elems[m->count].value = newval;
        } else
            m->elems[m->count].value = bv_strdup(value);
        m->count++;
    }
    if (!m->count) {
        bv_freep(&m->elems);
        bv_freep(pm);
    }

    return 0;

err_out:
    if (m && !m->count) {
        bv_freep(&m->elems);
        bv_freep(pm);
    }
    if (flags & BV_DICT_DONT_STRDUP_KEY) bv_free((void*)key);
    if (flags & BV_DICT_DONT_STRDUP_VAL) bv_free((void*)value);
    return BVERROR(ENOMEM);
}

int bv_dict_set_int(BVDictionary **pm, const char *key, int64_t value,
                int flags)
{
    char valuestr[22];
    snprintf(valuestr, sizeof(valuestr), "%"PRId64, value);
    return bv_dict_set(pm, key, valuestr, flags);
}

static int parse_key_value_pair(BVDictionary **pm, const char **buf,
                                const char *key_val_sep, const char *pairs_sep,
                                int flags)
{
    char *key = bv_get_token(buf, key_val_sep);
    char *val = NULL;
    int ret;

    if (key && *key && strspn(*buf, key_val_sep)) {
        (*buf)++;
        val = bv_get_token(buf, pairs_sep);
    }

    if (key && *key && val && *val)
        ret = bv_dict_set(pm, key, val, flags);
    else
        ret = BVERROR(EINVAL);

    bv_freep(&key);
    bv_freep(&val);

    return ret;
}

int bv_dict_parse_string(BVDictionary **pm, const char *str,
                         const char *key_val_sep, const char *pairs_sep,
                         int flags)
{
    int ret;

    if (!str)
        return 0;

    /* ignore STRDUP flags */
    flags &= ~(BV_DICT_DONT_STRDUP_KEY | BV_DICT_DONT_STRDUP_VAL);

    while (*str) {
        if ((ret = parse_key_value_pair(pm, &str, key_val_sep, pairs_sep, flags)) < 0)
            return ret;

        if (*str)
            str++;
    }

    return 0;
}

void bv_dict_free(BVDictionary **pm)
{
    BVDictionary *m = *pm;

    if (m) {
        while (m->count--) {
            bv_freep(&m->elems[m->count].key);
            bv_freep(&m->elems[m->count].value);
        }
        bv_freep(&m->elems);
    }
    bv_freep(pm);
}

void bv_dict_copy(BVDictionary **dst, const BVDictionary *src, int flags)
{
    BVDictionaryEntry *t = NULL;

    while ((t = bv_dict_get(src, "", t, BV_DICT_IGNORE_SUFFIX)))
        bv_dict_set(dst, t->key, t->value, flags);
}

int bv_dict_get_string(const BVDictionary *m, char **buffer,
                       const char key_val_sep, const char pairs_sep)
{
    BVDictionaryEntry *t = NULL;
    BVBPrint bprint;
    int cnt = 0;
    char special_chars[] = {pairs_sep, key_val_sep, '\0'};

    if (!buffer || pairs_sep == '\0' || key_val_sep == '\0' || pairs_sep == key_val_sep ||
        pairs_sep == '\\' || key_val_sep == '\\')
        return BVERROR(EINVAL);

    if (!bv_dict_count(m)) {
        *buffer = bv_strdup("");
        return *buffer ? 0 : BVERROR(ENOMEM);
    }

    bv_bprint_init(&bprint, 64, BV_BPRINT_SIZE_UNLIMITED);
    while ((t = bv_dict_get(m, "", t, BV_DICT_IGNORE_SUFFIX))) {
        if (cnt++)
            bv_bprint_append_data(&bprint, &pairs_sep, 1);
        bv_bprint_escape(&bprint, t->key, special_chars, BV_ESCAPE_MODE_BACKSLASH, 0);
        bv_bprint_append_data(&bprint, &key_val_sep, 1);
        bv_bprint_escape(&bprint, t->value, special_chars, BV_ESCAPE_MODE_BACKSLASH, 0);
    }
    return bv_bprint_finalize(&bprint, buffer);
}

#ifdef TEST
static void print_dict(const BVDictionary *m)
{
    BVDictionaryEntry *t = NULL;
    while ((t = bv_dict_get(m, "", t, BV_DICT_IGNORE_SUFFIX)))
        printf("%s %s   ", t->key, t->value);
    printf("\n");
}

static void test_separators(const BVDictionary *m, const char pair, const char val)
{
    BVDictionary *dict = NULL;
    char pairs[] = {pair , '\0'};
    char vals[]  = {val, '\0'};

    char *buffer = NULL;
    bv_dict_copy(&dict, m, 0);
    print_dict(dict);
    bv_dict_get_string(dict, &buffer, val, pair);
    printf("%s\n", buffer);
    bv_dict_free(&dict);
    bv_dict_parse_string(&dict, buffer, vals, pairs, 0);
    bv_freep(&buffer);
    print_dict(dict);
    bv_dict_free(&dict);
}

int main(void)
{
    BVDictionary *dict = NULL;
    char *buffer = NULL;

    printf("Testing bv_dict_get_string() and bv_dict_parse_string()\n");
    bv_dict_get_string(dict, &buffer, '=', ',');
    printf("%s\n", buffer);
    bv_freep(&buffer);
    bv_dict_set(&dict, "aaa", "aaa", 0);
    bv_dict_set(&dict, "b,b", "bbb", 0);
    bv_dict_set(&dict, "c=c", "ccc", 0);
    bv_dict_set(&dict, "ddd", "d,d", 0);
    bv_dict_set(&dict, "eee", "e=e", 0);
    bv_dict_set(&dict, "f,f", "f=f", 0);
    bv_dict_set(&dict, "g=g", "g,g", 0);
    test_separators(dict, ',', '=');
    bv_dict_free(&dict);
    bv_dict_set(&dict, "aaa", "aaa", 0);
    bv_dict_set(&dict, "bbb", "bbb", 0);
    bv_dict_set(&dict, "ccc", "ccc", 0);
    bv_dict_set(&dict, "\\,=\'\"", "\\,=\'\"", 0);
    test_separators(dict, '"',  '=');
    test_separators(dict, '\'', '=');
    test_separators(dict, ',', '"');
    test_separators(dict, ',', '\'');
    test_separators(dict, '\'', '"');
    test_separators(dict, '"', '\'');
    bv_dict_free(&dict);

    return 0;
}
#endif
