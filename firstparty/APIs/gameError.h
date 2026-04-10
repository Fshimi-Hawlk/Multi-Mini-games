/**
 * @file gameError.h
 * @brief Global error codes for all mini-games
 * @author Maxime CHAUVEAU
 * @date February 2026
 */

#ifndef GAME_ERROR_H
#define GAME_ERROR_H

typedef enum {
    GAME_OK = 0,
    GAME_ERR_NULL_PTR,
    GAME_ERR_INVALID_CONFIG,
    GAME_ERR_INVALID_PARAM,
    GAME_ERR_ALLOC_FAILED,
    GAME_ERR_ASSET_LOAD_FAILED,
    GAME_ERR_WINDOW_INIT_FAILED,
    GAME_ERR_AUDIO_INIT_FAILED,
    GAME_ERR_OUT_OF_RANGE,
} GameError_Et;

#define GAME_IS_OK(err) ((err) == GAME_OK)
#define GAME_IS_ERROR(err) ((err) != GAME_OK)

#endif // GAME_ERROR_H
