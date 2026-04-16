/**
    @file error.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Error handling macros for the chess game.
*/
#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

/**
    @brief Error logging macro.

    Prints an error message to stderr with the format specified.
    Usage: error("Message: %d", value); or error("Message");
*/
#define error(fmt, ...) fprintf(stderr, "[ERROR] : " fmt "\n", ##__VA_ARGS__)

#endif
