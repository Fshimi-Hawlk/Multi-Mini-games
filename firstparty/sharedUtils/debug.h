/**
    @file debug.h
    @author Multi Mini-Games Team
    @date 2026-04-03
    @date 2026-04-14
    @brief Debug utilities and development macros (TODO, UNREACHABLE, UNUSED, etc.).
*/
#ifndef FIRSTPARTY_UTILS_DEBUG_H
#define FIRSTPARTY_UTILS_DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

/**
    @brief Invalid pointer value used as a sentinel.
*/
#define INVALID_PTR (void *) -1

/**
    @brief Debug macro to log when a function is called (uses log_debug).
*/
#define dcall log_debug("called")

/**
    @brief Add a new line.
*/
#define nl printf(LINE_END);

/**
    @brief Set a variable to an unused state making the compiler stop complaining while developing.
*/
#define UNUSED(value) (void)(value)

/**
    @brief Create a stopping point with extra message for development purpose.
*/
#define TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

/**
    @brief Use whenever something should never happen, not from human mistake but rather from memory corruption or similar.
*/
#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#ifdef _WIN32
#    define LINE_END "\r\n"
#else
#    define LINE_END "\n"
#endif

#endif // FIRSTPARTY_UTILS_DEBUG_H