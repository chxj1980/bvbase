/*
 *===========================================================================
 *
 *          Name: bv_print.c
 *        Create: Sun 28 Oct 2012 11:53:44 AM CST
 *
 *   Discription: 
 *       Version: 1.0.0
 *
 *        Author: yuwei.zhang
 *         Email: yuwei.zhang@besovideo.com
 *
 *===========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include "bv_print.h"

#define BV_COLOR_NONE	"\033[0m "
#define BV_COLOR_BLACK	"\033[0;30m "
#define BV_COLOR_DARK_GRAY		"\033[1;30m "
#define BV_COLOR_BLUE			"\033[0;34m "
#define BV_COLOR_LIGHT_BLUE	"\033[1;34m "
#define BV_COLOR_GREEN			"\033[0;32m "
#define BV_COLOR_LIGHT_GREE    "\033[1;32m "
#define BV_COLOR_CYAN			"\033[0;36m "
#define BV_COLOR_LIGHT_CYAN	"\033[1;36m "
#define BV_COLOR_RED			"\033[0;31m "
#define BV_COLOR_LIGHT_RED		"\033[1;31m "
#define BV_COLOR_PURPLE		"\033[0;35m "
#define BV_COLOR_LIGHT_PURPLE	"\033[1;35m "
#define BV_COLOR_BROWN			"\033[0;33m "
#define BV_COLOR_YELLOW		"\033[1;33m "
#define BV_COLOR_LIGHT_GRAY	"\033[0;37m "
#define BV_COLOR_WHITE			"\033[1;37m "


#define BV_NAME_LEN_SUPPORT              64
#define BV_MAX_STRING_LEN                512
#define BV_STRING_MAX_LEN_SUPPORT        3056

#define BVNAME(name)              \
    printf("[%s]", name)

#define BVTIMESIMPLE()                                                             \
  do{                                                                        \
    struct timeval _tNow; struct tm _tmNow;                                  \
    gettimeofday(&_tNow, NULL);                                              \
    memcpy(&_tmNow, localtime(&_tNow.tv_sec), sizeof(_tmNow));               \
    printf("[%02d-%02d][%02d:%02d:%02d]",                         \
        _tmNow.tm_mon+1, _tmNow.tm_mday,                \
        _tmNow.tm_hour, _tmNow.tm_min, _tmNow.tm_sec);   \
  }while(0)

#define BVTIME() BVTIMESIMPLE()

#define BVTIMEMORE()                                            \
  do{                                                                        \
    struct timeval _tNow; struct tm _tmNow;                                  \
    gettimeofday(&_tNow, NULL);                                              \
    memcpy(&_tmNow, localtime(&_tNow.tv_sec), sizeof(_tmNow));               \
    printf("[%04d-%02d-%02d][%02d:%02d:%02d.%03ld]",                         \
        _tmNow.tm_year+1900, _tmNow.tm_mon+1, _tmNow.tm_mday,                \
        _tmNow.tm_hour, _tmNow.tm_min, _tmNow.tm_sec, _tNow.tv_usec/1000);   \
  }while(0)

#define BVFUNC(func)             \
  printf("%s()", func)        

#define BVLINE(line)             \
  printf("%04d", line)        

#define BVFILE(file)             \
  printf("%s", file) 

#define BVLEFT()   printf("[")
#define BVRIGHT()  printf("]")
#define BVCOLON()  printf(":")
#define BVAT()     printf("@")

#define BVALL(name, func, line, file)             \
  BVNAME(name);                 \
  BVTIME();                     \
  BVLEFT();                     \
  BVFUNC(func);                 \
  BVCOLON();                    \
  BVLINE(line);                 \
  BVAT();                       \
  BVFILE(file);                 \
  BVRIGHT()

#define BVERRORTIPS(err)     printf("%s", err)
#define BVWARNTIPS(warn)     printf("%s", warn)
#define BVINFOTIPS(info)     printf("%s", info)
#define BVMESSAGETIPS(message)  printf("%s", message)
#define BVDEBUGTIPS(debug)   printf("%s", debug)
#define BVCYCLETIPS(cycle)   printf("%s", cycle)
#define BVTIPS(tips)         printf("%s", tips)

#define  BV_MAX_NAME_LEN            32

#define  BV_MAX_KIND_COUNTS         64

typedef struct __BV_Print {
    int              bv_print_level;
    int              bv_print_style;
    char             bv_name[BV_MAX_NAME_LEN+1];
    char             bv_style[BV_MAX_STRING_LEN+1];
    char            *bv_color;
    int              bv_enable_color;
    int              bv_used;
    int              bv_kind_cur;
    int              bv_kind[BV_MAX_KIND_COUNTS];
    FILE            *file;
    int              save_type;  // how to save, 0 console, -1 none, 1 to file(use zlog, so depends on config of zlog.conf)
} BV_Print;

static BV_Print bv_print_out = {
    BV_PRINT_ALL,
    BV_PRINT_STYLE_ALL,
    "bv_print",
    "",
    (char*)BV_COLOR_NONE,
    BV_COLOR_TRUE,
    0,0
};

static BV_Print *bv_p = &bv_print_out;

BV_EXTERN_C_FUNC int bv_print_init(int bv_print_level, 
              int bv_print_style, int bv_print_enable_color, 
              const char *name)
{
    if (bv_p != NULL) {
        if (bv_p->bv_used == 1) {
            printf("Warn: you have inited a print, exit...\n");
            return -3;
        }
    }

    bv_p->bv_enable_color = bv_print_enable_color;
    bv_p->bv_color = (char*)BV_COLOR_NONE;
    bv_p->bv_print_level = bv_print_level;
    bv_p->bv_print_style = bv_print_style;
    bv_p->save_type = BV_PRINT_LOG_TYPE_CONSOLE;
    if (name != NULL) {
        int len = strlen(name);
        if (len > BV_NAME_LEN_SUPPORT || len > BV_MAX_NAME_LEN) {
            printf("WARN: long name [%d] > [%d] max support[%d], will be trancated\n", 
                   len, BV_MAX_NAME_LEN, BV_NAME_LEN_SUPPORT);
        }
        if (len <= 0) sprintf(bv_p->bv_name, "%s", "bv_print");
    } else {
        sprintf(bv_p->bv_name, "%s", "(bv_p)rint");
    }
    strncpy(bv_p->bv_name, name, sizeof(bv_p->bv_name));
    memset(bv_p->bv_style, 0, sizeof(bv_p->bv_style));

    bv_p->bv_used = 1;
    int i;
    for (i = 0; i < BV_MAX_KIND_COUNTS; i++) {
        bv_p->bv_kind[i] = 0;
    }

    return 0;
}

BV_EXTERN_C_FUNC int bv_print_deinit()
{
    if (bv_p) { 
#ifndef UNUSE_ZLOG
    if (bv_p->save_type == BV_PRINT_LOG_TYPE_FILE)
    zlog_fini();
#endif
    bv_p->bv_used = 0; 
    }
    return 0;
}

BV_EXTERN_C_FUNC int bv_print_init_file_conf(const char *filepath, const char *processName)
{
    if (!bv_p) return -1;
#ifndef UNUSE_ZLOG
    if (bv_p->save_type == BV_PRINT_LOG_TYPE_FILE)
    dzlog_init(filepath, name);
#endif
    return 0;
}

// @type: 0 console, -1 none (not print), 1 to file (depends on /etc/zlog.conf)
BV_EXTERN_C_FUNC int bv_print_set_save_type(int type)
{
    if (bv_p) { bv_p->save_type = 0; }
    return 0;
}

BV_EXTERN_C_FUNC int bv_print_get_log_type()
{
    if (bv_p) return bv_p->save_type;
    return -1;
}

BV_EXTERN_C_FUNC int bv_print_set_color(int level)
{
    if ((bv_p)->bv_enable_color != BV_COLOR_TRUE) return -1;

    if (level & BV_PRINT_NONE) {
        (bv_p)->bv_color = (char*)BV_COLOR_NONE;
    } else if (level & BV_PRINT_ERROR) {
        (bv_p)->bv_color = (char*)BV_COLOR_RED;
    } else if (level & BV_PRINT_WARN) {
        (bv_p)->bv_color = (char*)BV_COLOR_YELLOW;
    } else if (level & BV_PRINT_INFO) {
        (bv_p)->bv_color = (char*)BV_COLOR_WHITE;
    } else if (level & BV_PRINT_DEBUG) {
        (bv_p)->bv_color = (char*)BV_COLOR_GREEN;
    } else if (level & BV_PRINT_CYCLE) {
        (bv_p)->bv_color = (char*)BV_COLOR_LIGHT_CYAN;
    } else if (level & BV_PRINT_MESSAGE) {
        (bv_p)->bv_color = (char*)BV_COLOR_LIGHT_GRAY;
    } else {
    	(bv_p)->bv_color = (char*)BV_COLOR_CYAN;
    }

    return 0;
}


BV_EXTERN_C_FUNC int bv_is_print_level_none(int level)
{
    if (bv_p == NULL) return -1;
    if (level & BV_PRINT_NONE) return -2;
    return 0;
}

// about name time, func....
BV_EXTERN_C_FUNC int bv_print_before(int type, const char *func, int line, const char *file)
{
    if (bv_p == NULL) return -1;
    char    pout[1024];

    int style = (bv_p)->bv_print_style;
    int has_done = 0;

    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        if ((bv_p)->bv_color != NULL)
            printf("%s", (bv_p)->bv_color);
    }
    if (type == -1) {
        memset((bv_p)->bv_style, 0, sizeof((bv_p)->bv_style));
        goto out;
    }

    if (style & BV_PRINT_STYLE_ALL) {
        BVALL((bv_p)->bv_name, func, line, file);
    } else if (style & BV_PRINT_STYLE_NONE) {
        memset((bv_p)->bv_style, 0, sizeof((bv_p)->bv_style));
        goto out;
    } else {
        if (strlen((bv_p)->bv_name) > 0 && !(style & BV_PRINT_NONAME)) {
            BVNAME((bv_p)->bv_name);
        } 
        if (!(style & BV_PRINT_NOTIME)) {
            BVTIME();
        } 
        if (!(style & BV_PRINT_NOFUNC)) {
            BVLEFT();BVFUNC(func);
            has_done = 1;
        } 
        if (!(style & BV_PRINT_NOLINE)) {
            if (has_done == 0) BVLEFT();
            else if (has_done == 1) BVCOLON();
            BVLINE(line);
            has_done = 1;
        }
        if (!(style & BV_PRINT_NOFILE)) { 
            if (has_done == 0) BVLEFT();
            else if (has_done == 1) BVAT();
            BVFILE(file);
            has_done = 1;
        }
        if (has_done == 1) BVRIGHT();
    }

    if (style != BV_PRINT_STYLE_NONE) {
        switch(type) {
            case BV_PRINT_ERROR:
                BVERRORTIPS("[ERROR]**");
            break;
            case BV_PRINT_WARN:
                BVWARNTIPS("[WARN]*");
            break;
            case BV_PRINT_INFO:
                BVINFOTIPS("[INFO]");
            break;
            case BV_PRINT_MESSAGE:
                BVMESSAGETIPS("[MSG]");
            break;
            case BV_PRINT_DEBUG:
                BVDEBUGTIPS("[DEBUG]");
            break;
            case BV_PRINT_CYCLE:
                BVCYCLETIPS("[CYCLE]");
            break;
            default:
            break;
        }
        BVTIPS(" ==> ");
    }

out:
    sprintf(pout, "%s", (bv_p)->bv_style);
    printf("%s", pout);

    return 0;
}

BV_EXTERN_C_FUNC int bv_print(int type, const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    if ((bv_p)->bv_print_level & BV_PRINT_NONE) return -2;
    if (!((bv_p)->bv_print_level & type)) return -3;

    bv_print_set_color(type);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(type, func, line, file);
    printf("%s", out);

    va_end(arg_ptr);

    return 0;
}

BV_EXTERN_C_FUNC int bv_print_style_none(const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    //if (!(bv_p->bv_print_level & BV_PRINT_NONE)) return -2;
    bv_print_set_color(BV_PRINT_DEBUG);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(-1, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }
    va_end(arg_ptr);

    return 0;
}

BV_EXTERN_C_FUNC int bv_print_error(const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    if (bv_is_print_level_none((bv_p)->bv_print_level) < 0)  return -2;
    bv_print_set_color(BV_PRINT_ERROR);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(BV_PRINT_ERROR, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
}

BV_EXTERN_C_FUNC int bv_print_warn(const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    if (bv_is_print_level_none((bv_p)->bv_print_level) < 0) return -2;
    if (!((bv_p)->bv_print_level & BV_PRINT_WARN)) return -2;
    bv_print_set_color(BV_PRINT_WARN);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(BV_PRINT_WARN, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

BV_EXTERN_C_FUNC int bv_print_info(const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    if (bv_is_print_level_none((bv_p)->bv_print_level) < 0) return -2;
    if (!((bv_p)->bv_print_level & BV_PRINT_INFO)) return -2;
    bv_print_set_color(BV_PRINT_INFO);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(BV_PRINT_INFO, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

BV_EXTERN_C_FUNC int bv_print_message(const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    if (bv_is_print_level_none((bv_p)->bv_print_level) < 0) return -2;
    //if (!(bv_p->bv_print_level & BV_PRINT_INFO)) return -2;
    bv_print_set_color(BV_PRINT_MESSAGE);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(BV_PRINT_MESSAGE, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

BV_EXTERN_C_FUNC int bv_print_debug(const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    if (bv_is_print_level_none((bv_p)->bv_print_level) < 0) return -2;
    if (!((bv_p)->bv_print_level & BV_PRINT_DEBUG)) return -2;
    bv_print_set_color(BV_PRINT_DEBUG);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(BV_PRINT_DEBUG, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

BV_EXTERN_C_FUNC int bv_print_cycle(const char *func, int line, const char *file, const char *format, ...)
{
    if (bv_p == NULL) return -1;
    if (bv_is_print_level_none((bv_p)->bv_print_level) < 0) return -2;
    if (!((bv_p)->bv_print_level & BV_PRINT_CYCLE)) return -2;
    bv_print_set_color(BV_PRINT_CYCLE);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(BV_PRINT_CYCLE, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
} 

BV_EXTERN_C_FUNC int bv_print_change_level(int level)
{
    if ((bv_p) == NULL) return -1;

    return (bv_p)->bv_print_level = level;
}

BV_EXTERN_C_FUNC int bv_print_start_cycle()
{
    if ((bv_p) == NULL) return -1;
 
    return (bv_p)->bv_print_level |= BV_PRINT_CYCLE;
}

BV_EXTERN_C_FUNC int bv_print_stop_cycle()
{
    if ((bv_p) == NULL) return -1;

    return (bv_p)->bv_print_level &= ~BV_PRINT_CYCLE;
}
 
BV_EXTERN_C_FUNC int bv_print_start_debug()
{
    if ((bv_p) == NULL) return -1;

    return (bv_p)->bv_print_level |= BV_PRINT_DEBUG;
}

BV_EXTERN_C_FUNC int bv_print_stop_debug()
{
    if ((bv_p) == NULL) return -1;

    return (bv_p)->bv_print_level &= ~BV_PRINT_DEBUG;
}


BV_EXTERN_C_FUNC int bv_print_add_kind(int kind)
{
    if (!bv_p) return -1;
    int i = 0;
    for (i = 0; i < BV_MAX_KIND_COUNTS; i++) {
        if (bv_p->bv_kind[i] == 0) {
            bv_p->bv_kind[i] = kind;
            bv_p->bv_kind_cur++;
            break;
        }
    }
    return bv_p->bv_kind_cur;
}

BV_EXTERN_C_FUNC int bv_print_del_kind(int kind)
{
    if (!bv_p) return -1;
    int i;
    for (i = 0; i < BV_MAX_KIND_COUNTS; i++) {
        if (bv_p->bv_kind[i] == 0) break;
        if (bv_p->bv_kind[i] == kind) {
            bv_p->bv_kind[i] = bv_p->bv_kind[bv_p->bv_kind_cur-1];
            bv_p->bv_kind_cur--;
        }
    }
	return bv_p->bv_kind_cur;
}

BV_EXTERN_C_FUNC int bv_print_kind(int kind, const char *func, int line, const char *file, const char *format, ...)
{
    if ((bv_p) == NULL) return -1;
    if (bv_is_print_level_none((bv_p)->bv_print_level) < 0) return -2;
    int i;
    for (i = 0; i < bv_p->bv_kind_cur; i++) {
        if (bv_p->bv_kind[i] == 0) break;
        if (bv_p->bv_kind[i] == kind) break;
    }
    if (i == bv_p->bv_kind_cur) return -3;
    bv_print_set_color(0);

    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    bv_print_before(BV_PRINT_DEBUG, func, line, file);
    printf("%s", out);
    if ((bv_p)->bv_enable_color == BV_COLOR_TRUE) {
        printf(BV_COLOR_NONE);
    }

    va_end(arg_ptr);

    return 0;
}

BV_EXTERN_C_FUNC void bv_print_error_string(int errno)
{
    switch(errno) {
        case EIO: bverror("EIO(%d): [%s] io operation error\n", errno, strerror(errno));
        break;
        case EINTR: bverror("EINTR(%d): [%s] interrupt\n", errno, strerror(errno));
        break;
        case EAGAIN: bverror("EAGAIN(%d): [%s] deal again later\n", errno, strerror(errno));
        break;
        case EFAULT: bverror("EFAULT(%d): [%s] pointer param error\n", errno, strerror(errno));
        break;
        case EBADF: bverror("EBADF(%d): [%s] bad file descriptor\n", errno, strerror(errno));
        break;
        case ENOMEM: bverror("ENOMEM(%d): [%s] no memory\n", errno, strerror(errno));
        break;
        case EINVAL: bverror("EINVAL(%d): [%s] invalid param \n", errno, strerror(errno));
        break;
        default:
        bverror("(%d):[%s]", errno, strerror(errno));
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// section - test of write to file
///////////////////////////////////////////////////////////////////////////////

BV_EXTERN_C_FUNC int bv_open_file(const char *path)
{
    if (bv_p == NULL) return -1;

    bv_p->file = fopen(path, "a+");
    if (!bv_p->file) return -2;

    return 0;
}

BV_EXTERN_C_FUNC int bv_close_file()
{
    if (bv_p == NULL) return -1;
  
    if (bv_p->file) { fclose(bv_p->file); bv_p->file = NULL; }

    return 0;
}

BV_EXTERN_C_FUNC int bv_writeto_file(const char *func, int line, const char *file, const char *format, ...)
{
    if (bv_p == NULL) return -1;
    char  out[BV_STRING_MAX_LEN_SUPPORT];
    memset(out, 0, sizeof(out));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(out, format, arg_ptr);

    int ret = fwrite(out, 1, strlen(out), bv_p->file);
    if (ret <= 0) {
        printf("error [%d]: \n", ret);
    }

    va_end(arg_ptr);

    return 0;
}

#if 0
int main()
{
    BVVERSION("test for print start \n");
    bvmsg("form message start");

    return 0;
}
#endif

/*=============== End of file: bv_print.c ==========================*/

