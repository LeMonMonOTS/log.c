/**
 * Copyright (c) 2020 rxi; 2026 LeMonMonOTS
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOG_VERSION "0.1.1"

#define LOG_MASSAGE_BUFFER_LEN 1024


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
typedef void (*log_LockFn)(int lock, void* udata);

typedef enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL } log_Level;

#define log_trace(MSG) _log_trace_impl(__FILE__, __LINE__, (MSG))
#define log_debug(MSG) _log_debug_impl(__FILE__, __LINE__, (MSG))
#define log_info(MSG) _log_info_impl(__FILE__, __LINE__, (MSG))
#define log_warn(MSG) _log_warn_impl(__FILE__, __LINE__, (MSG))
#define log_error(MSG) _log_error_impl(__FILE__, __LINE__, (MSG))
#define log_fatal(MSG) _log_fatal_impl(__FILE__, __LINE__, (MSG))

const char* log_level_string(const log_Level level);
void        log_set_lock(const log_LockFn fn, void* udata);
void        log_set_level(const log_Level level);
void        log_set_quiet(const int enable);
int         log_add_callback(const log_LogFn fn, void* udata, const log_Level level);
int         log_add_fp(FILE* fp, const log_Level level);

/* 为适配 C89/C90 标准，将可变参数宏改为字符串输入msg */
void _log_trace_impl(const char* file, const int line, const char* msg);
void _log_debug_impl(const char* file, const int line, const char* msg);
void _log_info_impl(const char* file, const int line, const char* msg);
void _log_warn_impl(const char* file, const int line, const char* msg);
void _log_error_impl(const char* file, const int line, const char* msg);
void _log_fatal_impl(const char* file, const int line, const char* msg);

/* 传入一个缓存区来格式化 */
int log_format(char massage_buffer[], const char* fmt, ...);


#endif
