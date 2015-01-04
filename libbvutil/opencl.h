/*
 * Copyright (C) 2012 Peng  Gao     <peng@multicorewareinc.com>
 * Copyright (C) 2012 Li    Cao     <li@multicorewareinc.com>
 * Copyright (C) 2012 Wei   Gao     <weigao@multicorewareinc.com>
 * Copyright (C) 2013 Lenny Wang    <lwanghpc@gmail.com>
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
 * OpenCL wrapper
 *
 * This interface is considered still experimental and its API and ABI may
 * change without prior notice.
 */

#ifndef LIBBVUTIL_OPENCL_H
#define LIBBVUTIL_OPENCL_H

#include "config.h"
#if BV_HAVE_CL_CL_H
#include <CL/cl.h>
#else
#include <OpenCL/cl.h>
#endif
#include <stdint.h>
#include "dict.h"

#include "libbvutil/version.h"

#define BV_OPENCL_KERNEL( ... )# __VA_ARGS__

#define BV_OPENCL_MAX_KERNEL_NAME_SIZE 150

#define BV_OPENCL_MAX_DEVICE_NAME_SIZE 100

#define BV_OPENCL_MAX_PLATFORM_NAME_SIZE 100

typedef struct {
    int device_type;
    char device_name[BV_OPENCL_MAX_DEVICE_NAME_SIZE];
    cl_device_id device_id;
} BVOpenCLDeviceNode;

typedef struct {
    cl_platform_id platform_id;
    char platform_name[BV_OPENCL_MAX_PLATFORM_NAME_SIZE];
    int device_num;
    BVOpenCLDeviceNode **device_node;
} BVOpenCLPlatformNode;

typedef struct {
    int platform_num;
    BVOpenCLPlatformNode **platform_node;
} BVOpenCLDeviceList;

typedef struct {
    cl_platform_id platform_id;
    cl_device_type device_type;
    cl_context context;
    cl_device_id  device_id;
    cl_command_queue command_queue;
    char *platform_name;
} BVOpenCLExternalEnv;

/**
 * Get OpenCL device list.
 *
 * It must be freed with bv_opencl_free_device_list().
 *
 * @param device_list pointer to OpenCL environment device list,
 *                    should be released by bv_opencl_free_device_list()
 *
 * @return  >=0 on success, a negative error code in case of failure
 */
int bv_opencl_get_device_list(BVOpenCLDeviceList **device_list);

/**
  * Free OpenCL device list.
  *
  * @param device_list pointer to OpenCL environment device list
  *                       created by bv_opencl_get_device_list()
  */
void bv_opencl_free_device_list(BVOpenCLDeviceList **device_list);

/**
 * Set option in the global OpenCL context.
 *
 * This options affect the operation performed by the next
 * bv_opencl_init() operation.
 *
 * The currently accepted options are:
 * - platform: set index of platform in device list
 * - device: set index of device in device list
 *
 * See reference "OpenCL Specification Version: 1.2 chapter 5.6.4".
 *
 * @param key                 option key
 * @param val                 option value
 * @return >=0 on success, a negative error code in case of failure
 * @see bv_opencl_get_option()
 */
int bv_opencl_set_option(const char *key, const char *val);

/**
 * Get option value from the global OpenCL context.
 *
 * @param key        option key
 * @param out_val  pointer to location where option value will be
 *                         written, must be freed with bv_freep()
 * @return  >=0 on success, a negative error code in case of failure
 * @see bv_opencl_set_option()
 */
int bv_opencl_get_option(const char *key, uint8_t **out_val);

/**
 * Free option values of the global OpenCL context.
 *
 */
void bv_opencl_free_option(void);

/**
 * Allocate OpenCL external environment.
 *
 * It must be freed with bv_opencl_free_external_env().
 *
 * @return pointer to allocated OpenCL external environment
 */
BVOpenCLExternalEnv *bv_opencl_alloc_external_env(void);

/**
 * Free OpenCL external environment.
 *
 * @param ext_opencl_env pointer to OpenCL external environment
 *                       created by bv_opencl_alloc_external_env()
 */
void bv_opencl_free_external_env(BVOpenCLExternalEnv **ext_opencl_env);

/**
 * Get OpenCL error string.
 *
 * @param status    OpenCL error code
 * @return OpenCL error string
 */
const char *bv_opencl_errstr(cl_int status);

/**
 * Register kernel code.
 *
 *  The registered kernel code is stored in a global context, and compiled
 *  in the runtime environment when bv_opencl_init() is called.
 *
 * @param kernel_code    kernel code to be compiled in the OpenCL runtime environment
 * @return  >=0 on success, a negative error code in case of failure
 */
int bv_opencl_register_kernel_code(const char *kernel_code);

/**
 * Initialize the run time OpenCL environment
 *
 * @param ext_opencl_env external OpenCL environment, created by an
 *                       application program, ignored if set to NULL
 * @return >=0 on success, a negative error code in case of failure
 */
int bv_opencl_init(BVOpenCLExternalEnv *ext_opencl_env);

/**
 * compile specific OpenCL kernel source
 *
 * @param program_name  pointer to a program name used for identification
 * @param build_opts    pointer to a string that describes the preprocessor
 *                      build options to be used for building the program
 * @return a cl_program object
 */
cl_program bv_opencl_compile(const char *program_name, const char* build_opts);

/**
 * get OpenCL command queue
 *
 * @return a cl_command_queue object
 */
cl_command_queue bv_opencl_get_command_queue(void);

/**
 * Create OpenCL buffer.
 *
 * The buffer is used to save the data used or created by an OpenCL
 * kernel.
 * The created buffer must be released with bv_opencl_buffer_release().
 *
 * See clCreateBuffer() function reference for more information about
 * the parameters.
 *
 * @param cl_buf       pointer to OpenCL buffer
 * @param cl_buf_size  size in bytes of the OpenCL buffer to create
 * @param flags        flags used to control buffer attributes
 * @param host_ptr     host pointer of the OpenCL buffer
 * @return >=0 on success, a negative error code in case of failure
 */
int bv_opencl_buffer_create(cl_mem *cl_buf, size_t cl_buf_size, int flags, void *host_ptr);

/**
 * Write OpenCL buffer with data from src_buf.
 *
 * @param dst_cl_buf        pointer to OpenCL destination buffer
 * @param src_buf           pointer to source buffer
 * @param buf_size          size in bytes of the source and destination buffers
 * @return >=0 on success, a negative error code in case of failure
 */
int bv_opencl_buffer_write(cl_mem dst_cl_buf, uint8_t *src_buf, size_t buf_size);

/**
 * Read data from OpenCL buffer to memory buffer.
 *
 * @param dst_buf           pointer to destination buffer (CPU memory)
 * @param src_cl_buf        pointer to source OpenCL buffer
 * @param buf_size          size in bytes of the source and destination buffers
 * @return >=0 on success, a negative error code in case of failure
 */
int bv_opencl_buffer_read(uint8_t *dst_buf, cl_mem src_cl_buf, size_t buf_size);

/**
 * Write image data from memory to OpenCL buffer.
 *
 * The source must be an array of pointers to image plane buffers.
 *
 * @param dst_cl_buf         pointer to destination OpenCL buffer
 * @param dst_cl_buf_size    size in bytes of OpenCL buffer
 * @param dst_cl_buf_offset  the offset of the OpenCL buffer start position
 * @param src_data           array of pointers to source plane buffers
 * @param src_plane_sizes    array of sizes in bytes of the source plane buffers
 * @param src_plane_num      number of source image planes
 * @return >=0 on success, a negative error code in case of failure
 */
int bv_opencl_buffer_write_image(cl_mem dst_cl_buf, size_t cl_buffer_size, int dst_cl_offset,
                                 uint8_t **src_data, int *plane_size, int plane_num);

/**
 * Read image data from OpenCL buffer.
 *
 * @param dst_data           array of pointers to destination plane buffers
 * @param dst_plane_sizes    array of pointers to destination plane buffers
 * @param dst_plane_num      number of destination image planes
 * @param src_cl_buf         pointer to source OpenCL buffer
 * @param src_cl_buf_size    size in bytes of OpenCL buffer
 * @return >=0 on success, a negative error code in case of failure
 */
int bv_opencl_buffer_read_image(uint8_t **dst_data, int *plane_size, int plane_num,
                                cl_mem src_cl_buf, size_t cl_buffer_size);

/**
 * Release OpenCL buffer.
 *
 * @param cl_buf pointer to OpenCL buffer to release, which was
 *               previously filled with bv_opencl_buffer_create()
 */
void bv_opencl_buffer_release(cl_mem *cl_buf);

/**
 * Release OpenCL environment.
 *
 * The OpenCL environment is effectively released only if all the created
 * kernels had been released with bv_opencl_release_kernel().
 */
void bv_opencl_uninit(void);

/**
 * Benchmark an OpenCL device with a user defined callback function.  This function
 * sets up an external OpenCL environment including context and command queue on
 * the device then tears it down in the end.  The callback function should perform
 * the rest of the work.
 *
 * @param device            pointer to the OpenCL device to be used
 * @param platform          cl_platform_id handle to which the device belongs to
 * @param benchmark         callback function to perform the benchmark, return a
 *                          negative value in case of failure
 * @return the score passed from the callback function, a negative error code in case
 * of failure
 */
int64_t bv_opencl_benchmark(BVOpenCLDeviceNode *device, cl_platform_id platform,
                            int64_t (*benchmark)(BVOpenCLExternalEnv *ext_opencl_env));

#endif /* LIBBVUTIL_OPENCL_H */
