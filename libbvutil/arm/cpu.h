/*
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

#ifndef BVUTIL_ARM_CPU_H
#define BVUTIL_ARM_CPU_H

#include "config.h"
#include "libbvutil/cpu.h"
#include "libbvutil/cpu_internal.h"

#define have_armv5te(flags) CPUEXT(flags, ARMV5TE)
#define have_armv6(flags)   CPUEXT(flags, ARMV6)
#define have_armv6t2(flags) CPUEXT(flags, ARMV6T2)
#define have_vfp(flags)     CPUEXT(flags, VFP)
#define have_vfpv3(flags)   CPUEXT(flags, VFPV3)
#define have_neon(flags)    CPUEXT(flags, NEON)
#define have_setend(flags)  CPUEXT(flags, SETEND)

#endif /* BVUTIL_ARM_CPU_H */
