/**
 * @file suikaAPI.c
 * @author Multi Mini-Games Team
 * @date February 2026
 * @brief Implementation of the Suika mini-game API.
 */

#include "core/game.h"
#include "utils/types.h"
#include "utils/configs.h"
#include "logger.h"
#include "suikaAPI.h"
#include "audio.h"
#include <time.h>

/**
 * @brief Wrapper function for freeGame callback.
 * @param game Double pointer to game state (as void* for callback signature)
 * @return Error code
 */
Error_Et suika_freeGameWrapper(void* game)
{
    return suika_freeGame((SuikaGame_St**) game);
}

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
    InitAudioDevice();
    initAudio();
    suika_init(game);

    log_debug("Suika initialized successfully");
    return OK;
}

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

    float deltaTime = GetFrameTime();

    suika_update(game, deltaTime);

    BeginDrawing();
        suika_draw(game);
    EndDrawing();

    return OK;
}

Error_Et suika_freeGame(SuikaGame_St** game_ptr)
{
    if (game_ptr == NULL || *game_ptr == NULL)
    {
        return ERROR_NULL_POINTER;
    }

    SuikaGame_St* game = *game_ptr;

    suika_unloadAssets(game);
    suika_cleanup(game);

    freeAudio();
    CloseAudioDevice();

    free(game);
    *game_ptr = NULL;

    return OK;
}
