#ifndef LOGGING_COLOR_H
#define LOGGING_COLOR_H

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

#endif // LOGGING_COLOR_H