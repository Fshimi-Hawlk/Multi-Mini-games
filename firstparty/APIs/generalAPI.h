/**
 * @file generalAPI.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief General API definitions and common error codes.
 */

#ifndef GENERAL_API_H
#define GENERAL_API_H

/**
 * @enum Error_Et
 * @brief Common error codes for API functions.
 */
typedef enum { 
    OK = 0,               /**< No error. */
    ERROR_NULL_POINTER,   /**< Null pointer encountered. */
    ERROR_ALLOC,          /**< Memory allocation failure. */
    ERROR_INVALID         /**< Invalid parameter or state. */
} Error_Et;

/**
 * @struct BaseGame_St
 * @brief Base structure for game state.
 */
typedef struct BaseGame_St {
    int running;          /**< 1 if the game is running, 0 otherwise. */
    int paused;           /**< 1 if the game is paused, 0 otherwise. */
    long score;           /**< Current game score. */
    /**
     * @brief Function pointer to free the game state.
     * @param game Pointer to the game state structure.
     * @return Error_Et status code.
     */
    Error_Et (*freeGame)(void*);
} BaseGame_St;

#endif
