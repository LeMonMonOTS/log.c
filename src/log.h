/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LOG_VERSION "0.1.0"
#define LOG_USE_COLOR

typedef struct
{
    va_list     ap;
    const char* fmt;
    const char* file;
    struct tm*  time;
    void*       udata;
    int         line;
    int         level;
} log_Event;

typedef void (*log_LogFn)(log_Event* ev);
typedef void (*log_LockFn)(bool lock, void* udata);

#define log_trace(MSG) _log_trace_impl(__FILE__, __LINE__, (MSG))
#define log_debug(MSG) _log_debug_impl(__FILE__, __LINE__, (MSG))
#define log_info(MSG) _log_info_impl(__FILE__, __LINE__, (MSG))
#define log_warn(MSG) _log_warn_impl(__FILE__, __LINE__, (MSG))
#define log_error(MSG) _log_error_impl(__FILE__, __LINE__, (MSG))
#define log_fatal(MSG) _log_fatal_impl(__FILE__, __LINE__, (MSG))

const char* log_level_string(int level);
void        log_set_lock(log_LockFn fn, void* udata);
void        log_set_level(int level);
void        log_set_quiet(bool enable);
int         log_add_callback(log_LogFn fn, void* udata, int level);
int         log_add_fp(FILE* fp, int level);

/* 为适配 C89/C90 标准，将可变参数宏改为字符串输入msg，需要外部用户通过sprintf格式化字符串 */
void _log_trace_impl(const char* file, int line, const char* msg);
void _log_debug_impl(const char* file, int line, const char* msg);
void _log_info_impl(const char* file, int line, const char* msg);
void _log_warn_impl(const char* file, int line, const char* msg);
void _log_error_impl(const char* file, int line, const char* msg);
void _log_fatal_impl(const char* file, int line, const char* msg);


#endif
