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

#include "lobbyAPI.h"

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
    gameRef->subGameManager.currentScene = GAME_SCENE_LOBBY;
    
    /** Flag: game needs initialization on next frame */
    gameRef->subGameManager.needGameInit = false;

    GameCollisionZone_St gameZones[__gameSceneCount] = {
        [GAME_SCENE_TETRIS] = {
            .hitbox = {
                .x      = -350,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Tetris",
            .color = {0, 120, 220, 200},
        },
        [GAME_SCENE_SOLITAIRE] = {
            .hitbox = {
                .x      = -125,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Solitaire",
            .color = {0, 160, 80,  200},
        },
        [GAME_SCENE_SUIKA] = {
            .hitbox = {
                .x      = 100,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Suika",
            .color = {220, 80, 0,  200},
        },
        [GAME_SCENE_BOWLING] = {
            .hitbox = {
                .x      = 325,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Bowling",
            .color = {140, 0, 200, 200},
        },
        [GAME_SCENE_GOLF] = {
            .hitbox = {
                .x      = 550,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Golf 3D",
            .color = { 20, 160,  50, 200},
        },
        [GAME_SCENE_SNAKE] = {
            .hitbox = {
                .x      = 775,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Snake",
            .color = {  0, 200,  80, 200},
        },
        [GAME_SCENE_BINGO] = {
            .hitbox = {
                .x      = 1000,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "Bingo",
            .color = {255, 200,   0, 200},
        },
        [GAME_SCENE_BLOCKBLAST] = {
            .hitbox = {
                .x      = 1225,
                .y      = 425,
                .width  = 75,
                .height = 75
            },
            .name = "BlockBlast",
            .color = { 60,  60, 200, 200},
        },
    };

    memcpy(gameRef->subGameManager.gameZones, gameZones, sizeof(gameZones));

    gameRef->subGameManager.currentScene = GAME_SCENE_LOBBY;
    gameRef->subGameManager.needGameInit = false;

    gameRef->player = (Player_St) {
        .radius      = 20,
        .unlockedTextures = {
            [PLAYER_TEXTURE_DEFAULT]   = 1,
            [PLAYER_TEXTURE_EARTH]     = 1,
        }
    };

    gameRef->cam = (Camera2D) {
        .offset = { systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f },
        .zoom   = 1.0f,
    };

    gameRef->playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    lobby_initGrass();
    lobby_initTextures(gameRef);

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

    if (gameTime > 1.45f) {
        updatePlayer(&game->player, platforms, platformCount, dt);
    }

    game->cam.target = game->player.position;

    paramsMenu_update(&paramsMenu);

    // Collision check with game zone
    for (u8 i = 1; i < __gameSceneCount; ++i) {
        if (CheckCollisionCircleRec(game->player.position, game->player.radius, game->subGameManager.gameZones[i].hitbox)) {
            if (IsKeyPressed(KEY_E)) {
                game->subGameManager.currentScene = i;
                game->subGameManager.needGameInit = true;
            }
        } else if (game->subGameManager.gameHitGracePeriodActive) {
            game->subGameManager.gameHitGracePeriodActive = false;
        }
    }

    toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game->player, game);
    }

    updateGrass(&game->player, GetFrameTime(), gameTime, game->cam);
    updateAtmosphericEffects(dt, &game->player, game->cam);

    BeginDrawing(); {
        ClearBackground((Color){10, 10, 30, 255}); /* nuit */

        BeginMode2D(game->cam); {
            drawStarryBackground(game->player.position, game->cam);

            drawTree();
            
            drawPlatforms(platforms, platformCount);
            drawPlayer(game, &game->player);
            
            drawWorldBoundaries(&game->player);
            
            drawGrass(&game->player, game->cam);

            for (u8 i = 1; i < __gameSceneCount; ++i) {
                GameCollisionZone_St gameZone = game->subGameManager.gameZones[i];

                bool playerNear = CheckCollisionCircleRec(
                    game->player.position, game->player.radius, gameZone.hitbox);

                DrawRectangleRec(gameZone.hitbox, gameZone.color);
                if (playerNear)
                    DrawRectangleLinesEx(gameZone.hitbox, 3, WHITE);

                f32 cx = gameZone.hitbox.x + gameZone.hitbox.width / 2.0f;
                f32 cy = gameZone.hitbox.y + gameZone.hitbox.height / 2.0f;
                DrawTriangle(
                    (Vector2){cx,        cy - 18},
                    (Vector2){cx - 12,   cy + 8},
                    (Vector2){cx + 12,   cy + 8},
                    WHITE);

                f32 nameWidth = MeasureText(gameZone.name, 14);
                DrawText(gameZone.name,
                    gameZone.hitbox.x + (gameZone.hitbox.width - nameWidth) / 2.0f,
                    gameZone.hitbox.y - 20, 14, WHITE);

                if (playerNear) {
                    const char* prompt = "[ E ]";
                    f32 pw = MeasureText(prompt, 12);
                    DrawText(prompt,
                        gameZone.hitbox.x + (gameZone.hitbox.width - pw) / 2.0f,
                        gameZone.hitbox.y + gameZone.hitbox.height + 4, 12, YELLOW);
                }
            }

            drawAtmosphericEffects();
        } EndMode2D();

        drawScreenEffects(&game->player);

        f32 lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

        drawSkinButton();
        if (game->playerVisuals.isTextureMenuOpen) {
            drawMenuTextures(game);
        }

    } EndDrawing();

    return OK;
}

Error_Et lobby_freeGame(LobbyGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    LobbyGame_St* gameRef = *game;

    for (u8 i = 1; i < __gameSceneCount; ++i) {
        if (gameRef->subGameManager.miniGames[i] == NULL) continue;
        gameRef->subGameManager.miniGames[i]->freeGame(&gameRef->subGameManager.miniGames[i]);
        gameRef->subGameManager.miniGames[i] = NULL;
    }

    lobby_freeAudio();
    lobby_freeTextures(gameRef);

    // Cleanup params menu
    paramsMenu_free(&paramsMenu);

    free(gameRef);
    *game = NULL;

    CloseWindow();

    return OK;
}
