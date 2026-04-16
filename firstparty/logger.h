/**
    @file logger.h
    @author Multi Mini-Games Team
    @date 2025-07-14
    @date 2026-04-14
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

// ────────────────────────────────────────────────
// Types & Globals
// ────────────────────────────────────────────────

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


extern ColorString_t logger_black;  ///< ANSI Black color string.
extern ColorString_t logger_white;  ///< ANSI White color string.
extern ColorString_t logger_yellow;  ///< ANSI Yellow color string.


extern ColorString_t logger_logColor;  ///< Color for LOG level.
extern ColorString_t logger_traceColor;  ///< Color for TRACE level.
extern ColorString_t logger_debugColor;  ///< Color for DEBUG level.
extern ColorString_t logger_infoColor;  ///< Color for INFO level.
extern ColorString_t logger_warnColor;  ///< Color for WARN level.
extern ColorString_t logger_errorColor;  ///< Color for ERROR level.
extern ColorString_t logger_fatalColor;  ///< Color for FATAL level.

extern ColorString_t logger_functionColor;  ///< Color for function names in logs.
extern ColorString_t logger_fileColor;  ///< Color for filenames in logs.
extern ColorString_t logger_lineColor;  ///< Color for line numbers in logs.

/**
    @brief Runtime options controlling log output appearance and detail.
*/
typedef struct {
    bool _persistentOpt;        // if true, options are not reset after each log
    bool hideLineId;            // suppress file:line (func) prefix
    bool enableTrace;           // print stack trace after every log message
    int  traceBackAmount;       // number of frames to show (0 = use MAX_TRACE_BACK_FRAMES)
} LogExtraInfoOpt_St;


extern LogExtraInfoOpt_St _logExtraInfoOptions;  ///< Global logging options.

// Convenience macros to set logging options
#define setLogOpts(...)     _logExtraInfoOptions = (LogExtraInfoOpt_St) {__VA_ARGS__}
#define resetLogOpts()      _logExtraInfoOptions = (LogExtraInfoOpt_St) {0}
#define setLogOptsP(...)    _logExtraInfoOptions = (LogExtraInfoOpt_St) {._persistentOpt = true, __VA_ARGS__}

// ────────────────────────────────────────────────
// Logging Macros (preferred interface)
// ────────────────────────────────────────────────


#define logger_log(fmt, ...) logMessage(LOGGING_LEVEL_LOG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)    ///< Macro for generic log message.
#define log_info(fmt, ...)   logMessage(LOGGING_LEVEL_INFO,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)  ///< Macro for INFO log message.
#define log_warn(fmt, ...)   logMessage(LOGGING_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)  ///< Macro for WARN log message.
#define log_fatal(fmt, ...)  do { logMessage(LOGGING_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); abort(); } while(0)  ///< Macro for FATAL log message.


#ifdef _DEBUG  ///< Macro for DEBUG log message (enabled in _DEBUG builds).
#define log_debug(fmt, ...)  logMessage(LOGGING_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#endif


#ifdef _STACK_TRACE  ///< Macro for ERROR log message. Triggers stack trace if _STACK_TRACE is defined.
#define log_error(fmt, ...) do { logMessage(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); printStackTrace(MAX_TRACE_BACK_FRAMES); } while(0)
#else
#define log_error(fmt, ...) logMessage(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#endif

// ────────────────────────────────────────────────
// Core API
// ────────────────────────────────────────────────

extern ColorString_t getLevelString(LoggingLevel_Et level);

/**
    @brief Description for getLevelColor
    @param[in,out] level The level parameter
    @return Success/failure or the result of the function
*/
extern ColorString_t getLevelColor(LoggingLevel_Et level);

// ────────────────────────────────────────────────
// Core API
// ────────────────────────────────────────────────

extern ColorString_t getLevelString(LoggingLevel_Et level);

/**
    @brief Description for getLevelColor
    @param[in,out] level The level parameter
    @return Success/failure or the result of the function
*/
extern ColorString_t getLevelColor(LoggingLevel_Et level);

/**
    @brief Initialize logger: create/open log file (debug builds) + symbol handler.
    @return 0 on success, negative on failure
*/
int initLogger(void);

/**
    @brief Close log file and clean up symbol handler.
*/
void cleanupLogger(void);

/**
    @brief Print stack trace to stderr and log file (if open).
    @param targetDepth maximum number of frames to show
*/
void printStackTrace(unsigned int targetDepth);

/**
    Log a message with the specified level, file, line, function, and format.
    @param level Log level (e.g., "INFO", "ERROR").
    @param file Source file name (use __FILE__).
    @param line Source line number (use __LINE__).
    @param func Function name (use __func__).
    @param fmt Format string, followed by variadic arguments.
*/
void logMessage(LoggingLevel_Et level, const char *file, int line, const char *func, const char *fmt, ...);

/**
    @brief Print caller info at specified depth to stdout.
*/
#define printCallerInfoAt(depth)               \
    do {                                               \
        char __buf[512];                               \
        if (get_caller_info(__buf, sizeof __buf, depth) == 0) \
            puts(__buf);                               \
        else                                           \
            puts("Error getting caller info");         \
    } while(0)


#define printCallerInfo() printCallerInfoAt(2)  ///< Macro to print info of the immediate caller.

// ────────────────────────────────────────────────
// Symbol / Caller Info (mostly Linux)
// ────────────────────────────────────────────────

/**
    @brief Initialize platform-specific symbol handler (addr2line preparation on Linux).
    @return 0 or positive on success
*/
int initSymbolHandler(void);

/**
    @brief Cleanup symbol handler resources (no-op on Linux).
*/
void cleanupSymbolHandler(void);

/**
    @brief Get caller information at given backtrace depth.
    @param out       output buffer
    @param outSize   size of output buffer
    @param depth     stack depth (0 = current function, 1 = caller, ...)
    @return 0 on success, <0 on error
*/
int getCallerInfo(char *out, size_t outSize, unsigned int depth);

/**
    @brief Returns a short human-readable timestamp string (e.g. "08:11:23").
           The returned pointer is valid only until the next call.
*/
const char* getCurrentTimeString(void);

/**
    @brief Returns a high-precision timestamp string (HH:MM:SS.mmm).
           Precision: milliseconds.
           The returned pointer is valid only until the next call to this function.
*/
const char* getPreciseTimeString(void);

/**
    @brief Gets the current timestamp.
    @param buffer Output buffer.
    @param size Buffer size.
*/
void getTimeStamp(char *buffer, size_t size);

#endif // LOGGER_H
