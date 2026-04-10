#include "logger.h"

static FILE *logFile = NULL;  ///< Internal log file handle.

LogExtraInfoOpt_St _logExtraInfoOptions = {0};  ///< Internal logging options state.

static int startDepth = 3;  ///< Internal depth offset for stack traces.

#ifdef _DEBUG
    
    static const char *logFilePath = "logs/app.log";  ///< Path to the log file in debug builds.

    /**
        @brief Implementation of initLogger.
        @return 0 on success, negative on error.
     */
    int initLogger(void) {
        if (initSymbolHandler() < 0) {
            fprintf(stderr, "Failed to initialize symbol handler\n");
            return -1;
        }

        /* Create logs directory if it doesn't exist */
        mkdir("logs", 0755);

        logFile = fopen(logFilePath, "a");
        if (!logFile) {
            fprintf(stderr, "Failed to open log file: %s\n", logFilePath);
            cleanupSymbolHandler();
            return -2;
        }

        log_debug("Logger Initialized");
        
        return 0;
    }

    /**
        @brief Implementation of cleanupLogger.
     */
    void cleanupLogger(void) {
        if (logFile) {
            fclose(logFile);
            logFile = NULL;
        }

        cleanupSymbolHandler();
    }
#else
    int initLogger(void) { return 0; }
    void cleanupLogger(void) { }
#endif

/**
    @brief Implementation of logMessage.
    @param level Level.
    @param file File.
    @param line Line.
    @param func Func.
    @param fmt Format.
    @param ... Args.
*/
void logMessage(LoggingLevel_Et level, const char *file, int line, const char *func, const char *fmt, ...) {
    char timestamp[32];
    getTimeStamp(timestamp, sizeof(timestamp));

    ColorString_t levelStr = getLevelString(level);
    ColorString_t levelColor = getLevelColor(level);

    /* Format the log message */
    va_list args;
    char message[1024];
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    char logExtraInfoString[2048] = {0};
    snprintf(logExtraInfoString, sizeof(logExtraInfoString), "%s[%s%s\033[0m%s]\033[0m ", logger_yellow, levelColor, levelStr, logger_yellow);

    if (!_logExtraInfoOptions.hideLineId) {
        char lineLocationString[2048] = {0};
        snprintf(lineLocationString, sizeof(logExtraInfoString), "%s%s\033[0m:%s%d %s(%s%s%s)\033[0m: ", logger_fileColor, file, logger_lineColor, line, logger_yellow, logger_functionColor, func, logger_yellow);
        strcat(logExtraInfoString, lineLocationString);
    }

    /* Print to stderr and log file */
    fprintf(stderr, "%s%s\n", logExtraInfoString, message);

    if (_logExtraInfoOptions.enableTrace) {
        startDepth = 4;
        if (_logExtraInfoOptions.traceBackAmount > 0) printStackTrace(_logExtraInfoOptions.traceBackAmount);
        else printStackTrace(MAX_TRACE_BACK_FRAMES);
        startDepth = 3;
    }

    if (!_logExtraInfoOptions._persistentOpt)
        _logExtraInfoOptions = (LogExtraInfoOpt_St) {0};

    if (logFile) {
        fprintf(logFile, "%s [%s] %s:%d (%s): %s\n", timestamp, levelStr, file, line, func, message);
        fflush(logFile);
    }
}

/**
    @brief Implementation of printStackTrace.
    @param targetDepth Depth.
*/
void printStackTrace(unsigned int targetDepth) {
    /* Print stack trace until main */
    fprintf(stderr, "%sStack trace\033[0m:\n", getLevelColor(LOGGING_LEVEL_TRACE));
    if (logFile) {
        fprintf(logFile, "Stack trace:\n");
    }

    targetDepth = (targetDepth + startDepth) > MAX_TRACE_BACK_FRAMES ? MAX_TRACE_BACK_FRAMES : targetDepth + startDepth;

    for (unsigned int depth = startDepth; depth < targetDepth; depth++) {
        char buf[512];
        if (getCallerInfo(buf, sizeof(buf), depth) != 0) {
            fprintf(stderr, "  Error getting caller info at depth %u\n", depth);

            if (logFile) {
                fprintf(logFile, "  Error getting caller info at depth %u\n", depth);
            }

            break;
        }

        /* Parse buffer line by line */
        char *line = buf;
        char funcName[256] = {0};
        bool foundFunc = false;

        while (line && *line) {
            char *nextLine = strchr(line, '\n');
            if (nextLine) *nextLine = '\0';

            /* Extract function name for main detection */
            if (!foundFunc && strncmp(line, "Caller function: ", 17) == 0) {
                char *func_start = line + 17;
                char *func_end = strchr(func_start, '\n');
                
                if (!func_end) {
                    func_end = func_start + strlen(func_start);
                }
                
                size_t func_len = func_end - func_start;
                if (func_len >= sizeof(funcName)) {
                    func_len = sizeof(funcName) - 1;
                }

                strncpy(funcName, func_start, func_len);
                funcName[func_len] = '\0';
                foundFunc = true;
            }

            /* Print to stderr (colored) */
            if (strncmp(line, "Module: ", 8) == 0) {
                fprintf(stderr, "  %s\n", line);

            } else if (strncmp(line, "Caller function: ", 17) == 0) {
                char *func = line + 17;
                fprintf(stderr, "  %sCaller function: %s%s\033[0m\n", getLevelColor(LOGGING_LEVEL_TRACE), logger_functionColor, func);

            } else if (strncmp(line, "  at ", 5) == 0) {
                char *fileLine = line + 5;
                char *colon = strrchr(fileLine, ':');
                if (colon) {
                    *colon = '\0';
                    const char *file = fileLine;
                    const char *line_num = colon + 1;
                    fprintf(stderr, "  %sat %s%s:%s%s\033[0m\n", getLevelColor(LOGGING_LEVEL_TRACE), logger_fileColor, file, logger_lineColor, line_num);

                } else {
                    fprintf(stderr, "  %s%s\033[0m\n", logger_yellow, line);
                }

            } 

            /* Print to log file (plain text) */
            if (logFile) {
                fprintf(logFile, "  %s\n", line);
            }

            line = nextLine ? nextLine + 1 : NULL;
            if (nextLine) *nextLine = '\n';
        }

        /* Stop if we reach the main function */
        if (funcName[0] && strcmp(funcName, "main") == 0) {
            break;
        }
    }

    if (logFile) {
        fflush(logFile);
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

    
    static char   execPath[PATH_MAX_LENGTH];  ///< Global executable path for symbol resolution.
    
    static int    execPathInitialized = 0;  ///< Flag indicating if execPath is initialized.

    /**
        @brief Implementation of initSymbolHandler.
        @return Positive on success.
     */
    int initSymbolHandler(void) {
        if (execPathInitialized) return 1;
        ssize_t len = readlink("/proc/self/exe", execPath, sizeof(execPath)-1);
        if (len > 0 && len < (ssize_t)sizeof(execPath)) {
            execPath[len] = '\0';
        } else {
            execPath[0] = '\0';
        }
        execPathInitialized = 1;

        return execPathInitialized;
    }

    /**
        @brief Implementation of cleanupSymbolHandler.
     */
    void cleanupSymbolHandler(void) {
        /* No resources to free on Linux version */
        (void)execPathInitialized;
    }

    /**
        @brief Implementation of getCallerInfo.
        @param out Output buffer.
        @param outSize Buffer size.
        @param depth Depth.
        @return 0 on success.
     */
    int getCallerInfo(char *out, size_t outSize, unsigned int depth) {
        if (!execPathInitialized) {
            printf("symbols handler wasn't init\n");
            initSymbolHandler();
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
        const char *module = execPath;
        void *relAddr = addr;
        if (dladdr(addr, &info)) {
            if (info.dli_fname && info.dli_fname[0] != '\0') {
                module = info.dli_fname;
            }
            /* For PIE/ASLR, subtract the module base to get relative offset */
            relAddr = (void *)((uintptr_t)addr - (uintptr_t)info.dli_fbase);
        } 
        
        /* If we have a module path, ask addr2line for file:line */
        if (module[0] != '\0') {
            char cmd[PATH_MAX_LENGTH + 100];
            snprintf(cmd, sizeof(cmd),
                    "addr2line -f -C -e \"%s\" %p 2>/dev/null",
                    module, relAddr);
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
    int initSymbolHandler(void) { return 0; }
    void cleanupSymbolHandler(void) {}
    int getCallerInfo(char *out, size_t outSize, unsigned int depth) { 
        (void) outSize;
        (void) depth;

        out = ""; 
        (void) out;

        return 0; 
    }

#endif