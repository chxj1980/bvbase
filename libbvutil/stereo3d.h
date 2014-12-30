/*
 * Copyright (c) 2013 Vittorio Giovara <vittorio.giovara@gmail.com>
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

#ifndef BVUTIL_STEREO3D_H
#define BVUTIL_STEREO3D_H

#include <stdint.h>

#include "frame.h"

/**
 * List of possible 3D Types
 */
enum BVStereo3DType {
    /**
     * Video is not stereoscopic (and metadata has to be there).
     */
    BV_STEREO3D_2D,

    /**
     * Views are next to each other.
     *
     *    LLLLRRRR
     *    LLLLRRRR
     *    LLLLRRRR
     *    ...
     */
    BV_STEREO3D_SIDEBYSIDE,

    /**
     * Views are on top of each other.
     *
     *    LLLLLLLL
     *    LLLLLLLL
     *    RRRRRRRR
     *    RRRRRRRR
     */
    BV_STEREO3D_TOPBOTTOM,

    /**
     * Views are alternated temporally.
     *
     *     frame0   frame1   frame2   ...
     *    LLLLLLLL RRRRRRRR LLLLLLLL
     *    LLLLLLLL RRRRRRRR LLLLLLLL
     *    LLLLLLLL RRRRRRRR LLLLLLLL
     *    ...      ...      ...
     */
    BV_STEREO3D_FRAMESEQUENCE,

    /**
     * Views are packed in a checkerboard-like structure per pixel.
     *
     *    LRLRLRLR
     *    RLRLRLRL
     *    LRLRLRLR
     *    ...
     */
    BV_STEREO3D_CHECKERBOARD,

    /**
     * Views are next to each other, but when upscaling
     * apply a checkerboard pattern.
     *
     *     LLLLRRRR          L L L L    R R R R
     *     LLLLRRRR    =>     L L L L  R R R R
     *     LLLLRRRR          L L L L    R R R R
     *     LLLLRRRR           L L L L  R R R R
     */
    BV_STEREO3D_SIDEBYSIDE_QUINCUNX,

    /**
     * Views are packed per line, as if interlaced.
     *
     *    LLLLLLLL
     *    RRRRRRRR
     *    LLLLLLLL
     *    ...
     */
    BV_STEREO3D_LINES,

    /**
     * Views are packed per column.
     *
     *    LRLRLRLR
     *    LRLRLRLR
     *    LRLRLRLR
     *    ...
     */
    BV_STEREO3D_COLUMNS,
};


/**
 * Inverted views, Right/Bottom represents the left view.
 */
#define BV_STEREO3D_FLAG_INVERT     (1 << 0)

/**
 * Stereo 3D type: this structure describes how two videos are packed
 * within a single video surface, with additional information as needed.
 *
 * @note The struct must be allocated with bv_stereo3d_alloc() and
 *       its size is not a part of the public ABI.
 */
typedef struct BVStereo3D {
    /**
     * How views are packed within the video.
     */
    enum BVStereo3DType type;

    /**
     * Additional information about the frame packing.
     */
    int flags;
} BVStereo3D;

/**
 * Allocate an BVStereo3D structure and set its fields to default values.
 * The resulting struct can be freed using bv_freep().
 *
 * @return An BVStereo3D filled with default values or NULL on failure.
 */
BVStereo3D *bv_stereo3d_alloc(void);

/**
 * Allocate a complete BVFrameSideData and add it to the frame.
 *
 * @param frame The frame which side data is added to.
 *
 * @return The BVStereo3D structure to be filled by caller.
 */
BVStereo3D *bv_stereo3d_create_side_data(BVFrame *frame);

#endif /* BVUTIL_STEREO3D_H */
