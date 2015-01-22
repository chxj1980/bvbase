/*
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

#include "libbvutil/cpu.h"
#include "libbvutil/cpu_internal.h"
#include "config.h"

int bb_get_cpu_flags_aarch64(void)
{
    return BV_CPU_FLAG_ARMV8 * BV_HAVE_ARMV8 |
           BV_CPU_FLAG_NEON  * BV_HAVE_NEON  |
           BV_CPU_FLAG_VFP   * BV_HAVE_VFP;
}
