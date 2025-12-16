#include "utils/logger/logger.h"
#include "utils/logger/stacktrace.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/time.h>

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