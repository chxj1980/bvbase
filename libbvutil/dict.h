/*
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

/**
 * @file
 * Public dictionary API.
 * @deprecated
 *  BVDictionary is provided for compatibility with libav. It is both in
 *  implementation as well as API inefficient. It does not scale and is
 *  extremely slow with large dictionaries.
 *  It is recommended that new code uses our tree container from tree.c/h
 *  where applicable, which uses AVL trees to achieve O(log n) performance.
 */

#ifndef BVUTIL_DICT_H
#define BVUTIL_DICT_H

#include <stdint.h>

#include "version.h"

/**
 * @addtogroup lavu_dict BVDictionary
 * @ingroup lavu_data
 *
 * @brief Simple key:value store
 *
 * @{
 * Dictionaries are used for storing key:value pairs. To create
 * an BVDictionary, simply pass an address of a NULL pointer to
 * bv_dict_set(). NULL can be used as an empty dictionary wherever
 * a pointer to an BVDictionary is required.
 * Use bv_dict_get() to retrieve an entry or iterate over all
 * entries and finally bv_dict_free() to free the dictionary
 * and all its contents.
 *
 @code
   BVDictionary *d = NULL;           // "create" an empty dictionary
   BVDictionaryEntry *t = NULL;

   bv_dict_set(&d, "foo", "bar", 0); // add an entry

   char *k = bv_strdup("key");       // if your strings are already allocated,
   char *v = bv_strdup("value");     // you can avoid copying them like this
   bv_dict_set(&d, k, v, BV_DICT_DONT_STRDUP_KEY | BV_DICT_DONT_STRDUP_VAL);

   while (t = bv_dict_get(d, "", t, BV_DICT_IGNORE_SUFFIX)) {
       <....>                             // iterate over all entries in d
   }
   bv_dict_free(&d);
 @endcode
 *
 */

#define BV_DICT_MATCH_CASE      1   /**< Only get an entry with exact-case key match. Only relevant in bv_dict_get(). */
#define BV_DICT_IGNORE_SUFFIX   2   /**< Return first entry in a dictionary whose first part corresponds to the search key,
                                         ignoring the suffix of the found key string. Only relevant in bv_dict_get(). */
#define BV_DICT_DONT_STRDUP_KEY 4   /**< Take ownership of a key that's been
                                         allocated with bv_malloc() or another memory allocation function. */
#define BV_DICT_DONT_STRDUP_VAL 8   /**< Take ownership of a value that's been
                                         allocated with bv_malloc() or another memory allocation function. */
#define BV_DICT_DONT_OVERWRITE 16   ///< Don't overwrite existing entries.
#define BV_DICT_APPEND         32   /**< If the entry already exists, append to it.  Note that no
                                      delimiter is added, the strings are simply concatenated. */

typedef struct BVDictionaryEntry {
    char *key;
    char *value;
} BVDictionaryEntry;

typedef struct BVDictionary BVDictionary;

/**
 * Get a dictionary entry with matching key.
 *
 * The returned entry key or value must not be changed, or it will
 * cause undefined behavior.
 *
 * To iterate through all the dictionary entries, you can set the matching key
 * to the null string "" and set the BV_DICT_IGNORE_SUFFIX flag.
 *
 * @param prev Set to the previous matching element to find the next.
 *             If set to NULL the first matching element is returned.
 * @param key matching key
 * @param flags a collection of BV_DICT_* flags controlling how the entry is retrieved
 * @return found entry or NULL in case no matching entry was found in the dictionary
 */
BVDictionaryEntry *bv_dict_get(const BVDictionary *m, const char *key,
                               const BVDictionaryEntry *prev, int flags);

/**
 * Get number of entries in dictionary.
 *
 * @param m dictionary
 * @return  number of entries in dictionary
 */
int bv_dict_count(const BVDictionary *m);

/**
 * Set the given entry in *pm, overwriting an existing entry.
 *
 * Note: If BV_DICT_DONT_STRDUP_KEY or BV_DICT_DONT_STRDUP_VAL is set,
 * these arguments will be freed on error.
 *
 * @param pm pointer to a pointer to a dictionary struct. If *pm is NULL
 * a dictionary struct is allocated and put in *pm.
 * @param key entry key to add to *pm (will be bv_strduped depending on flags)
 * @param value entry value to add to *pm (will be bv_strduped depending on flags).
 *        Passing a NULL value will cause an existing entry to be deleted.
 * @return >= 0 on success otherwise an error code <0
 */
int bv_dict_set(BVDictionary **pm, const char *key, const char *value, int flags);

/**
 * Convenience wrapper for bv_dict_set that converts the value to a string
 * and stores it.
 *
 * Note: If BV_DICT_DONT_STRDUP_KEY is set, key will be freed on error.
 */
int bv_dict_set_int(BVDictionary **pm, const char *key, int64_t value, int flags);

/**
 * Parse the key/value pairs list and add the parsed entries to a dictionary.
 *
 * In case of failure, all the successfully set entries are stored in
 * *pm. You may need to manually free the created dictionary.
 *
 * @param key_val_sep  a 0-terminated list of characters used to separate
 *                     key from value
 * @param pairs_sep    a 0-terminated list of characters used to separate
 *                     two pairs from each other
 * @param flags        flags to use when adding to dictionary.
 *                     BV_DICT_DONT_STRDUP_KEY and BV_DICT_DONT_STRDUP_VAL
 *                     are ignored since the key/value tokens will always
 *                     be duplicated.
 * @return             0 on success, negative BVERROR code on failure
 */
int bv_dict_parse_string(BVDictionary **pm, const char *str,
                         const char *key_val_sep, const char *pairs_sep,
                         int flags);

/**
 * Copy entries from one BVDictionary struct into another.
 * @param dst pointer to a pointer to a BVDictionary struct. If *dst is NULL,
 *            this function will allocate a struct for you and put it in *dst
 * @param src pointer to source BVDictionary struct
 * @param flags flags to use when setting entries in *dst
 * @note metadata is read using the BV_DICT_IGNORE_SUFFIX flag
 */
void bv_dict_copy(BVDictionary **dst, const BVDictionary *src, int flags);

/**
 * Free all the memory allocated for an BVDictionary struct
 * and all keys and values.
 */
void bv_dict_free(BVDictionary **m);

/**
 * Get dictionary entries as a string.
 *
 * Create a string containing dictionary's entries.
 * Such string may be passed back to bv_dict_parse_string().
 * @note String is escaped with backslashes ('\').
 *
 * @param[in]  m             dictionary
 * @param[out] buffer        Pointer to buffer that will be allocated with string containg entries.
 *                           Buffer must be freed by the caller when is no longer needed.
 * @param[in]  key_val_sep   character used to separate key from value
 * @param[in]  pairs_sep     character used to separate two pairs from each other
 * @return                   >= 0 on success, negative on error
 * @warning Separators cannot be neither '\\' nor '\0'. They also cannot be the same.
 */
int bv_dict_get_string(const BVDictionary *m, char **buffer,
                       const char key_val_sep, const char pairs_sep);

/**
 * @}
 */

#endif /* BVUTIL_DICT_H */
