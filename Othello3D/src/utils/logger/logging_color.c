#include "utils/logger/logging_color.h"

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