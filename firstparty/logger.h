/**
 * @file logger.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Comprehensive logging system with colors and stack trace support.
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
 * @brief Maximum length for file paths.
 */
#define PATH_MAX_LENGTH  256

/**
 * @brief Maximum number of frames to capture in a stack trace.
 */
#ifndef MAX_TRACE_BACK_FRAMES
#define MAX_TRACE_BACK_FRAMES 32
#endif

/**
 * @brief Type for ANSI color strings.
 */
typedef const char *ColorString_t;

/**
 * @enum LoggingLevels_e
 * @brief Available logging levels.
 */
typedef enum LoggingLevels_e {
    LOGGING_LEVEL_LOG,   /**< Test level. */
    LOGGING_LEVEL_TRACE, /**< Tracing the code. Enabled in debug builds. */
    LOGGING_LEVEL_DEBUG, /**< Diagnostic information for developers. */
    LOGGING_LEVEL_INFO,  /**< Generally useful information. */
    LOGGING_LEVEL_WARN,  /**< Potential issues that don't stop the application. */
    LOGGING_LEVEL_ERROR, /**< Fatal error for the current operation. */
    LOGGING_LEVEL_FATAL  /**< Fatal error forcing application shutdown. */
} LoggingLevel_Et;

/** @brief ANSI Black color string. */
extern ColorString_t black;
/** @brief ANSI White color string. */
extern ColorString_t white;
/** @brief ANSI Yellow color string. */
extern ColorString_t yellow;

/** @brief Color for LOG level. */
extern ColorString_t logColor;
/** @brief Color for TRACE level. */
extern ColorString_t traceColor;
/** @brief Color for DEBUG level. */
extern ColorString_t debugColor;
/** @brief Color for INFO level. */
extern ColorString_t infoColor;
/** @brief Color for WARN level. */
extern ColorString_t warnColor;
/** @brief Color for ERROR level. */
extern ColorString_t errorColor;
/** @brief Color for FATAL level. */
extern ColorString_t fatalColor;

/** @brief Color for function names in logs. */
extern ColorString_t functionColor;
/** @brief Color for filenames in logs. */
extern ColorString_t fileColor;
/** @brief Color for line numbers in logs. */
extern ColorString_t lineColor;

/**
 * @struct LogExtraInfoOpt_s
 * @brief Options for additional information in log messages.
 */
typedef struct LogExtraInfoOpt_s {
    bool _persistentOpt;  /**< If true, options persist after one log call. */
    bool hideLineId;      /**< If true, hides [file line:column (funcName)]. */
    bool enableTrace;     /**< If true, enables stack trace for the log call. */
    int  traceBackAmount; /**< Number of frames to trace back. */
} LogExtraInfoOpt_St, *LogExtraInfoOptPtr_St;

/** @brief Global logging options. */
extern LogExtraInfoOpt_St _logExtraInfoOptions;

/**
 * @brief Macro to set logging options temporarily.
 */
#define setLogOpts(...) _logExtraInfoOptions = (LogExtraInfoOpt_St) {__VA_ARGS__}
/**
 * @brief Macro to reset logging options to default.
 */
#define resetLogOpts() _logExtraInfoOptions = (LogExtraInfoOpt_St) {0}
/**
 * @brief Macro to set logging options persistently.
 */
#define setLogOptsP(...) _logExtraInfoOptions = (LogExtraInfoOpt_St) {._persistentOpt = true, __VA_ARGS__}

/**
 * @brief Gets the string representation of a logging level.
 * @param level The logging level.
 * @return String representation (e.g., "INFO").
 */
extern ColorString_t getLevelString(LoggingLevel_Et level);

/**
 * @brief Gets the color string associated with a logging level.
 * @param level The logging level.
 * @return ANSI color string.
 */
extern ColorString_t getLevelColor(LoggingLevel_Et level);

/**
 * @brief Initializes the logger. Creates/opens the log file.
 * 
 * Call once at program start.
 * @return 0 on success, <0 on error.
 */
int init_logger(void);

/**
 * @brief Cleans up the logger. Closes the log file.
 * 
 * Call once at program end.
 */
void cleanup_logger(void);

/**
 * @brief Prints a stack trace to stderr and the log file.
 * @param targetDepth Maximum number of frames to print.
 */
void print_stack_trace(unsigned int targetDepth);

/**
 * @brief Logs a message with the specified level, file, line, function, and format.
 * 
 * @param level Log level (e.g., LOGGING_LEVEL_INFO).
 * @param file Source file name (usually __FILE__).
 * @param line Source line number (usually __LINE__).
 * @param func Function name (usually __func__).
 * @param fmt Format string, followed by variadic arguments.
 */
void log_message(LoggingLevel_Et level, const char *file, int line, const char *func, const char *fmt, ...);

/** @brief Macro for generic log message. */
#define logger_log(fmt, ...) log_message(LOGGING_LEVEL_LOG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
/** @brief Macro for INFO log message. */
#define log_info(fmt, ...)   log_message(LOGGING_LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
/** @brief Macro for WARN log message. */
#define log_warn(fmt, ...)   log_message(LOGGING_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
/** @brief Macro for FATAL log message. */
#define log_fatal(fmt, ...)  log_message(LOGGING_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

/** @brief Macro for DEBUG log message (enabled in _DEBUG builds). */
#ifdef _DEBUG
#define log_debug(fmt, ...)  log_message(LOGGING_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif

/** @brief Macro for ERROR log message. Triggers stack trace if _STACK_TRACE is defined. */
#ifdef _STACK_TRACE
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); print_stack_trace(MAX_TRACE_BACK_FRAMES)
#else
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#endif

/**
 * @brief Initializes the symbol handler for stack traces.
 * 
 * Call once at program start.
 * @return 0 or positive on success, <0 on error.
 */
int init_symbol_handler(void);

/**
 * @brief Cleans up the symbol handler.
 * 
 * Call once at program end.
 */
void cleanup_symbol_handler(void);

/**
 * @brief Fills a buffer with caller information at a specific backtrace depth.
 * 
 * @param out Buffer to fill.
 * @param outSize Size of the buffer.
 * @param depth Backtrace index (0 = self, 1 = direct caller, etc.).
 * @return 0 on success, <0 on error.
 */
int get_caller_info(char *out, size_t outSize, unsigned int depth);

/**
 * @brief Macro to print caller info at a specific depth.
 * @param depth Backtrace depth.
 */
#define print_caller_info_at_depth(depth)               \
    do {                                               \
        char __buf[512];                               \
        if (get_caller_info(__buf, sizeof __buf, depth) == 0) \
            puts(__buf);                               \
        else                                           \
            puts("Error getting caller info");         \
    } while(0)

/** @brief Macro to print info of the immediate caller. */
#define print_caller_info() print_caller_info_at_depth(2)

#endif // LOGGER_H

#ifdef LOGGER_IMPLEMENTATION

/** @brief ANSI Black color string implementation. */
ColorString_t black = "\033[38;2;255;255;255m";
/** @brief ANSI White color string implementation. */
ColorString_t white = "\033[38;2;255;255;255m";
/** @brief ANSI Yellow color string implementation. */
ColorString_t yellow = "\033[38;2;255;255;0m";

/** @brief Color for LOG level implementation. */
ColorString_t logColor = "\033[38;2;28;145;248m";
/** @brief Color for TRACE level implementation. */
ColorString_t traceColor = "\033[38;2;156;220;254m";
/** @brief Color for DEBUG level implementation. */
ColorString_t debugColor = "\033[38;2;197;93;192m";
/** @brief Color for INFO level implementation. */
ColorString_t infoColor = "\033[38;2;53;220;118m";
/** @brief Color for WARN level implementation. */
ColorString_t warnColor = "\033[38;2;206;145;60m";
/** @brief Color for ERROR level implementation. */
ColorString_t errorColor = "\033[1;38;2;220;20;20m";
/** @brief Color for FATAL level implementation. */
ColorString_t fatalColor = "\033[38;2;204;20;20m";

/** @brief Color for function names in logs implementation. */
ColorString_t functionColor = "\033[38;2;220;220;170m";
/** @brief Color for filenames in logs implementation. */
ColorString_t fileColor = "\033[38;2;78;201;176m";
/** @brief Color for line numbers in logs implementation. */
ColorString_t lineColor = "\033[38;2;181;206;168m";

/**
 * @brief Implementation of getLevelString.
 * @param level Logging level.
 * @return String representation.
 */
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

/**
 * @brief Implementation of getLevelColor.
 * @param level Logging level.
 * @return Color string.
 */
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

/** @brief Internal log file handle. */
static FILE *log_file = NULL;
/** @brief Internal logging options state. */
LogExtraInfoOpt_St _logExtraInfoOptions = {0};
/** @brief Internal depth offset for stack traces. */
static int startDepth = 3;

/**
 * @brief Gets the current timestamp.
 * @param buffer Output buffer.
 * @param size Buffer size.
 */
static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    snprintf(buffer + strlen(buffer), size - strlen(buffer), ".%03ld", tv.tv_usec / 1000);
}

#ifdef _DEBUG
    /** @brief Path to the log file in debug builds. */
    static const char *log_file_path = "logs/app.log";

    /**
     * @brief Implementation of init_logger.
     * @return 0 on success, negative on error.
     */
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

    /**
     * @brief Implementation of cleanup_logger.
     */
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

/**
 * @brief Implementation of log_message.
 * @param level Level.
 * @param file File.
 * @param line Line.
 * @param func Func.
 * @param fmt Format.
 * @param ... Args.
 */
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

    char logExtraInfoString[2048] = {0};
    snprintf(logExtraInfoString, sizeof(logExtraInfoString), "%s[%s%s\033[0m%s]\033[0m ", yellow, levelColor, levelStr, yellow);

    if (!_logExtraInfoOptions.hideLineId) {
        char lineLocationString[2048] = {0};
        snprintf(lineLocationString, sizeof(logExtraInfoString), "%s%s\033[0m:%s%d %s(%s%s%s)\033[0m: ", fileColor, file, lineColor, line, yellow, functionColor, func, yellow);
        strcat(logExtraInfoString, lineLocationString);
    }

    /* Print to stderr and log file */
    fprintf(stderr, "%s%s\n", logExtraInfoString, message);

    if (_logExtraInfoOptions.enableTrace) {
        startDepth = 4;
        if (_logExtraInfoOptions.traceBackAmount > 0) print_stack_trace(_logExtraInfoOptions.traceBackAmount);
        else print_stack_trace(MAX_TRACE_BACK_FRAMES);
        startDepth = 3;
    }

    if (!_logExtraInfoOptions._persistentOpt)
        _logExtraInfoOptions = (LogExtraInfoOpt_St) {0};

    if (log_file) {
        fprintf(log_file, "%s [%s] %s:%d (%s): %s\n", timestamp, levelStr, file, line, func, message);
        fflush(log_file);
    }
}

/**
 * @brief Implementation of print_stack_trace.
 * @param targetDepth Depth.
 */
void print_stack_trace(unsigned int targetDepth) {
    /* Print stack trace until main */
    fprintf(stderr, "%sStack trace\033[0m:\n", getLevelColor(LOGGING_LEVEL_TRACE));
    if (log_file) {
        fprintf(log_file, "Stack trace:\n");
    }

    targetDepth = (targetDepth + startDepth) > MAX_TRACE_BACK_FRAMES ? MAX_TRACE_BACK_FRAMES : targetDepth + startDepth;

    for (unsigned int depth = startDepth; depth < targetDepth; depth++) {
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

    /** @brief Global executable path for symbol resolution. */
    static char   g_exec_path[PATH_MAX_LENGTH];
    /** @brief Flag indicating if g_exec_path is initialized. */
    static int    g_exec_path_initialized = 0;

    /**
     * @brief Implementation of init_symbol_handler.
     * @return Positive on success.
     */
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

    /**
     * @brief Implementation of cleanup_symbol_handler.
     */
    void cleanup_symbol_handler(void) {
        /* No resources to free on Linux version */
        (void)g_exec_path_initialized;
    }

    /**
     * @brief Implementation of get_caller_info.
     * @param out Output buffer.
     * @param outSize Buffer size.
     * @param depth Depth.
     * @return 0 on success.
     */
    int get_caller_info(char *out, size_t outSize, unsigned int depth) {
        if (!g_exec_path_initialized) {
            printf("symbols handler wasn't init\n");
            init_symbol_handler();
        }

        void *buffer[MAX_TRACE_BACK_FRAMES];
        int nptrs = backtrace(buffer, MAX_TRACE_BACK_FRAMES);
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
