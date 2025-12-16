#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdio.h>

#include "logging_color.h"

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
#define log_debug(fmt, ...)  log_message(LOGGING_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)   log_message(LOGGING_LEVEL_WARN,  __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...)  log_message(LOGGING_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#ifdef _STACK_TRACE
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); print_stack_trace()
#else
#define log_error(fmt, ...) log_message(LOGGING_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#endif

#endif // LOGGER_H