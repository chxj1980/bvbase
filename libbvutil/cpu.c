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

#include <stdint.h>

#include "cpu.h"
#include "cpu_internal.h"
#include "config.h"
#include "opt.h"
#include "common.h"

#if HAVE_SCHED_GETAFFINITY
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <sched.h>
#endif
#if HAVE_GETPROCESSAFFINITYMASK
#include <windows.h>
#endif
#if HAVE_SYSCTL
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

static int flags, checked;

void bv_force_cpu_flags(int arg){
    if (   (arg & ( BV_CPU_FLAG_3DNOW    |
                    BV_CPU_FLAG_3DNOWEXT |
                    BV_CPU_FLAG_SSE      |
                    BV_CPU_FLAG_SSE2     |
                    BV_CPU_FLAG_SSE2SLOW |
                    BV_CPU_FLAG_SSE3     |
                    BV_CPU_FLAG_SSE3SLOW |
                    BV_CPU_FLAG_SSSE3    |
                    BV_CPU_FLAG_SSE4     |
                    BV_CPU_FLAG_SSE42    |
                    BV_CPU_FLAG_AVX      |
                    BV_CPU_FLAG_XOP      |
                    BV_CPU_FLAG_FMA3     |
                    BV_CPU_FLAG_FMA4     |
                    BV_CPU_FLAG_AVX2     ))
        && !(arg & BV_CPU_FLAG_MMX)) {
        bv_log(NULL, BV_LOG_WARNING, "MMX implied by specified flags\n");
        arg |= BV_CPU_FLAG_MMX;
    }

    flags   = arg;
    checked = arg != -1;
}

int bv_get_cpu_flags(void)
{
    if (checked)
        return flags;

    if (ARCH_AARCH64)
        flags = ff_get_cpu_flags_aarch64();
    if (ARCH_ARM)
        flags = ff_get_cpu_flags_arm();
    if (ARCH_PPC)
        flags = ff_get_cpu_flags_ppc();
    if (ARCH_X86)
        flags = ff_get_cpu_flags_x86();

    checked = 1;
    return flags;
}

void bv_set_cpu_flags_mask(int mask)
{
    checked       = 0;
    flags         = bv_get_cpu_flags() & mask;
    checked       = 1;
}

int bv_parse_cpu_flags(const char *s)
{
#define CPUFLAG_MMXEXT   (BV_CPU_FLAG_MMX      | BV_CPU_FLAG_MMXEXT | BV_CPU_FLAG_CMOV)
#define CPUFLAG_3DNOW    (BV_CPU_FLAG_3DNOW    | BV_CPU_FLAG_MMX)
#define CPUFLAG_3DNOWEXT (BV_CPU_FLAG_3DNOWEXT | CPUFLAG_3DNOW)
#define CPUFLAG_SSE      (BV_CPU_FLAG_SSE      | CPUFLAG_MMXEXT)
#define CPUFLAG_SSE2     (BV_CPU_FLAG_SSE2     | CPUFLAG_SSE)
#define CPUFLAG_SSE2SLOW (BV_CPU_FLAG_SSE2SLOW | CPUFLAG_SSE2)
#define CPUFLAG_SSE3     (BV_CPU_FLAG_SSE3     | CPUFLAG_SSE2)
#define CPUFLAG_SSE3SLOW (BV_CPU_FLAG_SSE3SLOW | CPUFLAG_SSE3)
#define CPUFLAG_SSSE3    (BV_CPU_FLAG_SSSE3    | CPUFLAG_SSE3)
#define CPUFLAG_SSE4     (BV_CPU_FLAG_SSE4     | CPUFLAG_SSSE3)
#define CPUFLAG_SSE42    (BV_CPU_FLAG_SSE42    | CPUFLAG_SSE4)
#define CPUFLAG_AVX      (BV_CPU_FLAG_AVX      | CPUFLAG_SSE42)
#define CPUFLAG_XOP      (BV_CPU_FLAG_XOP      | CPUFLAG_AVX)
#define CPUFLAG_FMA3     (BV_CPU_FLAG_FMA3     | CPUFLAG_AVX)
#define CPUFLAG_FMA4     (BV_CPU_FLAG_FMA4     | CPUFLAG_AVX)
#define CPUFLAG_AVX2     (BV_CPU_FLAG_AVX2     | CPUFLAG_AVX)
#define CPUFLAG_BMI1     (BV_CPU_FLAG_BMI1)
#define CPUFLAG_BMI2     (BV_CPU_FLAG_BMI2     | CPUFLAG_BMI1)
    static const BVOption cpuflags_opts[] = {
        { "flags"   , NULL, 0, BV_OPT_TYPE_FLAGS, { .i64 = 0 }, INT64_MIN, INT64_MAX, .unit = "flags" },
#if   ARCH_PPC
        { "altivec" , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ALTIVEC  },    .unit = "flags" },
#elif ARCH_X86
        { "mmx"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_MMX      },    .unit = "flags" },
        { "mmxext"  , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_MMXEXT       },    .unit = "flags" },
        { "sse"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSE          },    .unit = "flags" },
        { "sse2"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSE2         },    .unit = "flags" },
        { "sse2slow", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSE2SLOW     },    .unit = "flags" },
        { "sse3"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSE3         },    .unit = "flags" },
        { "sse3slow", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSE3SLOW     },    .unit = "flags" },
        { "ssse3"   , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSSE3        },    .unit = "flags" },
        { "atom"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ATOM     },    .unit = "flags" },
        { "sse4.1"  , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSE4         },    .unit = "flags" },
        { "sse4.2"  , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_SSE42        },    .unit = "flags" },
        { "avx"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_AVX          },    .unit = "flags" },
        { "xop"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_XOP          },    .unit = "flags" },
        { "fma3"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_FMA3         },    .unit = "flags" },
        { "fma4"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_FMA4         },    .unit = "flags" },
        { "avx2"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_AVX2         },    .unit = "flags" },
        { "bmi1"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_BMI1         },    .unit = "flags" },
        { "bmi2"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_BMI2         },    .unit = "flags" },
        { "3dnow"   , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_3DNOW        },    .unit = "flags" },
        { "3dnowext", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPUFLAG_3DNOWEXT     },    .unit = "flags" },
        { "cmov",     NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_CMOV     },    .unit = "flags" },
#elif ARCH_ARM
        { "armv5te",  NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV5TE  },    .unit = "flags" },
        { "armv6",    NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV6    },    .unit = "flags" },
        { "armv6t2",  NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV6T2  },    .unit = "flags" },
        { "vfp",      NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_VFP      },    .unit = "flags" },
        { "vfpv3",    NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_VFPV3    },    .unit = "flags" },
        { "neon",     NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_NEON     },    .unit = "flags" },
#elif ARCH_AARCH64
        { "armv8",    NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV8    },    .unit = "flags" },
        { "neon",     NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_NEON     },    .unit = "flags" },
        { "vfp",      NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_VFP      },    .unit = "flags" },
#endif
        { NULL },
    };
    static const BVClass class = {
        .class_name = "cpuflags",
        .item_name  = bv_default_item_name,
        .option     = cpuflags_opts,
        .version    = LIBBVUTIL_VERSION_INT,
    };

    int flags = 0, ret;
    const BVClass *pclass = &class;

    if ((ret = bv_opt_eval_flags(&pclass, &cpuflags_opts[0], s, &flags)) < 0)
        return ret;

    return flags & INT_MAX;
}

int bv_parse_cpu_caps(unsigned *flags, const char *s)
{
        static const BVOption cpuflags_opts[] = {
        { "flags"   , NULL, 0, BV_OPT_TYPE_FLAGS, { .i64 = 0 }, INT64_MIN, INT64_MAX, .unit = "flags" },
#if   ARCH_PPC
        { "altivec" , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ALTIVEC  },    .unit = "flags" },
#elif ARCH_X86
        { "mmx"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_MMX      },    .unit = "flags" },
        { "mmx2"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_MMX2     },    .unit = "flags" },
        { "mmxext"  , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_MMX2     },    .unit = "flags" },
        { "sse"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSE      },    .unit = "flags" },
        { "sse2"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSE2     },    .unit = "flags" },
        { "sse2slow", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSE2SLOW },    .unit = "flags" },
        { "sse3"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSE3     },    .unit = "flags" },
        { "sse3slow", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSE3SLOW },    .unit = "flags" },
        { "ssse3"   , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSSE3    },    .unit = "flags" },
        { "atom"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ATOM     },    .unit = "flags" },
        { "sse4.1"  , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSE4     },    .unit = "flags" },
        { "sse4.2"  , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SSE42    },    .unit = "flags" },
        { "avx"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_AVX      },    .unit = "flags" },
        { "xop"     , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_XOP      },    .unit = "flags" },
        { "fma3"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_FMA3     },    .unit = "flags" },
        { "fma4"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_FMA4     },    .unit = "flags" },
        { "avx2"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_AVX2     },    .unit = "flags" },
        { "bmi1"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_BMI1     },    .unit = "flags" },
        { "bmi2"    , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_BMI2     },    .unit = "flags" },
        { "3dnow"   , NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_3DNOW    },    .unit = "flags" },
        { "3dnowext", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_3DNOWEXT },    .unit = "flags" },
        { "cmov",     NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_CMOV     },    .unit = "flags" },

#define CPU_FLAG_P2 BV_CPU_FLAG_CMOV | BV_CPU_FLAG_MMX
#define CPU_FLAG_P3 CPU_FLAG_P2 | BV_CPU_FLAG_MMX2 | BV_CPU_FLAG_SSE
#define CPU_FLAG_P4 CPU_FLAG_P3| BV_CPU_FLAG_SSE2
        { "pentium2", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPU_FLAG_P2          },    .unit = "flags" },
        { "pentium3", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPU_FLAG_P3          },    .unit = "flags" },
        { "pentium4", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPU_FLAG_P4          },    .unit = "flags" },

#define CPU_FLAG_K62 BV_CPU_FLAG_MMX | BV_CPU_FLAG_3DNOW
#define CPU_FLAG_ATHLON   CPU_FLAG_K62 | BV_CPU_FLAG_CMOV | BV_CPU_FLAG_3DNOWEXT | BV_CPU_FLAG_MMX2
#define CPU_FLAG_ATHLONXP CPU_FLAG_ATHLON | BV_CPU_FLAG_SSE
#define CPU_FLAG_K8  CPU_FLAG_ATHLONXP | BV_CPU_FLAG_SSE2
        { "k6",       NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_MMX      },    .unit = "flags" },
        { "k62",      NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPU_FLAG_K62         },    .unit = "flags" },
        { "athlon",   NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPU_FLAG_ATHLON      },    .unit = "flags" },
        { "athlonxp", NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPU_FLAG_ATHLONXP    },    .unit = "flags" },
        { "k8",       NULL, 0, BV_OPT_TYPE_CONST, { .i64 = CPU_FLAG_K8          },    .unit = "flags" },
#elif ARCH_ARM
        { "armv5te",  NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV5TE  },    .unit = "flags" },
        { "armv6",    NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV6    },    .unit = "flags" },
        { "armv6t2",  NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV6T2  },    .unit = "flags" },
        { "vfp",      NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_VFP      },    .unit = "flags" },
        { "vfpv3",    NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_VFPV3    },    .unit = "flags" },
        { "neon",     NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_NEON     },    .unit = "flags" },
        { "setend",   NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_SETEND   },    .unit = "flags" },
#elif ARCH_AARCH64
        { "armv8",    NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_ARMV8    },    .unit = "flags" },
        { "neon",     NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_NEON     },    .unit = "flags" },
        { "vfp",      NULL, 0, BV_OPT_TYPE_CONST, { .i64 = BV_CPU_FLAG_VFP      },    .unit = "flags" },
#endif
        { NULL },
    };
    static const BVClass class = {
        .class_name = "cpuflags",
        .item_name  = bv_default_item_name,
        .option     = cpuflags_opts,
        .version    = LIBBVUTIL_VERSION_INT,
    };
    const BVClass *pclass = &class;

    return bv_opt_eval_flags(&pclass, &cpuflags_opts[0], s, flags);
}

int bv_cpu_count(void)
{
    static volatile int printed;

    int nb_cpus = 1;
#if HAVE_SCHED_GETAFFINITY && defined(CPU_COUNT)
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);

    if (!sched_getaffinity(0, sizeof(cpuset), &cpuset))
        nb_cpus = CPU_COUNT(&cpuset);
#elif HAVE_GETPROCESSAFFINITYMASK
    DWORD_PTR proc_aff, sys_aff;
    if (GetProcessAffinityMask(GetCurrentProcess(), &proc_aff, &sys_aff))
        nb_cpus = bv_popcount64(proc_aff);
#elif HAVE_SYSCTL && defined(HW_NCPU)
    int mib[2] = { CTL_HW, HW_NCPU };
    size_t len = sizeof(nb_cpus);

    if (sysctl(mib, 2, &nb_cpus, &len, NULL, 0) == -1)
        nb_cpus = 0;
#elif HAVE_SYSCONF && defined(_SC_NPROC_ONLN)
    nb_cpus = sysconf(_SC_NPROC_ONLN);
#elif HAVE_SYSCONF && defined(_SC_NPROCESSORS_ONLN)
    nb_cpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif

    if (!printed) {
        bv_log(NULL, BV_LOG_DEBUG, "detected %d logical cores\n", nb_cpus);
        printed = 1;
    }

    return nb_cpus;
}

#ifdef TEST

#include <stdio.h>
#include "bvstring.h"

#if !HAVE_GETOPT
#include "compat/getopt.c"
#endif

static const struct {
    int flag;
    const char *name;
} cpu_flag_tab[] = {
#if   ARCH_AARCH64
    { BV_CPU_FLAG_ARMV8,     "armv8"      },
    { BV_CPU_FLAG_NEON,      "neon"       },
    { BV_CPU_FLAG_VFP,       "vfp"        },
#elif ARCH_ARM
    { BV_CPU_FLAG_ARMV5TE,   "armv5te"    },
    { BV_CPU_FLAG_ARMV6,     "armv6"      },
    { BV_CPU_FLAG_ARMV6T2,   "armv6t2"    },
    { BV_CPU_FLAG_VFP,       "vfp"        },
    { BV_CPU_FLAG_VFPV3,     "vfpv3"      },
    { BV_CPU_FLAG_NEON,      "neon"       },
    { BV_CPU_FLAG_SETEND,    "setend"     },
#elif ARCH_PPC
    { BV_CPU_FLAG_ALTIVEC,   "altivec"    },
#elif ARCH_X86
    { BV_CPU_FLAG_MMX,       "mmx"        },
    { BV_CPU_FLAG_MMXEXT,    "mmxext"     },
    { BV_CPU_FLAG_SSE,       "sse"        },
    { BV_CPU_FLAG_SSE2,      "sse2"       },
    { BV_CPU_FLAG_SSE2SLOW,  "sse2slow"   },
    { BV_CPU_FLAG_SSE3,      "sse3"       },
    { BV_CPU_FLAG_SSE3SLOW,  "sse3slow"   },
    { BV_CPU_FLAG_SSSE3,     "ssse3"      },
    { BV_CPU_FLAG_ATOM,      "atom"       },
    { BV_CPU_FLAG_SSE4,      "sse4.1"     },
    { BV_CPU_FLAG_SSE42,     "sse4.2"     },
    { BV_CPU_FLAG_AVX,       "avx"        },
    { BV_CPU_FLAG_XOP,       "xop"        },
    { BV_CPU_FLAG_FMA3,      "fma3"       },
    { BV_CPU_FLAG_FMA4,      "fma4"       },
    { BV_CPU_FLAG_3DNOW,     "3dnow"      },
    { BV_CPU_FLAG_3DNOWEXT,  "3dnowext"   },
    { BV_CPU_FLAG_CMOV,      "cmov"       },
    { BV_CPU_FLAG_AVX2,      "avx2"       },
    { BV_CPU_FLAG_BMI1,      "bmi1"       },
    { BV_CPU_FLAG_BMI2,      "bmi2"       },
#endif
    { 0 }
};

static void print_cpu_flags(int cpu_flags, const char *type)
{
    int i;

    printf("cpu_flags(%s) = 0x%08X\n", type, cpu_flags);
    printf("cpu_flags_str(%s) =", type);
    for (i = 0; cpu_flag_tab[i].flag; i++)
        if (cpu_flags & cpu_flag_tab[i].flag)
            printf(" %s", cpu_flag_tab[i].name);
    printf("\n");
}


int main(int argc, char **argv)
{
    int cpu_flags_raw = bv_get_cpu_flags();
    int cpu_flags_eff;
    int cpu_count = bv_cpu_count();
    char threads[5] = "auto";
    int i;

    for(i = 0; cpu_flag_tab[i].flag; i++) {
        unsigned tmp = 0;
        if (bv_parse_cpu_caps(&tmp, cpu_flag_tab[i].name) < 0) {
            fprintf(stderr, "Table missing %s\n", cpu_flag_tab[i].name);
            return 4;
        }
    }

    if (cpu_flags_raw < 0)
        return 1;

    for (;;) {
        int c = getopt(argc, argv, "c:t:");
        if (c == -1)
            break;
        switch (c) {
        case 'c':
        {
            unsigned flags = bv_get_cpu_flags();
            if (bv_parse_cpu_caps(&flags, optarg) < 0)
                return 2;

            bv_force_cpu_flags(flags);
            break;
        }
        case 't':
        {
            int len = bv_strlcpy(threads, optarg, sizeof(threads));
            if (len >= sizeof(threads)) {
                fprintf(stderr, "Invalid thread count '%s'\n", optarg);
                return 2;
            }
        }
        }
    }

    cpu_flags_eff = bv_get_cpu_flags();

    if (cpu_flags_eff < 0)
        return 3;

    print_cpu_flags(cpu_flags_raw, "raw");
    print_cpu_flags(cpu_flags_eff, "effective");
    printf("threads = %s (cpu_count = %d)\n", threads, cpu_count);

    return 0;
}

#endif
