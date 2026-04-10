#include "logger.h"

ColorString_t logger_black = "\033[38;2;255;255;255m";
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

/**
    @brief Implementation of getLevelString.
    @param level Logging level.
    @return String representation.
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
    @brief Implementation of getLevelColor.
    @param level Logging level.
    @return Color string.
*/
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

void getTimeStamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    snprintf(buffer + strlen(buffer), size - strlen(buffer), ".%03ld", tv.tv_usec / 1000);
}