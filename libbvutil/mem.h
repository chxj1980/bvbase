/*
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
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

/**
 * @file
 * memory handling functions
 */

#ifndef BVUTIL_MEM_H
#define BVUTIL_MEM_H

#include <limits.h>
#include <stdint.h>

#include "attributes.h"
#include "error.h"
#include "bvutil.h"

/**
 * @addtogroup lavu_mem
 * @{
 */


#if defined(__INTEL_COMPILER) && __INTEL_COMPILER < 1110 || defined(__SUNPRO_C)
    #define DECLARE_ALIGNED(n,t,v)      t __attribute__ ((aligned (n))) v
    #define DECLARE_ASM_CONST(n,t,v)    const t __attribute__ ((aligned (n))) v
#elif defined(__TI_COMPILER_VERSION__)
    #define DECLARE_ALIGNED(n,t,v)                      \
        BV_PRAGMA(DATA_ALIGN(v,n))                      \
        t __attribute__((aligned(n))) v
    #define DECLARE_ASM_CONST(n,t,v)                    \
        BV_PRAGMA(DATA_ALIGN(v,n))                      \
        static const t __attribute__((aligned(n))) v
#elif defined(__GNUC__)
    #define DECLARE_ALIGNED(n,t,v)      t __attribute__ ((aligned (n))) v
    #define DECLARE_ASM_CONST(n,t,v)    static const t bv_used __attribute__ ((aligned (n))) v
#elif defined(_MSC_VER)
    #define DECLARE_ALIGNED(n,t,v)      __declspec(align(n)) t v
    #define DECLARE_ASM_CONST(n,t,v)    __declspec(align(n)) static const t v
#else
    #define DECLARE_ALIGNED(n,t,v)      t v
    #define DECLARE_ASM_CONST(n,t,v)    static const t v
#endif

#if BV_GCC_VERSION_AT_LEAST(3,1)
    #define bv_malloc_attrib __attribute__((__malloc__))
#else
    #define bv_malloc_attrib
#endif

#if BV_GCC_VERSION_AT_LEAST(4,3)
    #define bv_alloc_size(...) __attribute__((alloc_size(__VA_ARGS__)))
#else
    #define bv_alloc_size(...)
#endif

/**
 * Allocate a block of size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU).
 * @param size Size in bytes for the memory block to be allocated.
 * @return Pointer to the allocated block, NULL if the block cannot
 * be allocated.
 * @see bv_mallocz()
 */
void *bv_malloc(size_t size) bv_malloc_attrib bv_alloc_size(1);

/**
 * Allocate a block of size * nmemb bytes with bv_malloc().
 * @param nmemb Number of elements
 * @param size Size of the single element
 * @return Pointer to the allocated block, NULL if the block cannot
 * be allocated.
 * @see bv_malloc()
 */
bv_alloc_size(1, 2) static inline void *bv_malloc_array(size_t nmemb, size_t size)
{
    if (!size || nmemb >= INT_MAX / size)
        return NULL;
    return bv_malloc(nmemb * size);
}

/**
 * Allocate or reallocate a block of memory.
 * If ptr is NULL and size > 0, allocate a new block. If
 * size is zero, free the memory block pointed to by ptr.
 * @param ptr Pointer to a memory block already allocated with
 * bv_realloc() or NULL.
 * @param size Size in bytes of the memory block to be allocated or
 * reallocated.
 * @return Pointer to a newly-reallocated block or NULL if the block
 * cannot be reallocated or the function is used to free the memory block.
 * @warning Pointers originating from the bv_malloc() family of functions must
 *          not be passed to bv_realloc(). The former can be implemented using
 *          memalign() (or other functions), and there is no guarantee that
 *          pointers from such functions can be passed to realloc() at all.
 *          The situation is undefined according to POSIX and may crash with
 *          some libc implementations.
 * @see bv_fast_realloc()
 */
void *bv_realloc(void *ptr, size_t size) bv_alloc_size(2);

/**
 * Allocate or reallocate a block of memory.
 * This function does the same thing as bv_realloc, except:
 * - It takes two arguments and checks the result of the multiplication for
 *   integer overflow.
 * - It frees the input block in case of failure, thus avoiding the memory
 *   leak with the classic "buf = realloc(buf); if (!buf) return -1;".
 */
void *bv_realloc_f(void *ptr, size_t nelem, size_t elsize);

/**
 * Allocate or reallocate a block of memory.
 * If *ptr is NULL and size > 0, allocate a new block. If
 * size is zero, free the memory block pointed to by ptr.
 * @param   ptr Pointer to a pointer to a memory block already allocated
 *          with bv_realloc(), or pointer to a pointer to NULL.
 *          The pointer is updated on success, or freed on failure.
 * @param   size Size in bytes for the memory block to be allocated or
 *          reallocated
 * @return  Zero on success, an BVERROR error code on failure.
 * @warning Pointers originating from the bv_malloc() family of functions must
 *          not be passed to bv_reallocp(). The former can be implemented using
 *          memalign() (or other functions), and there is no guarantee that
 *          pointers from such functions can be passed to realloc() at all.
 *          The situation is undefined according to POSIX and may crash with
 *          some libc implementations.
 */
int bv_reallocp(void *ptr, size_t size);

/**
 * Allocate or reallocate an array.
 * If ptr is NULL and nmemb > 0, allocate a new block. If
 * nmemb is zero, free the memory block pointed to by ptr.
 * @param ptr Pointer to a memory block already allocated with
 * bv_realloc() or NULL.
 * @param nmemb Number of elements
 * @param size Size of the single element
 * @return Pointer to a newly-reallocated block or NULL if the block
 * cannot be reallocated or the function is used to free the memory block.
 * @warning Pointers originating from the bv_malloc() family of functions must
 *          not be passed to bv_realloc(). The former can be implemented using
 *          memalign() (or other functions), and there is no guarantee that
 *          pointers from such functions can be passed to realloc() at all.
 *          The situation is undefined according to POSIX and may crash with
 *          some libc implementations.
 */
bv_alloc_size(2, 3) void *bv_realloc_array(void *ptr, size_t nmemb, size_t size);

/**
 * Allocate or reallocate an array through a pointer to a pointer.
 * If *ptr is NULL and nmemb > 0, allocate a new block. If
 * nmemb is zero, free the memory block pointed to by ptr.
 * @param ptr Pointer to a pointer to a memory block already allocated
 * with bv_realloc(), or pointer to a pointer to NULL.
 * The pointer is updated on success, or freed on failure.
 * @param nmemb Number of elements
 * @param size Size of the single element
 * @return Zero on success, an BVERROR error code on failure.
 * @warning Pointers originating from the bv_malloc() family of functions must
 *          not be passed to bv_realloc(). The former can be implemented using
 *          memalign() (or other functions), and there is no guarantee that
 *          pointers from such functions can be passed to realloc() at all.
 *          The situation is undefined according to POSIX and may crash with
 *          some libc implementations.
 */
bv_alloc_size(2, 3) int bv_reallocp_array(void *ptr, size_t nmemb, size_t size);

/**
 * Free a memory block which has been allocated with bv_malloc(z)() or
 * bv_realloc().
 * @param ptr Pointer to the memory block which should be freed.
 * @note ptr = NULL is explicitly allowed.
 * @note It is recommended that you use bv_freep() instead.
 * @see bv_freep()
 */
void bv_free(void *ptr);

/**
 * Allocate a block of size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU) and
 * zero all the bytes of the block.
 * @param size Size in bytes for the memory block to be allocated.
 * @return Pointer to the allocated block, NULL if it cannot be allocated.
 * @see bv_malloc()
 */
void *bv_mallocz(size_t size) bv_malloc_attrib bv_alloc_size(1);

/**
 * Allocate a block of nmemb * size bytes with alignment suitable for all
 * memory accesses (including vectors if available on the CPU) and
 * zero all the bytes of the block.
 * The allocation will fail if nmemb * size is greater than or equal
 * to INT_MAX.
 * @param nmemb
 * @param size
 * @return Pointer to the allocated block, NULL if it cannot be allocated.
 */
void *bv_calloc(size_t nmemb, size_t size) bv_malloc_attrib;

/**
 * Allocate a block of size * nmemb bytes with bv_mallocz().
 * @param nmemb Number of elements
 * @param size Size of the single element
 * @return Pointer to the allocated block, NULL if the block cannot
 * be allocated.
 * @see bv_mallocz()
 * @see bv_malloc_array()
 */
bv_alloc_size(1, 2) static inline void *bv_mallocz_array(size_t nmemb, size_t size)
{
    if (!size || nmemb >= INT_MAX / size)
        return NULL;
    return bv_mallocz(nmemb * size);
}

/**
 * Duplicate the string s.
 * @param s string to be duplicated
 * @return Pointer to a newly-allocated string containing a
 * copy of s or NULL if the string cannot be allocated.
 */
char *bv_strdup(const char *s) bv_malloc_attrib;

/**
 * Duplicate a substring of the string s.
 * @param s string to be duplicated
 * @param len the maximum length of the resulting string (not counting the
 *            terminating byte).
 * @return Pointer to a newly-allocated string containing a
 * copy of s or NULL if the string cannot be allocated.
 */
char *bv_strndup(const char *s, size_t len) bv_malloc_attrib;

/**
 * Duplicate the buffer p.
 * @param p buffer to be duplicated
 * @return Pointer to a newly allocated buffer containing a
 * copy of p or NULL if the buffer cannot be allocated.
 */
void *bv_memdup(const void *p, size_t size);

/**
 * Free a memory block which has been allocated with bv_malloc(z)() or
 * bv_realloc() and set the pointer pointing to it to NULL.
 * @param ptr Pointer to the pointer to the memory block which should
 * be freed.
 * @note passing a pointer to a NULL pointer is safe and leads to no action.
 * @see bv_free()
 */
void bv_freep(void *ptr);

/**
 * Add an element to a dynamic array.
 *
 * The array to grow is supposed to be an array of pointers to
 * structures, and the element to add must be a pointer to an already
 * allocated structure.
 *
 * The array is reallocated when its size reaches powers of 2.
 * Therefore, the amortized cost of adding an element is constant.
 *
 * In case of success, the pointer to the array is updated in order to
 * point to the new grown array, and the number pointed to by nb_ptr
 * is incremented.
 * In case of failure, the array is freed, *tab_ptr is set to NULL and
 * *nb_ptr is set to 0.
 *
 * @param tab_ptr pointer to the array to grow
 * @param nb_ptr  pointer to the number of elements in the array
 * @param elem    element to add
 * @see bv_dynarray_add_nofree(), bv_dynarray2_add()
 */
void bv_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem);

/**
 * Add an element to a dynamic array.
 *
 * Function has the same functionality as bv_dynarray_add(),
 * but it doesn't free memory on fails. It returns error code
 * instead and leave current buffer untouched.
 *
 * @param tab_ptr pointer to the array to grow
 * @param nb_ptr  pointer to the number of elements in the array
 * @param elem    element to add
 * @return >=0 on success, negative otherwise.
 * @see bv_dynarray_add(), bv_dynarray2_add()
 */
int bv_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem);

/**
 * Add an element of size elem_size to a dynamic array.
 *
 * The array is reallocated when its number of elements reaches powers of 2.
 * Therefore, the amortized cost of adding an element is constant.
 *
 * In case of success, the pointer to the array is updated in order to
 * point to the new grown array, and the number pointed to by nb_ptr
 * is incremented.
 * In case of failure, the array is freed, *tab_ptr is set to NULL and
 * *nb_ptr is set to 0.
 *
 * @param tab_ptr   pointer to the array to grow
 * @param nb_ptr    pointer to the number of elements in the array
 * @param elem_size size in bytes of the elements in the array
 * @param elem_data pointer to the data of the element to add. If NULL, the space of
 *                  the new added element is not filled.
 * @return          pointer to the data of the element to copy in the new allocated space.
 *                  If NULL, the new allocated space is left uninitialized."
 * @see bv_dynarray_add(), bv_dynarray_add_nofree()
 */
void *bv_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data);

/**
 * Multiply two size_t values checking for overflow.
 * @return  0 if success, BVERROR(EINVAL) if overflow.
 */
static inline int bv_size_mult(size_t a, size_t b, size_t *r)
{
    size_t t = a * b;
    /* Hack inspired from glibc: only try the division if nelem and elsize
     * are both greater than sqrt(SIZE_MAX). */
    if ((a | b) >= ((size_t)1 << (sizeof(size_t) * 4)) && a && t / a != b)
        return BVERROR(EINVAL);
    *r = t;
    return 0;
}

/**
 * Set the maximum size that may me allocated in one block.
 */
void bv_max_alloc(size_t max);

/**
 * deliberately overlapping memcpy implementation
 * @param dst destination buffer
 * @param back how many bytes back we start (the initial size of the overlapping window), must be > 0
 * @param cnt number of bytes to copy, must be >= 0
 *
 * cnt > back is valid, this will copy the bytes we just copied,
 * thus creating a repeating pattern with a period length of back.
 */
void bv_memcpy_backptr(uint8_t *dst, int back, int cnt);

/**
 * Reallocate the given block if it is not large enough, otherwise do nothing.
 *
 * @see bv_realloc
 */
void *bv_fast_realloc(void *ptr, unsigned int *size, size_t min_size);

/**
 * Allocate a buffer, reusing the given one if large enough.
 *
 * Contrary to bv_fast_realloc the current buffer contents might not be
 * preserved and on error the old buffer is freed, thus no special
 * handling to avoid memleaks is necessary.
 *
 * @param ptr pointer to pointer to already allocated buffer, overwritten with pointer to new buffer
 * @param size size of the buffer *ptr points to
 * @param min_size minimum size of *ptr buffer after returning, *ptr will be NULL and
 *                 *size 0 if an error occurred.
 */
void bv_fast_malloc(void *ptr, unsigned int *size, size_t min_size);

/**
 * @}
 */

#endif /* BVUTIL_MEM_H */
