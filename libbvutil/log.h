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

#ifndef BVUTIL_LOG_H
#define BVUTIL_LOG_H

#include <stdarg.h>
#include "bvutil.h"
#include "attributes.h"

typedef enum {
    BV_CLASS_CATEGORY_NA = 0,
    BV_CLASS_CATEGORY_INPUT,
    BV_CLASS_CATEGORY_OUTPUT,
    BV_CLASS_CATEGORY_MUXER,
    BV_CLASS_CATEGORY_DEMUXER,
    BV_CLASS_CATEGORY_ENCODER,
    BV_CLASS_CATEGORY_DECODER,
    BV_CLASS_CATEGORY_FILTER,
    BV_CLASS_CATEGORY_BITSTREAM_FILTER,
    BV_CLASS_CATEGORY_SWSCALER,
    BV_CLASS_CATEGORY_SWRESAMPLER,
    BV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT = 40,
    BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
    BV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
    BV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
    BV_CLASS_CATEGORY_DEVICE_OUTPUT,
    BV_CLASS_CATEGORY_DEVICE_INPUT,
    BV_CLASS_CATEGORY_NB, ///< not part of ABI/API
}BVClassCategory;

#define BV_IS_INPUT_DEVICE(category) \
    (((category) == BV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT) || \
     ((category) == BV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT) || \
     ((category) == BV_CLASS_CATEGORY_DEVICE_INPUT))

#define BV_IS_OUTPUT_DEVICE(category) \
    (((category) == BV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT) || \
     ((category) == BV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT) || \
     ((category) == BV_CLASS_CATEGORY_DEVICE_OUTPUT))

struct BVOptionRanges;

/**
 * Describe the class of an BVClass context structure. That is an
 * arbitrary struct of which the first field is a pointer to an
 * BVClass struct (e.g. BVCodecContext, BVFormatContext etc.).
 */
typedef struct BVClass {
    /**
     * The name of the class; usually it is the same name as the
     * context structure type to which the BVClass is associated.
     */
    const char* class_name;

    /**
     * A pointer to a function which returns the name of a context
     * instance ctx associated with the class.
     */
    const char* (*item_name)(void* ctx);

    /**
     * a pointer to the first option specified in the class if any or NULL
     *
     * @see bv_set_default_options()
     */
    const struct BVOption *option;

    /**
     * LIBBVUTIL_VERSION with which this structure was created.
     * This is used to allow fields to be added without requiring major
     * version bumps everywhere.
     */

    int version;

    /**
     * Offset in the structure where log_level_offset is stored.
     * 0 means there is no such variable
     */
    int log_level_offset_offset;

    /**
     * Offset in the structure where a pointer to the parent context for
     * logging is stored. For example a decoder could pass its BVCodecContext
     * to eval as such a parent context, which an bv_log() implementation
     * could then leverage to display the parent context.
     * The offset can be NULL.
     */
    int parent_log_context_offset;

    /**
     * Return next BVOptions-enabled child or NULL
     */
    void* (*child_next)(void *obj, void *prev);

    /**
     * Return an BVClass corresponding to the next potential
     * BVOptions-enabled child.
     *
     * The difference between child_next and this is that
     * child_next iterates over _already existing_ objects, while
     * child_class_next iterates over _all possible_ children.
     */
    const struct BVClass* (*child_class_next)(const struct BVClass *prev);

    /**
     * Category used for visualization (like color)
     * This is only set if the category is equal for all objects using this class.
     * available since version (51 << 16 | 56 << 8 | 100)
     */
    BVClassCategory category;

    /**
     * Callback to return the category.
     * available since version (51 << 16 | 59 << 8 | 100)
     */
    BVClassCategory (*get_category)(void* ctx);

    /**
     * Callback to return the supported/allowed ranges.
     * available since version (52.12)
     */
    int (*query_ranges)(struct BVOptionRanges **, void *obj, const char *key, int flags);
} BVClass;

/**
 * @addtogroup lavu_log
 *
 * @{
 *
 * @defgroup lavu_log_constants Logging Constants
 *
 * @{
 */

/**
 * Print no output.
 */
#define BV_LOG_QUIET    -8

/**
 * Something went really wrong and we will crash now.
 */
#define BV_LOG_PANIC     0

/**
 * Something went wrong and recovery is not possible.
 * For example, no header was found for a format which depends
 * on headers or an illegal combination of parameters is used.
 */
#define BV_LOG_FATAL     8

/**
 * Something went wrong and cannot losslessly be recovered.
 * However, not all future data is affected.
 */
#define BV_LOG_ERROR    16

/**
 * Something somehow does not look correct. This may or may not
 * lead to problems. An example would be the use of '-vstrict -2'.
 */
#define BV_LOG_WARNING  24

/**
 * Standard information.
 */
#define BV_LOG_INFO     32

/**
 * Detailed information.
 */
#define BV_LOG_VERBOSE  40

/**
 * Stuff which is only useful for libav* developers.
 */
#define BV_LOG_DEBUG    48

#define BV_LOG_MAX_OFFSET (BV_LOG_DEBUG - BV_LOG_QUIET)

/**
 * @}
 */

/**
 * Sets additional colors for extended debugging sessions.
 * @code
   bv_log(ctx, BV_LOG_DEBUG|BV_LOG_C(134), "Message in purple\n");
   @endcode
 * Requires 256color terminal support. Uses outside debugging is not
 * recommended.
 */
#define BV_LOG_C(x) (x << 8)

/**
 * Send the specified message to the log if the level is less than or equal
 * to the current bv_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * @see bv_log_set_callback
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an BVClass struct.
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 */
void bv_log(void *avcl, int level, const char *fmt, ...) bv_printf_format(3, 4);


/**
 * Send the specified message to the log if the level is less than or equal
 * to the current bv_log_level. By default, all logging messages are sent to
 * stderr. This behavior can be altered by setting a different logging callback
 * function.
 * @see bv_log_set_callback
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an BVClass struct.
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * @param vl The arguments referenced by the format string.
 */
void bv_vlog(void *avcl, int level, const char *fmt, va_list vl);

/**
 * Get the current log level
 *
 * @see lavu_log_constants
 *
 * @return Current log level
 */
int bv_log_get_level(void);

/**
 * Set the log level
 *
 * @see lavu_log_constants
 *
 * @param level Logging level
 */
void bv_log_set_level(int level);

/**
 * Set the logging callback
 *
 * @note The callback must be thread safe, even if the application does not use
 *       threads itself as some codecs are multithreaded.
 *
 * @see bv_log_default_callback
 *
 * @param callback A logging function with a compatible signature.
 */
void bv_log_set_callback(void (*callback)(void*, int, const char*, va_list));

/**
 * Default logging callback
 *
 * It prints the message to stderr, optionally colorizing it.
 *
 * @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an BVClass struct.
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
 * @param vl The arguments referenced by the format string.
 */
void bv_log_default_callback(void *avcl, int level, const char *fmt,
                             va_list vl);

/**
 * Return the context name
 *
 * @param  ctx The BVClass context
 *
 * @return The BVClass class_name
 */
const char* bv_default_item_name(void* ctx);
BVClassCategory bv_default_get_category(void *ptr);

/**
 * Format a line of log the same way as the default callback.
 * @param line          buffer to receive the formated line
 * @param line_size     size of the buffer
 * @param print_prefix  used to store whether the prefix must be printed;
 *                      must point to a persistent integer initially set to 1
 */
void bv_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);

/**
 * bv_dlog macros
 * Useful to print debug messages that shouldn't get compiled in normally.
 */

#ifdef DEBUG
#    define bv_dlog(pctx, ...) bv_log(pctx, BV_LOG_DEBUG, __VA_ARGS__)
#else
#    define bv_dlog(pctx, ...) do { if (0) bv_log(pctx, BV_LOG_DEBUG, __VA_ARGS__); } while (0)
#endif

/**
 * Skip repeated messages, this requires the user app to use bv_log() instead of
 * (f)printf as the 2 would otherwise interfere and lead to
 * "Last message repeated x times" messages below (f)printf messages with some
 * bad luck.
 * Also to receive the last, "last repeated" line if any, the user app must
 * call bv_log(NULL, BV_LOG_QUIET, "%s", ""); at the end
 */
#define BV_LOG_SKIP_REPEATED 1

/**
 * Include the log severity in messages originating from codecs.
 *
 * Results in messages such as:
 * [rawvideo @ 0xDEADBEEF] [error] encode did not produce valid pts
 */
#define BV_LOG_PRINT_LEVEL 2

void bv_log_set_flags(int arg);
int bv_log_get_flags(void);

/**
 * @}
 */

#endif /* BVUTIL_LOG_H */
