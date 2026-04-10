/**
    @file lobbyAPI.c
    @author Fshimi-Hawlk
    @date 2026-02-08
    @date 2026-02-23
    @brief Lobby / hub scene - entry point and central navigation area
*/

#include "core/game.h"

#include "setups/app.h"
#include "setups/audio.h"
#include "setups/game.h"
#include "setups/texture.h"

#include "ui/app.h"
#include "ui/game.h"
#include "ui/grass.h"
#include "ui/background.h"
#include "ui/ambiance.h"

#include "utils/globals.h"

#include "sharedUtils/mathUtils.h"
#include "utils/userTypes.h"

#include "lobbyAPI.h"


static void updateCameraOnWindowResize(LobbyGame_St* const game) {
    const f32 originalWidth  = 800;
    const f32 originalHeight = 600;

    // Always keep camera perfectly centered on the new window size
    game->cam.offset = (Vector2){
        systemSettings.video.width  / 2.0f,
        systemSettings.video.height / 2.0f
    };

    // ── Zoom adaptation when width OR height changes ─────────────────────
    f32 zoomX = (f32)systemSettings.video.width  / originalWidth;
    f32 zoomY = (f32)systemSettings.video.height / originalHeight;

    game->cam.zoom = min(zoomX, zoomY);
}

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    Error_Et error = OK;

    systemSettings = DEFAULT_SYSTEM_SETTING;
    systemSettings.video.title = "Lobby";

    lobby_initApp();

    (void) configs; // Configs aren't used yet

    error = applySystemSettings();
    if (error != OK) {
        log_error("System settings couldn't be applied correctly");
    }

    (*game) = malloc(sizeof(LobbyGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    LobbyGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    /** Current active scene (lobby or one of the mini-games) */
    gameRef->subGameManager.currentScene = MINI_GAME_ID_LOBBY;
    
    /** Flag: game needs initialization on next frame */
    gameRef->subGameManager.needGameInit = false;

    GameCollisionZone_St gameZones[__miniGameIdCount] = {
        [MINI_GAME_ID_TETRIS] = {
            .hitbox = {
                .x      = -800,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Tetris",
            .color = {0, 120, 220, 200},
        },
        [MINI_GAME_ID_SOLITAIRE] = {
            .hitbox = {
                .x      = -575,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Solitaire",
            .color = {0, 160, 80,  200},
        },
        [MINI_GAME_ID_SUIKA] = {
            .hitbox = {
                .x      = -350,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Suika",
            .color = {220, 80, 0,  200},
        },
        [MINI_GAME_ID_BOWLING] = {
            .hitbox = {
                .x      = -125,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Bowling",
            .color = {140, 0, 200, 200},
        },
        [MINI_GAME_ID_GOLF] = {
            .hitbox = {
                .x      = 100,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Golf 3D",
            .color = { 20, 160,  50, 200},
        },
        [MINI_GAME_ID_SNAKE] = {
            .hitbox = {
                .x      = 325,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Snake",
            .color = {  0, 200,  80, 200},
        },
        [MINI_GAME_ID_POLY_BLAST] = {
            .hitbox = {
                .x      = 550,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "BlockBlast",
            .color = { 60,  60, 200, 200},
        },
    };

    memcpy(gameRef->subGameManager.gameZones, gameZones, sizeof(gameZones));

    gameRef->subGameManager.currentScene = MINI_GAME_ID_LOBBY;
    gameRef->subGameManager.needGameInit = false;

    gameRef->player = (Player_St) {
        .radius      = 20,
        .position    = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y},
        .onGround    = true,
        .unlockedTextures = {
            [PLAYER_TEXTURE_DEFAULT]   = 1,
            [PLAYER_TEXTURE_EARTH]     = 1,
        }
    };

    gameRef->cam = (Camera2D) {
        .offset = { systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f },
        .target = {.x = gameRef->player.position.x, gameRef->player.position.y - gameRef->player.radius * 1.5f},
        .zoom   = 1.0f,
    };

    gameRef->playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    lobby_initGrass();
    lobby_initBackgroundScale();
    updateCameraOnWindowResize(gameRef);

    error = lobby_initTextures(gameRef->playerVisuals.textures);

    // Initialize parameters menu (settings button)
    paramsMenu_init(&paramsMenu);

    return error;
}

Error_Et lobby_gameLoop(LobbyGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;

    f32 dt = GetFrameTime();

    skinButtonRect.x = GetScreenWidth()  - 70;
    skinButtonRect.y = GetScreenHeight() / 2.0f - 25;

    gameTime += dt;

    if (IsWindowResized()) {
        systemSettings.video.width = GetScreenWidth();
        systemSettings.video.height = GetScreenHeight();

        skinButtonRect = (Rectangle) {
            .x = systemSettings.video.width - 70,
            .y = systemSettings.video.height / 2.0f - 25,
            .width = 50, 
            .height = 50
        };

        updateCameraOnWindowResize(game);
    }

    if (gameTime > 2.0f) {
        lobby_updatePlayer(&game->player, platforms, platformCount, dt);
    }

    Vector2 desiredTarget = game->player.position;
    if (game->player.onGround && game->player.position.y > GROUND_Y - 70.0f) {
        desiredTarget.y -= 135.0f;
    } else {
        desiredTarget.y -= game->player.radius * 1.5f;
    }

    game->cam.target = Vector2Lerp(
        game->cam.target,
        desiredTarget,
        0.05f
    );

    paramsMenu_update(&paramsMenu);

    // Collision check with game zone
    for (u8 i = 1; i < __miniGameIdCount; ++i) {
        if (game->subGameManager.gameZones[i].name == NULL) continue;
        if (CheckCollisionCircleRec(game->player.position, game->player.radius, game->subGameManager.gameZones[i].hitbox)) {
            if (IsKeyPressed(KEY_E)) {
                game->subGameManager.currentScene = i;
                game->subGameManager.needGameInit = true;
            }
        } else if (game->subGameManager.gameHitGracePeriodActive) {
            game->subGameManager.gameHitGracePeriodActive = false;
        }
    }

    lobby_toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        lobby_choosePlayerTexture(&game->player, game);
    }

    lobby_updateGrass(&game->player, GetFrameTime(), gameTime, game->cam);
    lobby_updateAtmosphericEffects(dt, &game->player, game->cam);

    BeginDrawing(); {
        ClearBackground((Color){10, 10, 30, 255}); /* nuit */

        BeginMode2D(game->cam); {
            lobby_drawStarryBackground(game->player.position, game->cam);

            lobby_drawTree();
            
            lobby_drawPlatforms(platforms, platformCount);

            lobby_drawPlayer(game);
            
            lobby_drawWorldBoundaries(&game->player);
            
            lobby_drawGrass(&game->player, game->cam);

            lobby_drawGameZones(game);

            lobby_drawAtmosphericEffects();
        } EndMode2D();

        lobby_drawScreenEffects(&game->player);

        f32 lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

        lobby_drawSkinButton();
        if (game->playerVisuals.isTextureMenuOpen) {
            lobby_drawMenuTextures(game);
        }

    } EndDrawing();

    return OK;
}

Error_Et lobby_freeGame(LobbyGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    LobbyGame_St* gameRef = *game;

    for (u8 i = 1; i < __miniGameIdCount; ++i) {
        if (gameRef->subGameManager.miniGames[i] == NULL) continue;
        gameRef->subGameManager.miniGames[i]->freeGame(&gameRef->subGameManager.miniGames[i]);
        gameRef->subGameManager.miniGames[i] = NULL;
    }

    lobby_freeAudio();
    lobby_freeTextures(gameRef->playerVisuals.textures);

    // Cleanup params menu
    paramsMenu_free(&paramsMenu);

    free(gameRef);
    *game = NULL;

    CloseWindow();

    return OK;
}
