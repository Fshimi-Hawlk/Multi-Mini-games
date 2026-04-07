/**
    @file logger.h
    @author Fshimi-Hawlk
    @date 2025-07-14
    @date 2026-02-18
    @brief Flexible, colored console + file logger with stack trace support (Linux-focused).
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>

/**
    @brief Maximum length for file paths.
*/
#define PATH_MAX_LENGTH  256

/**
    @brief Maximum number of frames to capture in a stack trace.
*/
#ifndef MAX_TRACE_BACK_FRAMES
#define MAX_TRACE_BACK_FRAMES 32
#endif

// 
// Types & Globals
// 

typedef const char *ColorString_t;

/**
    @enum LoggingLevels_e
    @brief Available logging levels.
*/
typedef enum LoggingLevels_e {
    LOGGING_LEVEL_LOG,   ///< Test level.
    LOGGING_LEVEL_TRACE, ///< Tracing the code. Enabled in debug builds.
    LOGGING_LEVEL_DEBUG, ///< Diagnostic information for developers.
    LOGGING_LEVEL_INFO,  ///< Generally useful information.
    LOGGING_LEVEL_WARN,  ///< Potential issues that don't stop the application.
    LOGGING_LEVEL_ERROR, ///< Fatal error for the current operation.
    LOGGING_LEVEL_FATAL  ///< Fatal error forcing application shutdown.
} LoggingLevel_Et;


extern ColorString_t logger_black;
extern ColorString_t logger_white;
extern ColorString_t logger_yellow;

extern ColorString_t logger_logColor;
extern ColorString_t logger_traceColor;
extern ColorString_t logger_debugColor;
extern ColorString_t logger_infoColor;
extern ColorString_t logger_warnColor;
extern ColorString_t logger_errorColor;
extern ColorString_t logger_fatalColor;

extern ColorString_t logger_functionColor;
extern ColorString_t logger_fileColor;
extern ColorString_t logger_lineColor;

/**
    @brief Runtime options controlling log output appearance and detail.
*/
typedef struct {
    bool _persistentOpt;
    bool hideLineId;
    bool enableTrace;
    int  traceBackAmount;
} LogExtraInfoOpt_St;


extern LogExtraInfoOpt_St _logExtraInfoOptions;

#define setLogOpts(...)     _logExtraInfoOptions = (LogExtraInfoOpt_St) {__VA_ARGS__}
#define resetLogOpts()      _logExtraInfoOptions = (LogExtraInfoOpt_St) {0}
#define setLogOptsP(...)    _logExtraInfoOptions = (LogExtraInfoOpt_St) {._persistentOpt = true, __VA_ARGS__}

// 
// Logging Macros
// 

#define logger_log(fmt, ...) log_message(LOGGING_LEVEL_LOG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)   log_message(LOGGING_LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)   log_message(LOGGING_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)  log_message(LOGGING_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#ifdef _DEBUG
#define log_debug(fmt, ...)  log_message(LOGGING_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif

#ifdef _STACK_TRACE
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); print_stack_trace(MAX_TRACE_BACK_FRAMES)
#else
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#endif

// 
// Core API
// 

extern ColorString_t getLevelString(LoggingLevel_Et level);
extern ColorString_t getLevelColor(LoggingLevel_Et level);

int init_logger(void);
void cleanup_logger(void);
void print_stack_trace(unsigned int targetDepth);
void log_message(LoggingLevel_Et level, const char *file, int line, const char *func, const char *fmt, ...);

int init_symbol_handler(void);
void cleanup_symbol_handler(void);
int get_caller_info(char *out, size_t outSize, unsigned int depth);

const char* getCurrentTimeString(void);
const char* getPreciseTimeString(void);

#define print_caller_info_at_depth(depth)               \
    do {                                               \
        char __buf[512];                               \
        if (get_caller_info(__buf, sizeof __buf, depth) == 0) \
            fprintf(stderr, "\033[92m%s\033[0m%s\n", depth == 1 ? "CALLER: " : "", __buf); \
        else                                           \
            fprintf(stderr, "Error getting caller info\n"); \
    } while(0)


#define print_caller_info() print_caller_info_at_depth(2)

#endif // LOGGER_H

#ifdef LOGGER_IMPLEMENTATION

ColorString_t logger_black = "\033[38;2;0;0;0m";
ColorString_t logger_white = "\033[38;2;255;255;255m";
ColorString_t logger_yellow = "\033[38;2;255;255;0m";

ColorString_t logger_logColor = "\033[38;2;28;145;248m";
ColorString_t logger_traceColor = "\033[38;2;156;220;254m";
ColorString_t logger_debugColor = "\033[38;2;197;93;192m";
ColorString_t logger_infoColor = "\033[38;2;53;220;118m";
ColorString_t logger_warnColor = "\033[38;2;206;145;60m";
ColorString_t logger_errorColor = "\033[1;38;2;220;20;20m";
ColorString_t logger_fatalColor = "\033[38;2;204;20;20m";

ColorString_t logger_functionColor = "\033[38;2;220;220;170m";
ColorString_t logger_fileColor = "\033[38;2;78;201;176m";
ColorString_t logger_lineColor = "\033[38;2;181;206;168m";

ColorString_t getLevelString(LoggingLevel_Et level) {
    switch (level) {
        case LOGGING_LEVEL_LOG: return "LOG";
        case LOGGING_LEVEL_TRACE: return "TRACE";
        case LOGGING_LEVEL_DEBUG: return "DEBUG";
        case LOGGING_LEVEL_INFO: return "INFO";
        case LOGGING_LEVEL_WARN: return "WARN";
        case LOGGING_LEVEL_ERROR: return "ERROR";
        case LOGGING_LEVEL_FATAL: return "FATAL";
        default: return "NONE";
    }
}

ColorString_t getLevelColor(LoggingLevel_Et level) {
    switch (level) {
        case LOGGING_LEVEL_LOG: return logger_logColor;
        case LOGGING_LEVEL_TRACE: return logger_traceColor;
        case LOGGING_LEVEL_DEBUG: return logger_debugColor;
        case LOGGING_LEVEL_INFO: return logger_infoColor;
        case LOGGING_LEVEL_WARN: return logger_warnColor;
        case LOGGING_LEVEL_ERROR: return logger_errorColor;
        case LOGGING_LEVEL_FATAL: return logger_fatalColor;
        default: return logger_white;
    }
}

const char* getCurrentTimeString(void) {
    static char buf[32];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    return buf;
}

const char* getPreciseTimeString(void) {
    static char buf[32];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm* tm_info = localtime(&tv.tv_sec);
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    snprintf(buf + 8, sizeof(buf) - 8, ".%03ld", tv.tv_usec / 1000);
    return buf;
}

static FILE *log_file = NULL;
LogExtraInfoOpt_St _logExtraInfoOptions = {0};
static int startDepth = 3;

static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    snprintf(buffer + strlen(buffer), size - strlen(buffer), ".%03ld", tv.tv_usec / 1000);
}

#ifdef _DEBUG
    static const char *log_file_path = "logs/app.log";
    int init_logger(void) {
        if (init_symbol_handler() < 0) return -1;
        mkdir("logs", 0755);
        log_file = fopen(log_file_path, "a");
        if (!log_file) return -2;
        return 0;
    }
    void cleanup_logger(void) {
        if (log_file) { fclose(log_file); log_file = NULL; }
        cleanup_symbol_handler();
    }
#else
    int init_logger(void) { return 0; }
    void cleanup_logger(void) { }
#endif

void log_message(LoggingLevel_Et level, const char *file, int line, const char *func, const char *fmt, ...) {
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    ColorString_t levelStr = getLevelString(level);
    ColorString_t levelColor = getLevelColor(level);
    va_list args;
    char message[1024];
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    char logExtraInfoString[2048] = {0};
    snprintf(logExtraInfoString, sizeof(logExtraInfoString), "%s[%s%s\033[0m%s]\033[0m ", logger_yellow, levelColor, levelStr, logger_yellow);
    if (!_logExtraInfoOptions.hideLineId) {
        char loc[2048] = {0};
        snprintf(loc, sizeof(loc), "%s%s\033[0m:%s%d %s(%s%s%s)\033[0m: ", logger_fileColor, file, logger_lineColor, line, logger_yellow, logger_functionColor, func, logger_yellow);
        strcat(logExtraInfoString, loc);
    }
    fprintf(stderr, "%s%s\n", logExtraInfoString, message);
    if (_logExtraInfoOptions.enableTrace) {
        startDepth = 4;
        print_stack_trace(_logExtraInfoOptions.traceBackAmount > 0 ? _logExtraInfoOptions.traceBackAmount : MAX_TRACE_BACK_FRAMES);
        startDepth = 3;
    }
    if (!_logExtraInfoOptions._persistentOpt) _logExtraInfoOptions = (LogExtraInfoOpt_St) {0};
    if (log_file) {
        fprintf(log_file, "%s [%s] %s:%d (%s): %s\n", timestamp, levelStr, file, line, func, message);
        fflush(log_file);
    }
}

void print_stack_trace(unsigned int targetDepth) {
    fprintf(stderr, "%sStack trace\033[0m:\n", getLevelColor(LOGGING_LEVEL_TRACE));
    if (log_file) fprintf(log_file, "Stack trace:\n");
    targetDepth = (targetDepth + startDepth) > MAX_TRACE_BACK_FRAMES ? MAX_TRACE_BACK_FRAMES : targetDepth + startDepth;
    for (unsigned int depth = startDepth; depth < targetDepth; depth++) {
        char buf[512];
        if (get_caller_info(buf, sizeof(buf), depth) != 0) break;
        char *line = buf;
        char func_name[256] = {0};
        bool found_func = false;
        while (line && *line) {
            char *next_line = strchr(line, '\n');
            if (next_line) *next_line = '\0';
            if (!found_func && strncmp(line, "Caller function: ", 17) == 0) {
                strncpy(func_name, line + 17, sizeof(func_name)-1);
                found_func = true;
            }
            if (strncmp(line, "Module: ", 8) == 0) fprintf(stderr, "  %s\n", line);
            else if (strncmp(line, "Caller function: ", 17) == 0) fprintf(stderr, "  %sCaller function: %s%s\033[0m\n", getLevelColor(LOGGING_LEVEL_TRACE), logger_functionColor, line + 17);
            else if (strncmp(line, "  at ", 5) == 0) {
                char *file_line = line + 5;
                char *colon = strrchr(file_line, ':');
                if (colon) {
                    *colon = '\0';
                    fprintf(stderr, "  %sat %s%s:%s%s\033[0m\n", getLevelColor(LOGGING_LEVEL_TRACE), logger_fileColor, file_line, logger_lineColor, colon + 1);
                } else fprintf(stderr, "  %s%s\033[0m\n", logger_yellow, line);
            }
            if (log_file) fprintf(log_file, "  %s\n", line);
            line = next_line ? next_line + 1 : NULL;
        }
        if (func_name[0] && strcmp(func_name, "main") == 0) break;
    }
    if (log_file) fflush(log_file);
}

#ifdef _DEBUG
    #ifndef __USE_GNU
    #define __USE_GNU
    #endif
    #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
    #endif
    #include <execinfo.h>
    #include <dlfcn.h>
    #include <stdint.h>
    #include <unistd.h>
    #include <limits.h>
    static char g_exec_path[PATH_MAX_LENGTH];
    static int g_exec_path_initialized = 0;
    int init_symbol_handler(void) {
        if (g_exec_path_initialized) return 1;
        ssize_t len = readlink("/proc/self/exe", g_exec_path, sizeof(g_exec_path)-1);
        if (len > 0) g_exec_path[len] = '\0';
        else g_exec_path[0] = '\0';
        g_exec_path_initialized = 1;
        return 1;
    }
    void cleanup_symbol_handler(void) {}
    int get_caller_info(char *out, size_t outSize, unsigned int depth) {
        if (!g_exec_path_initialized) init_symbol_handler();
        void *buffer[MAX_TRACE_BACK_FRAMES];
        int nptrs = backtrace(buffer, MAX_TRACE_BACK_FRAMES);
        if (nptrs <= (int)depth) return -1;
        void *addr = buffer[depth];
        Dl_info info;
        const char *func = (dladdr(addr, &info) && info.dli_sname) ? info.dli_sname : "[unknown]";
        snprintf(out, outSize, "Caller function: %s", func);
        const char *module = g_exec_path;
        void *rel_addr = addr;
        if (dladdr(addr, &info)) {
            if (info.dli_fname && info.dli_fname[0] != '\0') module = info.dli_fname;
            rel_addr = (void *)((uintptr_t)addr - (uintptr_t)info.dli_fbase);
        }
        if (module[0] != '\0') {
            char cmd[PATH_MAX_LENGTH + 100];
            snprintf(cmd, sizeof(cmd), "addr2line -f -C -e \"%s\" %p 2>/dev/null", module, rel_addr);
            FILE *fp = popen(cmd, "r");
            if (fp) {
                char linebuf[PATH_MAX_LENGTH + 50];
                if (fgets(linebuf, sizeof(linebuf), fp)) {
                    if (fgets(linebuf, sizeof(linebuf), fp)) {
                        size_t l = strlen(linebuf);
                        if (l && linebuf[l-1] == '\n') linebuf[l-1] = '\0';
                        strcat(out, "\n  at "); strcat(out, linebuf);
                    }
                }
                pclose(fp);
            }
        }
        return 0;
    }
#else 
    int init_symbol_handler(void) { return 0; }
    void cleanup_symbol_handler(void) {}
    int get_caller_info(char *out, size_t outSize, unsigned int depth) { (void)outSize; (void)depth; out[0] = '\0'; return 0; }
#endif

#endif // LOGGER_IMPLEMENTATION
