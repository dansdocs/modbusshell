/**
 * 
 * Daniel 2018
 * 
 * Simple logging - redirect the output by providing a function with a printf style signature. 
 * 
 * This is a single header file style library. See approach: 
 * https://github.com/RandyGaul/tinyheaders
 * https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
 * To use include as normal - but one and only onec file, just before #include "./log.h" also put in #define LOG_IMPLEMENTATION 
 * You can #include "./log.h" in other c files as normal. 
 * 
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define log_VERSION "0.1.0"

typedef void (*log_LockFn)(void *udata, int lock);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

//When writing the arduino version, it will be a NOP. 
//#define log_debug(...) do{}while(0)

void log_set_udata(void *udata);
void log_set_lock(log_LockFn fn);
void log_set_fp(FILE *fp);
void log_set_level(int level);
void log_set_quiet(int enable);

void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif // LOG_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#ifdef LOG_IMPLEMENTATION
#undef LOG_IMPLEMENTATION

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <time.h>
    
    static struct {
        void *udata;
        log_LockFn lock;
        FILE *fp;
        int level;
        int quiet;
    } log_L;


    static const char *log_level_names[] = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };

    #ifdef LOG_USE_COLOR
        static const char *log_level_colors[] = {
            "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
        };
    #endif


    static void log_lock(void)   {
      if (log_L.lock) {
        log_L.lock(log_L.udata, 1);
      }
    }
    
    
    static void log_unlock(void) {
      if (log_L.lock) {
        log_L.lock(log_L.udata, 0);
      }
    }
    
    
    void log_set_udata(void *udata) {
      log_L.udata = udata;
    }


    void log_set_lock(log_LockFn fn) {
      log_L.lock = fn;
    }
    
    
    void log_set_fp(FILE *fp) {
      log_L.fp = fp;
    }
    
    
    void log_set_level(int level) {
      log_L.level = level;
    }
    
    
    void log_set_quiet(int enable) {
      log_L.quiet = enable ? 1 : 0;
    }
    
    
    void log_log(int level, const char *file, int line, const char *fmt, ...) {
        if (level < log_L.level) {
            return;
        }
    
        /* Acquire lock */
        log_lock();
    
        /* Get current time */
        time_t t = time(NULL);
        struct tm *lt = localtime(&t);
    
        /* Log to stderr */
        if (!log_L.quiet) {
            va_list args;
            char buf[16];
            buf[strftime(buf, sizeof(buf), "%H:%M:%S", lt)] = '\0';
            #ifdef LOG_USE_COLOR
                fprintf(
                    stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
                    buf, log_level_colors[level], log_level_names[level], file, line);
            #else
                fprintf(stderr, "%s %-5s %s:%d: ", buf, log_level_names[level], file, line);
            #endif
            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
            fprintf(stderr, "\n");
        }

        /* Log to file */
        if (log_L.fp) {
            va_list args;
            char buf[32];
            buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
            fprintf(log_L.fp, "%s %-5s %s:%d: ", buf, log_level_names[level], file, line);
            va_start(args, fmt);
            vfprintf(log_L.fp, fmt, args);
            va_end(args);
            fprintf(log_L.fp, "\n");
        }

        /* Release lock */
        log_unlock();
    }
#endif // LOG_IMPLEMENTATION

