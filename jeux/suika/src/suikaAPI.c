/**
 * @file suikaAPI.c
 * @author Multi Mini-Games Team
 * @date February 2026
 * @brief Implementation of the Suika mini-game API.
 */

<<<<<<< HEAD
#include "core/game.h"
#include "utils/types.h"
#include "utils/configs.h"
#include "logger.h"
#include "suikaAPI.h"
#include "audio.h"
#include <time.h>

=======
#include "suikaAPI.h"  // Must be first - defines SuikaGame_St opaque type
#include "utils/types.h"  // Includes raylib and defines SuikaGame_St struct

// Include params menu header for type definition (not implementation)
#include "../../../firstparty/include/ui/paramsMenu.h"

#include "core/game.h"
#include "utils/configs.h"
#include "logger.h"
#include "audio.h"
#include <time.h>

// Global params menu state for suika
static ParamsMenu_St suikaParamsMenu = {0};

>>>>>>> 3777fd6 (- add : new 3D golf game)
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
    suika_initAudio();
    suika_init(game);

<<<<<<< HEAD
=======
    // Initialize params menu (settings button)
    paramsMenu_init(&suikaParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
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

<<<<<<< HEAD
=======
    // ESC key returns to lobby
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
        return OK;
    }

    // Update params menu (settings button clicks)
    paramsMenu_update(&suikaParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    float deltaTime = GetFrameTime();

    suika_update(game, deltaTime);

    BeginDrawing();
        suika_draw(game);
<<<<<<< HEAD
=======

        // Draw params menu (settings button)
        paramsMenu_draw(&suikaParamsMenu);
>>>>>>> 3777fd6 (- add : new 3D golf game)
    EndDrawing();

    return OK;
}

Error_Et suika_freeGame(SuikaGame_St** game_ptr)
{
    if (game_ptr == NULL || *game_ptr == NULL)
    {
        return ERROR_NULL_POINTER;
    }

<<<<<<< HEAD
=======
    // Cleanup params menu
    paramsMenu_free(&suikaParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    SuikaGame_St* game = *game_ptr;

    suika_unloadAssets(game);
    suika_freeAudio();
    suika_cleanup(game);

    free(game);
    *game_ptr = NULL;

    return OK;
}
