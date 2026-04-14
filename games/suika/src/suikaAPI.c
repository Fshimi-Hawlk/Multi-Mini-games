/**
    @file suikaAPI.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @date 2026-04-14
    @brief Implementation of the Suika mini-game API.
*/
#include "suikaAPI.h"  // Must be first - defines SuikaGame_St opaque type
#include "utils/types.h"  // Includes raylib and defines SuikaGame_St struct

#include "core/game.h"
#include "utils/configs.h"
#include "logger.h"
#include "utils/audio.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

/**
    @brief Wrapper function for freeGame callback.

    @param[in,out] game Double pointer to game state (as void* for callback signature)
    @return             Error_Et code
*/
static Error_Et suika_freeGameWrapper(void* game)
{
    return suika_freeGame((SuikaGame_St**) game);
}

/**
    @brief Core initialization function for the Suika game.

    @param[out]    game_ptr Double pointer to the game handle to be allocated
    @param[in]     configs  Initial game configurations
    @return                 Error_Et code
*/
Error_Et suika_initGame__full(SuikaGame_St** game_ptr, SuikaConfigs_St configs)
{
    (void)configs;

    static bool seeded = false;
    if (!seeded)
    {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    if (game_ptr == NULL)
    {
        log_error("NULL game pointer in suika_initGame");
        return ERROR_NULL_POINTER;
    }

    *game_ptr = malloc(sizeof(SuikaGame_St));
    if (*game_ptr == NULL)
    {
        log_error("Failed to allocate SuikaGame_St");
        return ERROR_ALLOC;
    }

    SuikaGame_St* game = *game_ptr;
    memset(game, 0, sizeof(*game));

    game->base.freeGame = suika_freeGameWrapper;
    game->base.running = true;

    suika_loadAssets(game);
    suika_initAudio();
    suika_init(game);

    log_debug("Suika initialized successfully");
    return OK;
}

/**
    @brief Main loop for the Suika game.

    @param[in,out] game Pointer to the game handle
    @return             Error_Et code
*/
Error_Et suika_gameLoop(SuikaGame_St* const game)
{
    if (game == NULL)
    {
        log_error("NULL game pointer in suika_gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running)
    {
        return OK;
    }

    // ESC key returns to lobby
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
        return OK;
    }

    float deltaTime = GetFrameTime();

    suika_update(game, deltaTime);

    BeginDrawing();
        suika_draw(game);
    EndDrawing();

    return OK;
}

/**
    @brief Free all resources associated with the Suika game.

    @param[in,out] game_ptr Double pointer to the game handle to be freed
    @return                 Error_Et code
*/
Error_Et suika_freeGame(SuikaGame_St** game_ptr)
{
    if (game_ptr == NULL || *game_ptr == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    SuikaGame_St* game = *game_ptr;

    suika_unloadAssets(game);
    suika_freeAudio();
    suika_cleanup(game);

    free(game);
    *game_ptr = NULL;

    return OK;
}
