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

#include "opencl.h"
#include "bvstring.h"
#include "log.h"
#include "bvassert.h"
#include "opt.h"

#if BV_HAVE_THREADS
#include "thread.h"
#include "atomic.h"

static volatile BVMutex *atomic_opencl_lock = NULL;
#define LOCK_OPENCL bv_mutex_lock(atomic_opencl_lock)
#define UNLOCK_OPENCL bv_mutex_unlock(atomic_opencl_lock)
#else
#define LOCK_OPENCL
#define UNLOCK_OPENCL
#endif

#define MAX_KERNEL_CODE_NUM 200

typedef struct {
    int is_compiled;
    const char *kernel_string;
} KernelCode;

typedef struct {
    const BVClass *class;
    int log_offset;
    void *log_ctx;
    int init_count;
    int opt_init_flag;
     /**
     * if set to 1, the OpenCL environment was created by the user and
     * passed as BVOpenCLExternalEnv when initing ,0:created by opencl wrapper.
     */
    int is_user_created;
    int platform_idx;
    int device_idx;
    cl_platform_id platform_id;
    cl_device_type device_type;
    cl_context context;
    cl_device_id device_id;
    cl_command_queue command_queue;
    int kernel_code_count;
    KernelCode kernel_code[MAX_KERNEL_CODE_NUM];
    BVOpenCLDeviceList device_list;
} OpenclContext;

#define OFFSET(x) offsetof(OpenclContext, x)

static const BVOption opencl_options[] = {
     { "platform_idx",        "set platform index value",  OFFSET(platform_idx),  BV_OPT_TYPE_INT,    {.i64=-1}, -1, INT_MAX},
     { "device_idx",          "set device index value",    OFFSET(device_idx),    BV_OPT_TYPE_INT,    {.i64=-1}, -1, INT_MAX},
     { NULL }
};

static const BVClass openclutils_class = {
    .class_name                = "OPENCLUTILS",
    .option                    = opencl_options,
    .item_name                 = bv_default_item_name,
    .version                   = LIBBVUTIL_VERSION_INT,
    .log_level_offset_offset   = offsetof(OpenclContext, log_offset),
    .parent_log_context_offset = offsetof(OpenclContext, log_ctx),
};

static OpenclContext opencl_ctx = {&openclutils_class};

static const cl_device_type device_type[] = {CL_DEVICE_TYPE_GPU, CL_DEVICE_TYPE_CPU};

typedef struct {
    int err_code;
    const char *err_str;
} OpenclErrorMsg;

static const OpenclErrorMsg opencl_err_msg[] = {
    {CL_DEVICE_NOT_FOUND,                               "DEVICE NOT FOUND"},
    {CL_DEVICE_NOT_BVAILABLE,                           "DEVICE NOT BVAILABLE"},
    {CL_COMPILER_NOT_BVAILABLE,                         "COMPILER NOT BVAILABLE"},
    {CL_MEM_OBJECT_ALLOCATION_FAILURE,                  "MEM OBJECT ALLOCATION FAILURE"},
    {CL_OUT_OF_RESOURCES,                               "OUT OF RESOURCES"},
    {CL_OUT_OF_HOST_MEMORY,                             "OUT OF HOST MEMORY"},
    {CL_PROFILING_INFO_NOT_BVAILABLE,                   "PROFILING INFO NOT BVAILABLE"},
    {CL_MEM_COPY_OVERLAP,                               "MEM COPY OVERLAP"},
    {CL_IMAGE_FORMAT_MISMATCH,                          "IMAGE FORMAT MISMATCH"},
    {CL_IMAGE_FORMAT_NOT_SUPPORTED,                     "IMAGE FORMAT NOT_SUPPORTED"},
    {CL_BUILD_PROGRAM_FAILURE,                          "BUILD PROGRAM FAILURE"},
    {CL_MAP_FAILURE,                                    "MAP FAILURE"},
    {CL_MISALIGNED_SUB_BUFFER_OFFSET,                   "MISALIGNED SUB BUFFER OFFSET"},
    {CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST,      "EXEC STATUS ERROR FOR EVENTS IN WAIT LIST"},
    {CL_COMPILE_PROGRAM_FAILURE,                        "COMPILE PROGRAM FAILURE"},
    {CL_LINKER_NOT_BVAILABLE,                           "LINKER NOT BVAILABLE"},
    {CL_LINK_PROGRAM_FAILURE,                           "LINK PROGRAM FAILURE"},
    {CL_DEVICE_PARTITION_FAILED,                        "DEVICE PARTITION FAILED"},
    {CL_KERNEL_ARG_INFO_NOT_BVAILABLE,                  "KERNEL ARG INFO NOT BVAILABLE"},
    {CL_INVALID_VALUE,                                  "INVALID VALUE"},
    {CL_INVALID_DEVICE_TYPE,                            "INVALID DEVICE TYPE"},
    {CL_INVALID_PLATFORM,                               "INVALID PLATFORM"},
    {CL_INVALID_DEVICE,                                 "INVALID DEVICE"},
    {CL_INVALID_CONTEXT,                                "INVALID CONTEXT"},
    {CL_INVALID_QUEUE_PROPERTIES,                       "INVALID QUEUE PROPERTIES"},
    {CL_INVALID_COMMAND_QUEUE,                          "INVALID COMMAND QUEUE"},
    {CL_INVALID_HOST_PTR,                               "INVALID HOST PTR"},
    {CL_INVALID_MEM_OBJECT,                             "INVALID MEM OBJECT"},
    {CL_INVALID_IMAGE_FORMAT_DESCRIPTOR,                "INVALID IMAGE FORMAT DESCRIPTOR"},
    {CL_INVALID_IMAGE_SIZE,                             "INVALID IMAGE SIZE"},
    {CL_INVALID_SAMPLER,                                "INVALID SAMPLER"},
    {CL_INVALID_BINARY,                                 "INVALID BINARY"},
    {CL_INVALID_BUILD_OPTIONS,                          "INVALID BUILD OPTIONS"},
    {CL_INVALID_PROGRAM,                                "INVALID PROGRAM"},
    {CL_INVALID_PROGRAM_EXECUTABLE,                     "INVALID PROGRAM EXECUTABLE"},
    {CL_INVALID_KERNEL_NAME,                            "INVALID KERNEL NAME"},
    {CL_INVALID_KERNEL_DEFINITION,                      "INVALID KERNEL DEFINITION"},
    {CL_INVALID_KERNEL,                                 "INVALID KERNEL"},
    {CL_INVALID_ARG_INDEX,                              "INVALID ARG INDEX"},
    {CL_INVALID_ARG_VALUE,                              "INVALID ARG VALUE"},
    {CL_INVALID_ARG_SIZE,                               "INVALID ARG_SIZE"},
    {CL_INVALID_KERNEL_ARGS,                            "INVALID KERNEL ARGS"},
    {CL_INVALID_WORK_DIMENSION,                         "INVALID WORK DIMENSION"},
    {CL_INVALID_WORK_GROUP_SIZE,                        "INVALID WORK GROUP SIZE"},
    {CL_INVALID_WORK_ITEM_SIZE,                         "INVALID WORK ITEM SIZE"},
    {CL_INVALID_GLOBAL_OFFSET,                          "INVALID GLOBAL OFFSET"},
    {CL_INVALID_EVENT_WAIT_LIST,                        "INVALID EVENT WAIT LIST"},
    {CL_INVALID_EVENT,                                  "INVALID EVENT"},
    {CL_INVALID_OPERATION,                              "INVALID OPERATION"},
    {CL_INVALID_GL_OBJECT,                              "INVALID GL OBJECT"},
    {CL_INVALID_BUFFER_SIZE,                            "INVALID BUFFER SIZE"},
    {CL_INVALID_MIP_LEVEL,                              "INVALID MIP LEVEL"},
    {CL_INVALID_GLOBAL_WORK_SIZE,                       "INVALID GLOBAL WORK SIZE"},
    {CL_INVALID_PROPERTY,                               "INVALID PROPERTY"},
    {CL_INVALID_IMAGE_DESCRIPTOR,                       "INVALID IMAGE DESCRIPTOR"},
    {CL_INVALID_COMPILER_OPTIONS,                       "INVALID COMPILER OPTIONS"},
    {CL_INVALID_LINKER_OPTIONS,                         "INVALID LINKER OPTIONS"},
    {CL_INVALID_DEVICE_PARTITION_COUNT,                 "INVALID DEVICE PARTITION COUNT"},
};

const char *bv_opencl_errstr(cl_int status)
{
    int i;
    for (i = 0; i < BV_ARRAY_ELEMS(opencl_err_msg); i++) {
        if (opencl_err_msg[i].err_code == status)
            return opencl_err_msg[i].err_str;
    }
    return "unknown error";
}

static void free_device_list(BVOpenCLDeviceList *device_list)
{
    int i, j;
    if (!device_list)
        return;
    for (i = 0; i < device_list->platform_num; i++) {
        if (!device_list->platform_node[i])
            continue;
        for (j = 0; j < device_list->platform_node[i]->device_num; j++) {
            bv_freep(&(device_list->platform_node[i]->device_node[j]));
        }
        bv_freep(&device_list->platform_node[i]->device_node);
        bv_freep(&device_list->platform_node[i]);
    }
    bv_freep(&device_list->platform_node);
    device_list->platform_num = 0;
}

static int get_device_list(BVOpenCLDeviceList *device_list)
{
    cl_int status;
    int i, j, k, device_num, total_devices_num, ret = 0;
    int *devices_num;
    cl_platform_id *platform_ids = NULL;
    cl_device_id *device_ids = NULL;
    BVOpenCLDeviceNode *device_node = NULL;
    status = clGetPlatformIDs(0, NULL, &device_list->platform_num);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not get OpenCL platform ids: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    platform_ids = bv_mallocz_array(device_list->platform_num, sizeof(cl_platform_id));
    if (!platform_ids)
        return BVERROR(ENOMEM);
    status = clGetPlatformIDs(device_list->platform_num, platform_ids, NULL);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
                "Could not get OpenCL platform ids: %s\n", bv_opencl_errstr(status));
        ret = BVERROR_EXTERNAL;
        goto end;
    }
    device_list->platform_node = bv_mallocz_array(device_list->platform_num, sizeof(BVOpenCLPlatformNode *));
    if (!device_list->platform_node) {
        ret = BVERROR(ENOMEM);
        goto end;
    }
    devices_num = bv_mallocz(sizeof(int) * BV_ARRAY_ELEMS(device_type));
    if (!devices_num) {
        ret = BVERROR(ENOMEM);
        goto end;
    }
    for (i = 0; i < device_list->platform_num; i++) {
        device_list->platform_node[i] = bv_mallocz(sizeof(BVOpenCLPlatformNode));
        if (!device_list->platform_node[i]) {
            ret = BVERROR(ENOMEM);
            goto end;
        }
        device_list->platform_node[i]->platform_id = platform_ids[i];
        status = clGetPlatformInfo(platform_ids[i], CL_PLATFORM_VENDOR,
                                   sizeof(device_list->platform_node[i]->platform_name),
                                   device_list->platform_node[i]->platform_name, NULL);
        total_devices_num = 0;
        for (j = 0; j < BV_ARRAY_ELEMS(device_type); j++) {
            status = clGetDeviceIDs(device_list->platform_node[i]->platform_id,
                                    device_type[j], 0, NULL, &devices_num[j]);
            total_devices_num += devices_num[j];
        }
        device_list->platform_node[i]->device_node = bv_mallocz_array(total_devices_num, sizeof(BVOpenCLDeviceNode *));
        if (!device_list->platform_node[i]->device_node) {
            ret = BVERROR(ENOMEM);
            goto end;
        }
        for (j = 0; j < BV_ARRAY_ELEMS(device_type); j++) {
            if (devices_num[j]) {
                device_ids = bv_mallocz_array(devices_num[j], sizeof(cl_device_id));
                if (!device_ids) {
                    ret = BVERROR(ENOMEM);
                    goto end;
                }
                status = clGetDeviceIDs(device_list->platform_node[i]->platform_id, device_type[j],
                                        devices_num[j], device_ids, NULL);
                if (status != CL_SUCCESS) {
                    bv_log(&opencl_ctx, BV_LOG_WARNING,
                            "Could not get device ID: %s:\n", bv_opencl_errstr(status));
                    bv_freep(&device_ids);
                    continue;
                }
                for (k = 0; k < devices_num[j]; k++) {
                    device_num = device_list->platform_node[i]->device_num;
                    device_list->platform_node[i]->device_node[device_num] = bv_mallocz(sizeof(BVOpenCLDeviceNode));
                    if (!device_list->platform_node[i]->device_node[device_num]) {
                        ret = BVERROR(ENOMEM);
                        goto end;
                    }
                    device_node = device_list->platform_node[i]->device_node[device_num];
                    device_node->device_id = device_ids[k];
                    device_node->device_type = device_type[j];
                    status = clGetDeviceInfo(device_node->device_id, CL_DEVICE_NAME,
                                             sizeof(device_node->device_name), device_node->device_name,
                                             NULL);
                    if (status != CL_SUCCESS) {
                        bv_log(&opencl_ctx, BV_LOG_WARNING,
                                "Could not get device name: %s\n", bv_opencl_errstr(status));
                        continue;
                    }
                    device_list->platform_node[i]->device_num++;
                }
                bv_freep(&device_ids);
            }
        }
    }
end:
    bv_freep(&platform_ids);
    bv_freep(&devices_num);
    bv_freep(&device_ids);
    if (ret < 0)
        free_device_list(device_list);
    return ret;
}

int bv_opencl_get_device_list(BVOpenCLDeviceList **device_list)
{
    int ret = 0;
    *device_list = bv_mallocz(sizeof(BVOpenCLDeviceList));
    if (!(*device_list)) {
        bv_log(&opencl_ctx, BV_LOG_ERROR, "Could not allocate opencl device list\n");
        return BVERROR(ENOMEM);
    }
    ret = get_device_list(*device_list);
    if (ret < 0) {
        bv_log(&opencl_ctx, BV_LOG_ERROR, "Could not get device list from environment\n");
        free_device_list(*device_list);
        bv_freep(device_list);
        return ret;
    }
    return ret;
}

void bv_opencl_free_device_list(BVOpenCLDeviceList **device_list)
{
    free_device_list(*device_list);
    bv_freep(device_list);
}

static inline int init_opencl_mtx(void)
{
#if BV_HAVE_THREADS
    if (!atomic_opencl_lock) {
        int err;
        BVMutex *tmp = bv_malloc(sizeof(BVMutex));
        if (!tmp)
            return BVERROR(ENOMEM);
        if ((err = bv_mutex_init(tmp, NULL))) {
            bv_free(tmp);
            return BVERROR(err);
        }
        if (bvpriv_atomic_ptr_cas(&atomic_opencl_lock, NULL, tmp)) {
            bv_mutex_destroy(tmp);
            bv_free(tmp);
        }
    }
#endif
    return 0;
}

int bv_opencl_set_option(const char *key, const char *val)
{
    int ret = init_opencl_mtx( );
    if (ret < 0)
        return ret;
    LOCK_OPENCL;
    if (!opencl_ctx.opt_init_flag) {
        bv_opt_set_defaults(&opencl_ctx);
        opencl_ctx.opt_init_flag = 1;
    }
    ret = bv_opt_set(&opencl_ctx, key, val, 0);
    UNLOCK_OPENCL;
    return ret;
}

int bv_opencl_get_option(const char *key, uint8_t **out_val)
{
    int ret = 0;
    LOCK_OPENCL;
    ret = bv_opt_get(&opencl_ctx, key, 0, out_val);
    UNLOCK_OPENCL;
    return ret;
}

void bv_opencl_free_option(void)
{
    /*FIXME: free openclutils context*/
    LOCK_OPENCL;
    bv_opt_free(&opencl_ctx);
    UNLOCK_OPENCL;
}

BVOpenCLExternalEnv *bv_opencl_alloc_external_env(void)
{
    BVOpenCLExternalEnv *ext = bv_mallocz(sizeof(BVOpenCLExternalEnv));
    if (!ext) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not malloc external opencl environment data space\n");
    }
    return ext;
}

void bv_opencl_free_external_env(BVOpenCLExternalEnv **ext_opencl_env)
{
    bv_freep(ext_opencl_env);
}

int bv_opencl_register_kernel_code(const char *kernel_code)
{
    int i, ret = init_opencl_mtx( );
    if (ret < 0)
        return ret;
    LOCK_OPENCL;
    if (opencl_ctx.kernel_code_count >= MAX_KERNEL_CODE_NUM) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not register kernel code, maximum number of registered kernel code %d already reached\n",
               MAX_KERNEL_CODE_NUM);
        ret = BVERROR(EINVAL);
        goto end;
    }
    for (i = 0; i < opencl_ctx.kernel_code_count; i++) {
        if (opencl_ctx.kernel_code[i].kernel_string == kernel_code) {
            bv_log(&opencl_ctx, BV_LOG_WARNING, "Same kernel code has been registered\n");
            goto end;
        }
    }
    opencl_ctx.kernel_code[opencl_ctx.kernel_code_count].kernel_string = kernel_code;
    opencl_ctx.kernel_code[opencl_ctx.kernel_code_count].is_compiled = 0;
    opencl_ctx.kernel_code_count++;
end:
    UNLOCK_OPENCL;
    return ret;
}

cl_program bv_opencl_compile(const char *program_name, const char *build_opts)
{
    int i;
    cl_int status;
    int kernel_code_idx = 0;
    const char *kernel_source;
    size_t kernel_code_len;
    char* ptr = NULL;
    cl_program program = NULL;

    LOCK_OPENCL;
    for (i = 0; i < opencl_ctx.kernel_code_count; i++) {
        // identify a program using a unique name within the kernel source
        ptr = bv_stristr(opencl_ctx.kernel_code[i].kernel_string, program_name);
        if (ptr && !opencl_ctx.kernel_code[i].is_compiled) {
            kernel_source = opencl_ctx.kernel_code[i].kernel_string;
            kernel_code_len = strlen(opencl_ctx.kernel_code[i].kernel_string);
            kernel_code_idx = i;
            break;
        }
    }
    if (!kernel_source) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Unable to find OpenCL kernel source '%s'\n", program_name);
        goto end;
    }

    /* create a CL program from kernel source */
    program = clCreateProgramWithSource(opencl_ctx.context, 1, &kernel_source, &kernel_code_len, &status);
    if(status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Unable to create OpenCL program '%s': %s\n", program_name, bv_opencl_errstr(status));
        program = NULL;
        goto end;
    }
    status = clBuildProgram(program, 1, &(opencl_ctx.device_id), build_opts, NULL, NULL);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Compilation failed with OpenCL program: %s\n", program_name);
        program = NULL;
        goto end;
    }

    opencl_ctx.kernel_code[kernel_code_idx].is_compiled = 1;
end:
    UNLOCK_OPENCL;
    return program;
}

cl_command_queue bv_opencl_get_command_queue(void)
{
    return opencl_ctx.command_queue;
}

static int init_opencl_env(OpenclContext *opencl_ctx, BVOpenCLExternalEnv *ext_opencl_env)
{
    cl_int status;
    cl_context_properties cps[3];
    int i, ret = 0;
    BVOpenCLDeviceNode *device_node = NULL;

    if (ext_opencl_env) {
        if (opencl_ctx->is_user_created)
            return 0;
        opencl_ctx->platform_id     = ext_opencl_env->platform_id;
        opencl_ctx->is_user_created = 1;
        opencl_ctx->command_queue   = ext_opencl_env->command_queue;
        opencl_ctx->context         = ext_opencl_env->context;
        opencl_ctx->device_id       = ext_opencl_env->device_id;
        opencl_ctx->device_type     = ext_opencl_env->device_type;
    } else {
        if (!opencl_ctx->is_user_created) {
            if (!opencl_ctx->device_list.platform_num) {
                ret = get_device_list(&opencl_ctx->device_list);
                if (ret < 0) {
                    return ret;
                }
            }
            if (opencl_ctx->platform_idx >= 0) {
                if (opencl_ctx->device_list.platform_num < opencl_ctx->platform_idx + 1) {
                    bv_log(opencl_ctx, BV_LOG_ERROR, "User set platform index not exist\n");
                    return BVERROR(EINVAL);
                }
                if (!opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->device_num) {
                    bv_log(opencl_ctx, BV_LOG_ERROR, "No devices in user specific platform with index %d\n",
                           opencl_ctx->platform_idx);
                    return BVERROR(EINVAL);
                }
                opencl_ctx->platform_id = opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->platform_id;
            } else {
                /* get a usable platform by default*/
                for (i = 0; i < opencl_ctx->device_list.platform_num; i++) {
                    if (opencl_ctx->device_list.platform_node[i]->device_num) {
                        opencl_ctx->platform_id = opencl_ctx->device_list.platform_node[i]->platform_id;
                        opencl_ctx->platform_idx = i;
                        break;
                    }
                }
            }
            if (!opencl_ctx->platform_id) {
                bv_log(opencl_ctx, BV_LOG_ERROR, "Could not get OpenCL platforms\n");
                return BVERROR_EXTERNAL;
            }
            /* get a usable device*/
            if (opencl_ctx->device_idx >= 0) {
                if (opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->device_num < opencl_ctx->device_idx + 1) {
                    bv_log(opencl_ctx, BV_LOG_ERROR,
                           "Could not get OpenCL device idx %d in the user set platform\n", opencl_ctx->platform_idx);
                    return BVERROR(EINVAL);
                }
            } else {
                opencl_ctx->device_idx = 0;
            }

            device_node = opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->device_node[opencl_ctx->device_idx];
            opencl_ctx->device_id = device_node->device_id;
            opencl_ctx->device_type = device_node->device_type;

            /*
             * Use available platform.
             */
            bv_log(opencl_ctx, BV_LOG_VERBOSE, "Platform Name: %s, Device Name: %s\n",
                   opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->platform_name,
                   device_node->device_name);
            cps[0] = CL_CONTEXT_PLATFORM;
            cps[1] = (cl_context_properties)opencl_ctx->platform_id;
            cps[2] = 0;

            opencl_ctx->context = clCreateContextFromType(cps, opencl_ctx->device_type,
                                                       NULL, NULL, &status);
            if (status != CL_SUCCESS) {
                bv_log(opencl_ctx, BV_LOG_ERROR,
                       "Could not get OpenCL context from device type: %s\n", bv_opencl_errstr(status));
                return BVERROR_EXTERNAL;
            }
            opencl_ctx->command_queue = clCreateCommandQueue(opencl_ctx->context, opencl_ctx->device_id,
                                                          0, &status);
            if (status != CL_SUCCESS) {
                bv_log(opencl_ctx, BV_LOG_ERROR,
                       "Could not create OpenCL command queue: %s\n", bv_opencl_errstr(status));
                return BVERROR_EXTERNAL;
            }
        }
    }
    return ret;
}

int bv_opencl_init(BVOpenCLExternalEnv *ext_opencl_env)
{
    int ret = init_opencl_mtx( );
    if (ret < 0)
        return ret;
    LOCK_OPENCL;
    if (!opencl_ctx.init_count) {
        if (!opencl_ctx.opt_init_flag) {
            bv_opt_set_defaults(&opencl_ctx);
            opencl_ctx.opt_init_flag = 1;
        }
        ret = init_opencl_env(&opencl_ctx, ext_opencl_env);
        if (ret < 0)
            goto end;
        if (opencl_ctx.kernel_code_count <= 0) {
            bv_log(&opencl_ctx, BV_LOG_ERROR,
                   "No kernel code is registered, compile kernel file failed\n");
            ret = BVERROR(EINVAL);
            goto end;
        }
    }
    opencl_ctx.init_count++;
end:
    UNLOCK_OPENCL;
    return ret;
}

void bv_opencl_uninit(void)
{
    cl_int status;
    LOCK_OPENCL;
    opencl_ctx.init_count--;
    if (opencl_ctx.is_user_created)
        goto end;
    if (opencl_ctx.init_count > 0)
        goto end;
    if (opencl_ctx.command_queue) {
        status = clReleaseCommandQueue(opencl_ctx.command_queue);
        if (status != CL_SUCCESS) {
            bv_log(&opencl_ctx, BV_LOG_ERROR,
                   "Could not release OpenCL command queue: %s\n", bv_opencl_errstr(status));
        }
        opencl_ctx.command_queue = NULL;
    }
    if (opencl_ctx.context) {
        status = clReleaseContext(opencl_ctx.context);
        if (status != CL_SUCCESS) {
            bv_log(&opencl_ctx, BV_LOG_ERROR,
                   "Could not release OpenCL context: %s\n", bv_opencl_errstr(status));
        }
        opencl_ctx.context = NULL;
    }
    free_device_list(&opencl_ctx.device_list);
end:
    if (opencl_ctx.init_count <= 0)
        bv_opt_free(&opencl_ctx); //FIXME: free openclutils context
    UNLOCK_OPENCL;
}

int bv_opencl_buffer_create(cl_mem *cl_buf, size_t cl_buf_size, int flags, void *host_ptr)
{
    cl_int status;
    *cl_buf = clCreateBuffer(opencl_ctx.context, flags, cl_buf_size, host_ptr, &status);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR, "Could not create OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    return 0;
}

void bv_opencl_buffer_release(cl_mem *cl_buf)
{
    cl_int status = 0;
    if (!cl_buf)
        return;
    status = clReleaseMemObject(*cl_buf);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not release OpenCL buffer: %s\n", bv_opencl_errstr(status));
    }
    memset(cl_buf, 0, sizeof(*cl_buf));
}

int bv_opencl_buffer_write(cl_mem dst_cl_buf, uint8_t *src_buf, size_t buf_size)
{
    cl_int status;
    void *mapped = clEnqueueMapBuffer(opencl_ctx.command_queue, dst_cl_buf,
                                      CL_TRUE, CL_MAP_WRITE, 0, sizeof(uint8_t) * buf_size,
                                      0, NULL, NULL, &status);

    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not map OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    memcpy(mapped, src_buf, buf_size);

    status = clEnqueueUnmapMemObject(opencl_ctx.command_queue, dst_cl_buf, mapped, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not unmap OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    return 0;
}

int bv_opencl_buffer_read(uint8_t *dst_buf, cl_mem src_cl_buf, size_t buf_size)
{
    cl_int status;
    void *mapped = clEnqueueMapBuffer(opencl_ctx.command_queue, src_cl_buf,
                                      CL_TRUE, CL_MAP_READ, 0, buf_size,
                                      0, NULL, NULL, &status);

    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not map OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    memcpy(dst_buf, mapped, buf_size);

    status = clEnqueueUnmapMemObject(opencl_ctx.command_queue, src_cl_buf, mapped, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not unmap OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    return 0;
}

int bv_opencl_buffer_write_image(cl_mem dst_cl_buf, size_t cl_buffer_size, int dst_cl_offset,
                                 uint8_t **src_data, int *plane_size, int plane_num)
{
    int i, buffer_size = 0;
    uint8_t *temp;
    cl_int status;
    void *mapped;
    if ((unsigned int)plane_num > 8) {
        return BVERROR(EINVAL);
    }
    for (i = 0;i < plane_num;i++) {
        buffer_size += plane_size[i];
    }
    if (buffer_size > cl_buffer_size) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Cannot write image to OpenCL buffer: buffer too small\n");
        return BVERROR(EINVAL);
    }
    mapped = clEnqueueMapBuffer(opencl_ctx.command_queue, dst_cl_buf,
                                CL_TRUE, CL_MAP_WRITE, 0, buffer_size + dst_cl_offset,
                                0, NULL, NULL, &status);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not map OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    temp = mapped;
    temp += dst_cl_offset;
    for (i = 0; i < plane_num; i++) {
        memcpy(temp, src_data[i], plane_size[i]);
        temp += plane_size[i];
    }
    status = clEnqueueUnmapMemObject(opencl_ctx.command_queue, dst_cl_buf, mapped, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not unmap OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    return 0;
}

int bv_opencl_buffer_read_image(uint8_t **dst_data, int *plane_size, int plane_num,
                                cl_mem src_cl_buf, size_t cl_buffer_size)
{
    int i,buffer_size = 0,ret = 0;
    uint8_t *temp;
    void *mapped;
    cl_int status;
    if ((unsigned int)plane_num > 8) {
        return BVERROR(EINVAL);
    }
    for (i = 0; i < plane_num; i++) {
        buffer_size += plane_size[i];
    }
    if (buffer_size > cl_buffer_size) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Cannot write image to CPU buffer: OpenCL buffer too small\n");
        return BVERROR(EINVAL);
    }
    mapped = clEnqueueMapBuffer(opencl_ctx.command_queue, src_cl_buf,
                                CL_TRUE, CL_MAP_READ, 0, buffer_size,
                                0, NULL, NULL, &status);

    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not map OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    temp = mapped;
    if (ret >= 0) {
        for (i = 0; i < plane_num; i++) {
            memcpy(dst_data[i], temp, plane_size[i]);
            temp += plane_size[i];
        }
    }
    status = clEnqueueUnmapMemObject(opencl_ctx.command_queue, src_cl_buf, mapped, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        bv_log(&opencl_ctx, BV_LOG_ERROR,
               "Could not unmap OpenCL buffer: %s\n", bv_opencl_errstr(status));
        return BVERROR_EXTERNAL;
    }
    return 0;
}

int64_t bv_opencl_benchmark(BVOpenCLDeviceNode *device_node, cl_platform_id platform,
                            int64_t (*benchmark)(BVOpenCLExternalEnv *ext_opencl_env))
{
    int64_t ret = 0;
    cl_int status;
    cl_context_properties cps[3];
    BVOpenCLExternalEnv *ext_opencl_env = NULL;

    ext_opencl_env = bv_opencl_alloc_external_env();
    ext_opencl_env->device_id = device_node->device_id;
    ext_opencl_env->device_type = device_node->device_type;
    bv_log(&opencl_ctx, BV_LOG_VERBOSE, "Performing test on OpenCL device %s\n",
           device_node->device_name);

    cps[0] = CL_CONTEXT_PLATFORM;
    cps[1] = (cl_context_properties)platform;
    cps[2] = 0;
    ext_opencl_env->context = clCreateContextFromType(cps, ext_opencl_env->device_type,
                                                      NULL, NULL, &status);
    if (status != CL_SUCCESS || !ext_opencl_env->context) {
        ret = BVERROR_EXTERNAL;
        goto end;
    }
    ext_opencl_env->command_queue = clCreateCommandQueue(ext_opencl_env->context,
                                                         ext_opencl_env->device_id, 0, &status);
    if (status != CL_SUCCESS || !ext_opencl_env->command_queue) {
        ret = BVERROR_EXTERNAL;
        goto end;
    }
    ret = benchmark(ext_opencl_env);
    if (ret < 0)
        bv_log(&opencl_ctx, BV_LOG_ERROR, "Benchmark failed with OpenCL device %s\n",
               device_node->device_name);
end:
    if (ext_opencl_env->command_queue)
        clReleaseCommandQueue(ext_opencl_env->command_queue);
    if (ext_opencl_env->context)
        clReleaseContext(ext_opencl_env->context);
    bv_opencl_free_external_env(&ext_opencl_env);
    return ret;
}
