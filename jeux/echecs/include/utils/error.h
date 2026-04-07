/**
 * @file error.h
 * @author Maxime CHAUVEAU
 * @brief Error handling macros for the Echecs (Chess) game.
 * @version 1.0
 * @date 2024
 *
 * This file contains error reporting macros used throughout
 * the game for logging and debugging purposes.
 */

#ifndef UTILS_ERROR_H
#define UTILS_ERROR_H

#include <stdio.h>

/**
 * @brief Error logging macro.
 *
 * Prints an error message to stderr with the format specified.
 * Usage: error("Message: %d", value); or error("Message");
 */
#define error(fmt, ...) fprintf(stderr, "[ERROR] : " fmt "\n", ##__VA_ARGS__)

#endif
