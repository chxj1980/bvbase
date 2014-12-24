/*
 *===================================================================
 *
 *          Name: bv_print.h
 *        Create: Sat 27 Oct 2012 09:49:46 AM CST
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===================================================================
 */
#ifndef __BV_PRINT_H
#define __BV_PRINT_H

#ifdef  __cplusplus 
#define BV_EXTERN_C_START       extern "C" {
#define BV_EXTERN_C_END         }
#define BV_EXTERN_C_FUNC        extern "C"
#else
#define BV_EXTERN_C_START
#define BV_EXTERN_C_END 
#define BV_EXTERN_C_FUNC
#endif

#ifndef UNUSE_ZLOG
#include "zlog.h"
#endif

/* use for bv_print_level */
enum {
    BV_PRINT_NONE = 1, BV_PRINT_ERROR = 1 << 1,
    BV_PRINT_WARN = 1 << 2, BV_PRINT_INFO = 1 << 3,
    BV_PRINT_MESSAGE = 1 << 4,
    BV_PRINT_DEBUG = 1 << 5,
    BV_PRINT_CYCLE = 1 << 6,
    BV_PRINT_ALL = BV_PRINT_ERROR|BV_PRINT_WARN|BV_PRINT_INFO|BV_PRINT_MESSAGE|BV_PRINT_DEBUG|BV_PRINT_CYCLE,
};

/* use for bv_print_style */
// you can use some of them except none and all
// none use above BV_PRINT_NONE
// time style %04y:%02m:%02d
enum {
    BV_PRINT_STYLE_ALL = 1 << 1, // [name][time][func():line@file] ==> 
    BV_PRINT_STYLE_NONE = 1 << 2,  // xxx
    BV_PRINT_NONAME = 1 << 3, // [time]...
    BV_PRINT_NOTIME = 1 << 4, // [name][func... 
    BV_PRINT_NOFUNC = 1 << 5, // [name][time][line@file] ==>
    BV_PRINT_NOLINE = 1 << 6, // [name][time][func()@file] ==>
    BV_PRINT_NOFILE = 1 << 7, // [name][time][func():line] ==>
};

enum {
    BV_PRINT_LOG_TYPE_NONE = -1,
    BV_PRINT_LOG_TYPE_CONSOLE = 0,
    BV_PRINT_LOG_TYPE_FILE = 1,
};

enum {
    BV_COLOR_FALSE = 0,
    BV_COLOR_TRUE,
};
 
BV_EXTERN_C_FUNC int bv_print_init(int bv_print_level, 
          int bv_print_style, int bv_print_enable_color, 
          const char *name);
BV_EXTERN_C_FUNC int bv_print_deinit();

// @type: 0 console, -1 none (not print), 1 to file (depends on /etc/zlog.conf)
// BV_PRINT_LOG_TYPE_*
BV_EXTERN_C_FUNC int bv_print_set_save_type(int type);
BV_EXTERN_C_FUNC int bv_print_get_log_type();
BV_EXTERN_C_FUNC int bv_print_init_file_conf(const char *filepath, const char *processName);
// Please set the config file path and the process name if 
// you want set log to file
// @conf: it is config file, and it tell program how to write log and where.
#define BVLOG_INIT(conf, name) bv_print_init_file_conf(conf, name);

BV_EXTERN_C_FUNC int bv_print_change_level(int level);
BV_EXTERN_C_FUNC int bv_print_start_debug();
BV_EXTERN_C_FUNC int bv_print_stop_debug();
BV_EXTERN_C_FUNC int bv_print_start_cycle();
BV_EXTERN_C_FUNC int bv_print_stop_cycle();

// @func: print log with diff type
// please use the micro define.
BV_EXTERN_C_FUNC int bv_print_info(const char *func, int line, const char *file,
                     const char *format, ...);
BV_EXTERN_C_FUNC int bv_print_warn(const char *func, int line, const char *file,
                     const char *format, ...);
BV_EXTERN_C_FUNC int bv_print_message(const char *func, int line, const char *file,
                     const char *format, ...);
BV_EXTERN_C_FUNC int bv_print_error(const char *func, int line, const char *file,
                     const char *format, ...);
BV_EXTERN_C_FUNC int bv_print_debug(const char *func, int line, const char *file,
                     const char *format, ...);
BV_EXTERN_C_FUNC int bv_print_cycle(const char *func, int line, const char *file,
                     const char *format, ...);
BV_EXTERN_C_FUNC int bv_print_none(const char *func, int line, const char *file,
                     const char *format, ...);
BV_EXTERN_C_FUNC void bv_print_error_string(int errno);

// You may control print level with kind,
// add kind, the bvkind will print out
// del kind, the bvkind will print nothing
BV_EXTERN_C_FUNC int bv_print_add_kind(int kind);
BV_EXTERN_C_FUNC int bv_print_del_kind(int kind);
BV_EXTERN_C_FUNC int bv_print_kind(int kind, const char *func, int line, const char *file, const char *format, ...);
#define bvkind(kind, format, ...) bv_print_kind(kind, __func__, __LINE__, __FILE__, format, ##__VA_ARGS__);

#define bvdzlog_error(format, ...) 
#define bvdzlog_info(format, ...)
#define bvdzlog_warn(format, ...)
#define bvdzlog_debug(format, ...)
#define bvdzlog_notice(format, ...)
#ifndef UNUSE_ZLOG
#define bvdzlog_error(format, ...) dzlog_error(format, ##__VA_ARGS__)
#define bvdzlog_info(format, ...) dzlog_info(format, ##__VA_ARGS__)
#define bvdzlog_warn(format, ...) dzlog_warn(format, ##__VA_ARGS__)
#define bvdzlog_debug(format, ...) dzlog_debug(format, ##__VA_ARGS__)
#define bvdzlog_notice(format, ...) dzlog_notice(format, ##__VA_ARGS__)
#endif

// Caller please call these functions below

#define bverror(format, ...)    \
        switch (bv_print_get_log_type()) {    \
            case BV_PRINT_LOG_TYPE_FILE:    \
               bvdzlog_error(format, ##__VA_ARGS__);    \
            break;  \
            case BV_PRINT_LOG_TYPE_CONSOLE: bv_print_error(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define bvinfo(format, ...)    \
        switch (bv_print_get_log_type()) {    \
            case BV_PRINT_LOG_TYPE_FILE:    \
               bvdzlog_info(format, ##__VA_ARGS__);    \
            break;  \
            case BV_PRINT_LOG_TYPE_CONSOLE: bv_print_info(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define bvwarn(format, ...)    \
        switch (bv_print_get_log_type()) {    \
            case BV_PRINT_LOG_TYPE_FILE:    \
               bvdzlog_warn(format, ##__VA_ARGS__);    \
            break;  \
            case BV_PRINT_LOG_TYPE_CONSOLE: bv_print_warn(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define bvmsg(format, ...)    \
        switch (bv_print_get_log_type()) {    \
            case BV_PRINT_LOG_TYPE_FILE:    \
               bvdzlog_notice(format, ##__VA_ARGS__);    \
            break;  \
            case BV_PRINT_LOG_TYPE_CONSOLE: bv_print_message(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n"); break; \
            default: break;  \
        };   

#define bvdebug(format, ...)    \
        switch (bv_print_get_log_type()) {    \
            case BV_PRINT_LOG_TYPE_FILE:    \
               bvdzlog_debug(format, ##__VA_ARGS__);    \
            break;  \
            case BV_PRINT_LOG_TYPE_CONSOLE: bv_print_debug(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n");break; \
            default: break;  \
        };   

// bvcycle and bvnone force print to console.
#define bvcycle(format, ...)    \
        bv_print_cycle(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n");

#define bvnone(format, ...)    \
        bv_print_style_none(__func__, __LINE__, __FILE__, format, ##__VA_ARGS__); printf("\n");


// It will print out the meanful string with the number
// force to console
#define bverrno(no)   bv_print_error_string(no);

#define BVVERSION(fmt, args...)                                                 \
    do{	printf("\033[01;31;31m");                                              \
	printf(fmt, ##args);                                                   \
	printf(" Build time: %s, %s.\n\033[0m", __DATE__, __TIME__);           \
    }while(0)

#endif

/*=============== End of file: bv_print.h =====================*/
