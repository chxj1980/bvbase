/*
 * Copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
 * Copyright (c) 2008 Peter Ross
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

#ifndef BVUTIL_CHANNEL_LAYOUT_H
#define BVUTIL_CHANNEL_LAYOUT_H

#include <stdint.h>

/**
 * @file
 * audio channel layout utility functions
 */

/**
 * @addtogroup lavu_audio
 * @{
 */

/**
 * @defgroup channel_masks Audio channel masks
 *
 * A channel layout is a 64-bits integer with a bit set for every channel.
 * The number of bits set must be equal to the number of channels.
 * The value 0 means that the channel layout is not known.
 * @note this data structure is not powerful enough to handle channels
 * combinations that have the same channel multiple times, such as
 * dual-mono.
 *
 * @{
 */
#define BV_CH_FRONT_LEFT             0x00000001
#define BV_CH_FRONT_RIGHT            0x00000002
#define BV_CH_FRONT_CENTER           0x00000004
#define BV_CH_LOW_FREQUENCY          0x00000008
#define BV_CH_BACK_LEFT              0x00000010
#define BV_CH_BACK_RIGHT             0x00000020
#define BV_CH_FRONT_LEFT_OF_CENTER   0x00000040
#define BV_CH_FRONT_RIGHT_OF_CENTER  0x00000080
#define BV_CH_BACK_CENTER            0x00000100
#define BV_CH_SIDE_LEFT              0x00000200
#define BV_CH_SIDE_RIGHT             0x00000400
#define BV_CH_TOP_CENTER             0x00000800
#define BV_CH_TOP_FRONT_LEFT         0x00001000
#define BV_CH_TOP_FRONT_CENTER       0x00002000
#define BV_CH_TOP_FRONT_RIGHT        0x00004000
#define BV_CH_TOP_BACK_LEFT          0x00008000
#define BV_CH_TOP_BACK_CENTER        0x00010000
#define BV_CH_TOP_BACK_RIGHT         0x00020000
#define BV_CH_STEREO_LEFT            0x20000000  ///< Stereo downmix.
#define BV_CH_STEREO_RIGHT           0x40000000  ///< See BV_CH_STEREO_LEFT.
#define BV_CH_WIDE_LEFT              0x0000000080000000ULL
#define BV_CH_WIDE_RIGHT             0x0000000100000000ULL
#define BV_CH_SURROUND_DIRECT_LEFT   0x0000000200000000ULL
#define BV_CH_SURROUND_DIRECT_RIGHT  0x0000000400000000ULL
#define BV_CH_LOW_FREQUENCY_2        0x0000000800000000ULL

/** Channel mask value used for BVCodecContext.request_channel_layout
    to indicate that the user requests the channel order of the decoder output
    to be the native codec channel order. */
#define BV_CH_LAYOUT_NATIVE          0x8000000000000000ULL

/**
 * @}
 * @defgroup channel_mask_c Audio channel layouts
 * @{
 * */
#define BV_CH_LAYOUT_MONO              (BV_CH_FRONT_CENTER)
#define BV_CH_LAYOUT_STEREO            (BV_CH_FRONT_LEFT|BV_CH_FRONT_RIGHT)
#define BV_CH_LAYOUT_2POINT1           (BV_CH_LAYOUT_STEREO|BV_CH_LOW_FREQUENCY)
#define BV_CH_LAYOUT_2_1               (BV_CH_LAYOUT_STEREO|BV_CH_BACK_CENTER)
#define BV_CH_LAYOUT_SURROUND          (BV_CH_LAYOUT_STEREO|BV_CH_FRONT_CENTER)
#define BV_CH_LAYOUT_3POINT1           (BV_CH_LAYOUT_SURROUND|BV_CH_LOW_FREQUENCY)
#define BV_CH_LAYOUT_4POINT0           (BV_CH_LAYOUT_SURROUND|BV_CH_BACK_CENTER)
#define BV_CH_LAYOUT_4POINT1           (BV_CH_LAYOUT_4POINT0|BV_CH_LOW_FREQUENCY)
#define BV_CH_LAYOUT_2_2               (BV_CH_LAYOUT_STEREO|BV_CH_SIDE_LEFT|BV_CH_SIDE_RIGHT)
#define BV_CH_LAYOUT_QUAD              (BV_CH_LAYOUT_STEREO|BV_CH_BACK_LEFT|BV_CH_BACK_RIGHT)
#define BV_CH_LAYOUT_5POINT0           (BV_CH_LAYOUT_SURROUND|BV_CH_SIDE_LEFT|BV_CH_SIDE_RIGHT)
#define BV_CH_LAYOUT_5POINT1           (BV_CH_LAYOUT_5POINT0|BV_CH_LOW_FREQUENCY)
#define BV_CH_LAYOUT_5POINT0_BACK      (BV_CH_LAYOUT_SURROUND|BV_CH_BACK_LEFT|BV_CH_BACK_RIGHT)
#define BV_CH_LAYOUT_5POINT1_BACK      (BV_CH_LAYOUT_5POINT0_BACK|BV_CH_LOW_FREQUENCY)
#define BV_CH_LAYOUT_6POINT0           (BV_CH_LAYOUT_5POINT0|BV_CH_BACK_CENTER)
#define BV_CH_LAYOUT_6POINT0_FRONT     (BV_CH_LAYOUT_2_2|BV_CH_FRONT_LEFT_OF_CENTER|BV_CH_FRONT_RIGHT_OF_CENTER)
#define BV_CH_LAYOUT_HEXAGONAL         (BV_CH_LAYOUT_5POINT0_BACK|BV_CH_BACK_CENTER)
#define BV_CH_LAYOUT_6POINT1           (BV_CH_LAYOUT_5POINT1|BV_CH_BACK_CENTER)
#define BV_CH_LAYOUT_6POINT1_BACK      (BV_CH_LAYOUT_5POINT1_BACK|BV_CH_BACK_CENTER)
#define BV_CH_LAYOUT_6POINT1_FRONT     (BV_CH_LAYOUT_6POINT0_FRONT|BV_CH_LOW_FREQUENCY)
#define BV_CH_LAYOUT_7POINT0           (BV_CH_LAYOUT_5POINT0|BV_CH_BACK_LEFT|BV_CH_BACK_RIGHT)
#define BV_CH_LAYOUT_7POINT0_FRONT     (BV_CH_LAYOUT_5POINT0|BV_CH_FRONT_LEFT_OF_CENTER|BV_CH_FRONT_RIGHT_OF_CENTER)
#define BV_CH_LAYOUT_7POINT1           (BV_CH_LAYOUT_5POINT1|BV_CH_BACK_LEFT|BV_CH_BACK_RIGHT)
#define BV_CH_LAYOUT_7POINT1_WIDE      (BV_CH_LAYOUT_5POINT1|BV_CH_FRONT_LEFT_OF_CENTER|BV_CH_FRONT_RIGHT_OF_CENTER)
#define BV_CH_LAYOUT_7POINT1_WIDE_BACK (BV_CH_LAYOUT_5POINT1_BACK|BV_CH_FRONT_LEFT_OF_CENTER|BV_CH_FRONT_RIGHT_OF_CENTER)
#define BV_CH_LAYOUT_OCTAGONAL         (BV_CH_LAYOUT_5POINT0|BV_CH_BACK_LEFT|BV_CH_BACK_CENTER|BV_CH_BACK_RIGHT)
#define BV_CH_LAYOUT_STEREO_DOWNMIX    (BV_CH_STEREO_LEFT|BV_CH_STEREO_RIGHT)

enum AVMatrixEncoding {
    BV_MATRIX_ENCODING_NONE,
    BV_MATRIX_ENCODING_DOLBY,
    BV_MATRIX_ENCODING_DPLII,
    BV_MATRIX_ENCODING_DPLIIX,
    BV_MATRIX_ENCODING_DPLIIZ,
    BV_MATRIX_ENCODING_DOLBYEX,
    BV_MATRIX_ENCODING_DOLBYHEADPHONE,
    BV_MATRIX_ENCODING_NB
};

/**
 * Return a channel layout id that matches name, or 0 if no match is found.
 *
 * name can be one or several of the following notations,
 * separated by '+' or '|':
 * - the name of an usual channel layout (mono, stereo, 4.0, quad, 5.0,
 *   5.0(side), 5.1, 5.1(side), 7.1, 7.1(wide), downmix);
 * - the name of a single channel (FL, FR, FC, LFE, BL, BR, FLC, FRC, BC,
 *   SL, SR, TC, TFL, TFC, TFR, TBL, TBC, TBR, DL, DR);
 * - a number of channels, in decimal, optionally followed by 'c', yielding
 *   the default channel layout for that number of channels (@see
 *   bv_get_default_channel_layout);
 * - a channel layout mask, in hexadecimal starting with "0x" (see the
 *   BV_CH_* macros).
 *
 * @warning Starting from the next major bump the trailing character
 * 'c' to specify a number of channels will be required, while a
 * channel layout mask could also be specified as a decimal number
 * (if and only if not followed by "c").
 *
 * Example: "stereo+FC" = "2c+FC" = "2c+1c" = "0x7"
 */
uint64_t bv_get_channel_layout(const char *name);

/**
 * Return a description of a channel layout.
 * If nb_channels is <= 0, it is guessed from the channel_layout.
 *
 * @param buf put here the string containing the channel layout
 * @param buf_size size in bytes of the buffer
 */
void bv_get_channel_layout_string(char *buf, int buf_size, int nb_channels, uint64_t channel_layout);

struct BVBPrint;
/**
 * Append a description of a channel layout to a bprint buffer.
 */
void bv_bprint_channel_layout(struct BVBPrint *bp, int nb_channels, uint64_t channel_layout);

/**
 * Return the number of channels in the channel layout.
 */
int bv_get_channel_layout_nb_channels(uint64_t channel_layout);

/**
 * Return default channel layout for a given number of channels.
 */
int64_t bv_get_default_channel_layout(int nb_channels);

/**
 * Get the index of a channel in channel_layout.
 *
 * @param channel a channel layout describing exactly one channel which must be
 *                present in channel_layout.
 *
 * @return index of channel in channel_layout on success, a negative BVERROR
 *         on error.
 */
int bv_get_channel_layout_channel_index(uint64_t channel_layout,
                                        uint64_t channel);

/**
 * Get the channel with the given index in channel_layout.
 */
uint64_t bv_channel_layout_extract_channel(uint64_t channel_layout, int index);

/**
 * Get the name of a given channel.
 *
 * @return channel name on success, NULL on error.
 */
const char *bv_get_channel_name(uint64_t channel);

/**
 * Get the description of a given channel.
 *
 * @param channel  a channel layout with a single channel
 * @return  channel description on success, NULL on error
 */
const char *bv_get_channel_description(uint64_t channel);

/**
 * Get the value and name of a standard channel layout.
 *
 * @param[in]  index   index in an internal list, starting at 0
 * @param[out] layout  channel layout mask
 * @param[out] name    name of the layout
 * @return  0  if the layout exists,
 *          <0 if index is beyond the limits
 */
int bv_get_standard_channel_layout(unsigned index, uint64_t *layout,
                                   const char **name);

/**
 * @}
 * @}
 */

#endif /* BVUTIL_CHANNEL_LAYOUT_H */
