#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

#include <time.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/time.h>

#define MAX_FRAMES    64
#define PATH_MAX_LENGTH  256

typedef const char *ColorString_t;

typedef enum LoggingLevels_e {
    LOGGING_LEVEL_LOG, // Test level
    LOGGING_LEVEL_TRACE, // Tracing the code. Enabled in debug build 
    LOGGING_LEVEL_DEBUG, // Information that is diagnostically helpful to people more than just developers
    LOGGING_LEVEL_INFO, // Generally useful information to log
    LOGGING_LEVEL_WARN, // Anything that can potentially cause application oddities
    LOGGING_LEVEL_ERROR, // Any error which is fatal to the operation, but not the service or application
    LOGGING_LEVEL_FATAL // Any error that is forcing a shutdown of the service or application to prevent data loss
} LoggingLevel_Et;

extern ColorString_t black;
extern ColorString_t white;
extern ColorString_t yellow;

extern ColorString_t logColor;
extern ColorString_t traceColor;
extern ColorString_t debugColor;
extern ColorString_t infoColor;
extern ColorString_t warnColor;
extern ColorString_t errorColor;
extern ColorString_t fatalColor;

extern ColorString_t functionColor;
extern ColorString_t fileColor;
extern ColorString_t lineColor;

extern ColorString_t getLevelString(LoggingLevel_Et level);
extern ColorString_t getLevelColor(LoggingLevel_Et level);

/**
 * Initialize the logger. Creates/opens the log file.
 * Call once at program start.
 * Returns 0 on success, <0 on error.
 */
int init_logger(void);

/**
 * Cleanup the logger. Closes the log file.
 * Call once at program end.
 */
void cleanup_logger(void);

void print_stack_trace(void);

/**
 * Log a message with the specified level, file, line, function, and format.
 * @param level Log level (e.g., "INFO", "ERROR").
 * @param file Source file name (use __FILE__).
 * @param line Source line number (use __LINE__).
 * @param func Function name (use __func__).
 * @param fmt Format string, followed by variadic arguments.
 */
void log_message(LoggingLevel_Et level, const char *file, int line, const char *func, const char *fmt, ...);

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
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); print_stack_trace()
#else
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#endif


/**
 * Initialize the symbol handler. Call once at program start.
 */
int init_symbol_handler(void);

/**
 * Cleanup the symbol handler. Call once at program end.
 * (No-op on Linux, provided for API symmetry with Windows version.)
 */
void cleanup_symbol_handler(void);

/**
 * Fill `out` (of size `outSize`) with the caller info at
 * backtrace index `depth`.  E.g.:
 *   depth = 1 → immediate caller of the function that calls this.
 *
 * Returns 0 on success (even if symbol not found), <0 on error.
 */
int get_caller_info(char *out, size_t outSize, unsigned int depth);

/**
 * Convenience: print caller info at given `depth` (see above).
 */
#define print_caller_info_at_depth(depth)               \
    do {                                               \
        char __buf[512];                               \
        if (get_caller_info(__buf, sizeof __buf, depth) == 0) \
            puts(__buf);                               \
        else                                           \
            puts("Error getting caller info");         \
    } while(0)


#define print_caller_info() print_caller_info_at_depth(2)


#endif // LOGGER_H

#ifdef LOGGER_IMPLEMENTATION

/*

error: 204 102 102 | CC6666
warning: 240 198 116 | F0C674
info: 181 189 104 | B5BD68
debug: 178 148 187 | B294BB
trace: 138 190 183 | 8ABEB7
spam: 129 162 190 | 81A2BE

function: 220 220 170 | DCDCAA

"\033[38;2;255;255;255m"

*/

ColorString_t black = "\033[38;2;255;255;255m";
ColorString_t white = "\033[38;2;255;255;255m";
ColorString_t yellow = "\033[38;2;255;255;0m";

ColorString_t logColor = "\033[38;2;28;145;248m";
ColorString_t traceColor = "\033[38;2;156;220;254m";
ColorString_t debugColor = "\033[38;2;197;93;192m";
ColorString_t infoColor = "\033[38;2;53;220;118m";
ColorString_t warnColor = "\033[38;2;206;145;60m";
ColorString_t errorColor = "\033[1;38;2;220;20;20m";
ColorString_t fatalColor = "\033[38;2;204;20;20m";

ColorString_t functionColor = "\033[38;2;220;220;170m";
ColorString_t fileColor = "\033[38;2;78;201;176m";
ColorString_t lineColor = "\033[38;2;181;206;168m";

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
        case LOGGING_LEVEL_LOG: return logColor;
        case LOGGING_LEVEL_TRACE: return traceColor;
        case LOGGING_LEVEL_DEBUG: return debugColor;
        case LOGGING_LEVEL_INFO: return infoColor;
        case LOGGING_LEVEL_WARN: return warnColor;
        case LOGGING_LEVEL_ERROR: return errorColor;
        case LOGGING_LEVEL_FATAL: return fatalColor;
        default: return white;
    }
}

static FILE *log_file = NULL;

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
        if (init_symbol_handler() < 0) {
            fprintf(stderr, "Failed to initialize symbol handler\n");
            return -1;
        }

        /* Create logs directory if it doesn't exist */
        mkdir("logs", 0755);

        log_file = fopen(log_file_path, "a");
        if (!log_file) {
            fprintf(stderr, "Failed to open log file: %s\n", log_file_path);
            cleanup_symbol_handler();
            return -2;
        }

        log_debug("Logger Initialized");
        
        return 0;
    }

    void cleanup_logger(void) {
        if (log_file) {
            fclose(log_file);
            log_file = NULL;
        }

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

    /* Format the log message */
    va_list args;
    char message[1024];
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    /* Print to stderr and log file */
    fprintf(stderr, "%s[%s%s\033[0m%s] %s%s\033[0m:%s%d %s(%s%s%s)\033[0m: %s\n", yellow, levelColor, levelStr, yellow, fileColor, file, lineColor, line, yellow, functionColor, func, yellow, message);
    if (log_file) {
        fprintf(log_file, "%s [%s] %s:%d (%s): %s\n", timestamp, levelStr, file, line, func, message);
        fflush(log_file);
    }
}

void print_stack_trace(void) {
    /* Print stack trace until main */
    fprintf(stderr, "%sStack trace\033[0m:\n", getLevelColor(LOGGING_LEVEL_TRACE));
    if (log_file) {
        fprintf(log_file, "Stack trace:\n");
    }

    for (unsigned int depth = 3; depth < MAX_FRAMES; depth++) {
        char buf[512];
        if (get_caller_info(buf, sizeof(buf), depth) != 0) {
            fprintf(stderr, "  Error getting caller info at depth %u\n", depth);
            if (log_file) {
                fprintf(log_file, "  Error getting caller info at depth %u\n", depth);
            }
            break;
        }

        /* Parse buffer line by line */
        char *line = buf;
        char func_name[256] = {0};
        int found_func = 0;

        while (line && *line) {
            char *next_line = strchr(line, '\n');
            if (next_line) *next_line = '\0';

            /* Extract function name for main detection */
            if (!found_func && strncmp(line, "Caller function: ", 17) == 0) {
                char *func_start = line + 17;
                char *func_end = strchr(func_start, '\n');
                
                if (!func_end) {
                    func_end = func_start + strlen(func_start);
                }
                
                size_t func_len = func_end - func_start;
                if (func_len >= sizeof(func_name)) {
                    func_len = sizeof(func_name) - 1;
                }

                strncpy(func_name, func_start, func_len);
                func_name[func_len] = '\0';
                found_func = 1;
            }

            /* Print to stderr (colored) */
            if (strncmp(line, "Module: ", 8) == 0) {
                fprintf(stderr, "  %s\n", line);

            } else if (strncmp(line, "Caller function: ", 17) == 0) {
                char *func = line + 17;
                fprintf(stderr, "  %sCaller function: %s%s\033[0m\n", getLevelColor(LOGGING_LEVEL_TRACE), functionColor, func);

            } else if (strncmp(line, "  at ", 5) == 0) {
                char *file_line = line + 5;
                char *colon = strrchr(file_line, ':');
                if (colon) {
                    *colon = '\0';
                    const char *file = file_line;
                    const char *line_num = colon + 1;
                    fprintf(stderr, "  %sat %s%s:%s%s\033[0m\n", getLevelColor(LOGGING_LEVEL_TRACE), fileColor, file, lineColor, line_num);

                } else {
                    fprintf(stderr, "  %s%s\033[0m\n", yellow, line);
                }

            } 
            // else {
            //     fprintf(stderr, "  %s\n", line);
            // }

            /* Print to log file (plain text) */
            if (log_file) {
                fprintf(log_file, "  %s\n", line);
            }

            line = next_line ? next_line + 1 : NULL;
            if (next_line) *next_line = '\n';
        }

        /* Stop if we reach the main function */
        if (func_name[0] && strcmp(func_name, "main") == 0) {
            break;
        }
    }

    if (log_file) {
        fflush(log_file);
    }
}

#ifdef _DEBUG
    #ifndef __USE_GNU
    #define __USE_GNU
    #endif

    #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
    #endif

    // Linux-specific code
    #include <execinfo.h>
    #include <dlfcn.h>
    #include <stdint.h>
    #include <unistd.h>
    #include <limits.h>

    static char   g_exec_path[PATH_MAX_LENGTH];
    static int    g_exec_path_initialized = 0;

    /* Read /proc/self/exe once so we can call addr2line later */
    int init_symbol_handler(void) {
        if (g_exec_path_initialized) return 1;
        ssize_t len = readlink("/proc/self/exe", g_exec_path, sizeof(g_exec_path)-1);
        if (len > 0 && len < (ssize_t)sizeof(g_exec_path)) {
            g_exec_path[len] = '\0';
        } else {
            g_exec_path[0] = '\0';
        }
        g_exec_path_initialized = 1;

        return g_exec_path_initialized;
    }

    void cleanup_symbol_handler(void) {
        /* No resources to free on Linux version */
        (void)g_exec_path_initialized;
    }

    /**
    * depth: backtrace index.
    *   0 => this function,
    *   1 => the direct caller of this function,
    *   2 => caller’s caller, etc.
    */
    int get_caller_info(char *out, size_t outSize, unsigned int depth) {
        if (!g_exec_path_initialized) {
            printf("symbols handler wasn't init\n");
            init_symbol_handler();
        }

        void *buffer[MAX_FRAMES];
        int nptrs = backtrace(buffer, MAX_FRAMES);
        if (nptrs <= (int)depth) {
            return snprintf(out, outSize, "No stack frame at depth %u", depth) < 0 ? -1 : 0;
        }

        void *addr = buffer[depth];
        Dl_info info;

        const char *func = NULL;

        if (dladdr(addr, &info) && info.dli_sname) {
            func = info.dli_sname;
        } else {
            func = "[unknown]";
        }
        
        /* Write function name */
        int w = snprintf(out, outSize, "Caller function: %s", func);
        if (w < 0 || (size_t)w >= outSize) return -1;
        
        /* Determine module path and relative address for addr2line */
        const char *module = g_exec_path;
        void *rel_addr = addr;
        if (dladdr(addr, &info)) {
            if (info.dli_fname && info.dli_fname[0] != '\0') {
                module = info.dli_fname;
            }
            /* For PIE/ASLR, subtract the module base to get relative offset */
            rel_addr = (void *)((uintptr_t)addr - (uintptr_t)info.dli_fbase);
        } 
        
        /* If we have a module path, ask addr2line for file:line */
        if (module[0] != '\0') {
            char cmd[PATH_MAX_LENGTH + 100];
            /*
            * -f: show function name
            * -C: demangle C++ names (harmless for C)
            * -e: executable or shared object
            * Using a two-stage read: first line = function, second = file:line
            */
            snprintf(cmd, sizeof(cmd),
                    "addr2line -f -C -e \"%s\" %p 2>/dev/null",
                    module, rel_addr);
            FILE *fp = popen(cmd, "r");
            if (fp) {
                char linebuf[PATH_MAX_LENGTH + 50];
                /* Skip the function line (we already have it from dladdr) */
                if (fgets(linebuf, sizeof(linebuf), fp)) {
                    /* Read file:line */
                    if (fgets(linebuf, sizeof(linebuf), fp)) {
                        size_t len = strlen(linebuf);
                        if (len && linebuf[len-1] == '\n') {
                            linebuf[len-1] = '\0';
                        }
                        /* Append to output */
                        size_t cur = strlen(out);
                        snprintf(out + cur, outSize - cur, "\n  at %s", linebuf);
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
    int get_caller_info(char *out, size_t outSize, unsigned int depth) { 
        (void) outSize;
        (void) depth;

        out = ""; 
        (void) out;

        return 0; 
    }
#endif

#endif // LOGGER_IMPLEMENTATION