/*
 * Copyright (c) 2020 rxi; 2026 LeMonMonOTS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "log.h"

#define MAX_CALLBACKS 32

typedef struct
{
    log_LogFn fn;
    void*     udata;
    int       level;
} Callback;

static struct
{
    void*      udata;
    log_LockFn lock;
    int        level;
    int        quiet;
    Callback   callbacks[MAX_CALLBACKS];
} L;

static const char* level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

#ifdef LOG_USE_COLOR
static const char* level_colors[] =
    {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif


static void stdout_callback(log_Event* ev) {
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
#ifdef LOG_USE_COLOR
    fprintf(ev->udata,
            "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
            buf,
            level_colors[ev->level],
            level_strings[ev->level],
            ev->file,
            ev->line);
#else
    fprintf(ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level], ev->file, ev->line);
#endif
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}


static void file_callback(log_Event* ev) {
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
    fprintf(ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level], ev->file, ev->line);
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}


static void lock(void) {
    if (L.lock) {
        L.lock(1, L.udata);
    }
}


static void unlock(void) {
    if (L.lock) {
        L.lock(0, L.udata);
    }
}


const char* log_level_string(const log_Level level) {
    return level_strings[level];
}


void log_set_lock(const log_LockFn fn, void* udata) {
    L.lock  = fn;
    L.udata = udata;
}


void log_set_level(const log_Level level) {
    L.level = level;
}


void log_set_quiet(const int enable) {
    L.quiet = enable;
}


int log_add_callback(const log_LogFn fn, void* udata, const log_Level level) {
    int i;
    for (i = 0; i < MAX_CALLBACKS; i++) {
        if (!L.callbacks[i].fn) {
            L.callbacks[i].fn    = fn;
            L.callbacks[i].udata = udata;
            L.callbacks[i].level = level;
            return 0;
        }
    }
    return -1;
}


int log_add_fp(FILE* fp, const log_Level level) {
    return log_add_callback(file_callback, fp, level);
}


static void init_event(log_Event* ev, void* udata) {
    if (!ev->time) {
        time_t t = time(NULL);
        ev->time = localtime(&t);
    }
    ev->udata = udata;
}


void log_log(int level, const char* file, int line, const char* fmt, ...) {
    int       i;
    log_Event ev;

    memset(&ev, 0, sizeof(ev));
    ev.fmt   = fmt;
    ev.file  = file;
    ev.line  = line;
    ev.level = level;

    lock();

    if (!L.quiet && level >= L.level) {
        init_event(&ev, stderr);
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    for (i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++) {
        Callback* cb = &L.callbacks[i];
        if (level >= cb->level) {
            init_event(&ev, cb->udata);
            va_start(ev.ap, fmt);
            cb->fn(&ev);
            va_end(ev.ap);
        }
    }

    unlock();
}


void _log_trace_impl(const char* file, const int line, const char* msg) {
    log_log(LOG_TRACE, file, line, msg);
}


void _log_debug_impl(const char* file, const int line, const char* msg) {
    log_log(LOG_DEBUG, file, line, msg);
}


void _log_info_impl(const char* file, const int line, const char* msg) {
    log_log(LOG_INFO, file, line, msg);
}


void _log_warn_impl(const char* file, const int line, const char* msg) {
    log_log(LOG_WARN, file, line, msg);
}


void _log_error_impl(const char* file, const int line, const char* msg) {
    log_log(LOG_ERROR, file, line, msg);
}


void _log_fatal_impl(const char* file, const int line, const char* msg) {
    log_log(LOG_FATAL, file, line, msg);
}


int log_format(char massage_buffer[], const char* fmt, ...) {
    va_list args;

    if (fmt == NULL) {
        return 0;
    }

    massage_buffer[0] = '\0';

    va_start(args, fmt);
    vsprintf(massage_buffer, fmt, args);
    va_end(args);

    return 1;
}