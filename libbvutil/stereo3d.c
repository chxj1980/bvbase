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

#include <stdint.h>
#include <string.h>

#include "mem.h"
#include "stereo3d.h"

BVStereo3D *bv_stereo3d_alloc(void)
{
    return bv_mallocz(sizeof(BVStereo3D));
}

BVStereo3D *bv_stereo3d_create_side_data(BVFrame *frame)
{
    BVFrameSideData *side_data = bv_frame_new_side_data(frame,
                                                        BV_FRAME_DATA_STEREO3D,
                                                        sizeof(BVStereo3D));
    if (!side_data)
        return NULL;

    memset(side_data->data, 0, sizeof(BVStereo3D));

    return (BVStereo3D *)side_data->data;
}
