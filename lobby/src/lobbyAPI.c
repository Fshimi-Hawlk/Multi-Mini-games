/**
    @file lobbyAPI.c
    @author Fshimi-Hawlk
    @date 2026-02-08
    @date 2026-02-23
    @brief Lobby / hub scene - entry point and central navigation area
*/

#include "core/game.h"

#include "raylib.h"
#include "ui/app.h"
#include "ui/game.h"

#include "utils/common.h"
#include "utils/globals.h"

#include "lobbyAPI.h"
#include "systemSettings.h"

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    Error_Et error = OK;

    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(systemSettings.video.width, systemSettings.video.height, WINDOW_TITLE);
    SetWindowPosition(100, 100);

    (void) configs;

    systemSettings = DEFAULT_SYSTEM_SETTING;
    systemSettings.video.resizable = true;
    systemSettings.video.title = "Lobby";
    error = applySystemSettings();
    if (error != OK) {
        log_error("System settings couldn't be applied correctly");
    }

    (*game) = malloc(sizeof(LobbyGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    LobbyGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    /* Hitboxes posées sur le sol (floor top = y:500, hitbox height = 75 → y:425)
     * Réparties en x de -350 à +350 pour couvrir toute la zone de jeu. */
    gameRef->subGameManager.gameHitboxes[GAME_SCENE_TETRIS] = (Rectangle) {
        .x      = -350,
        .y      = 425,
        .width  = 75,
        .height = 75
    };

    gameRef->subGameManager.gameHitboxes[GAME_SCENE_SOLITAIRE] = (Rectangle) {
        .x      = -125,
        .y      = 425,
        .width  = 75,
        .height = 75
    };

    gameRef->subGameManager.gameHitboxes[GAME_SCENE_SUIKA] = (Rectangle) {
        .x      = 100,
        .y      = 425,
        .width  = 75,
        .height = 75
    };

    gameRef->subGameManager.gameHitboxes[GAME_SCENE_BOWLING] = (Rectangle) {
        .x      = 325,
        .y      = 425,
        .width  = 75,
        .height = 75
    };

    gameRef->subGameManager.currentScene = GAME_SCENE_LOBBY;
    gameRef->subGameManager.needGameInit = false;

    /* FIX: coyoteTime removed from struct — we now only initialize coyoteTimer.
     * jumpBuffer initialized to 0: the buffer should not be active at spawn. */
    gameRef->player = (Player_st) {
        .position    = { 0, 440 },  /* spawn above floor: floor_y(500) - radius(20) - margin(40) */
        .radius      = 20,
        .coyoteTimer = COYOTE_TIME,
        .jumpBuffer  = 0.0f
    };

    gameRef->player.unlockedTextures[PLAYER_TEXTURE_DEFAULT]   = 1;
    gameRef->player.unlockedTextures[PLAYER_TEXTURE_EARTH]     = 1;

    gameRef->cam = (Camera2D) {
        .offset = { systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f },
        .zoom   = 1.0f,
    };

    gameRef->playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH] = LoadTexture(IMAGES_PATH "earth.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH])) {
        log_warn("%s couldn't be loaded properly.", IMAGES_PATH "earth.png");
        error = ERROR_TEXTURE_LOAD;
    }

    gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE] = LoadTexture(IMAGES_PATH "trollFace.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE])) {
        log_warn("%s couldn't be loaded properly.", IMAGES_PATH "trollFace.png");
        error = ERROR_TEXTURE_LOAD;
    }

    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded properly.", IMAGES_PATH "logoSkin.png");
        error = ERROR_TEXTURE_LOAD;
    }

    return error;
}

Error_Et lobby_gameLoop(LobbyGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;

    f32 dt = GetFrameTime();

    updatePlayer(&game->player, platforms, platformCount, dt);
    game->cam.target = game->player.position;
    /* Recalculate offset each frame — window may have been resized by a mini-game */
    game->cam.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game->player, game);
    }

    for (u8 i = 1; i < __gameSceneCount; ++i) {
        if (CheckCollisionCircleRec(game->player.position, game->player.radius, game->subGameManager.gameHitboxes[i])) {
            // Vérifier si la touche E est pressée pour lancer le jeu
            if (IsKeyPressed(KEY_E)) {
                game->subGameManager.currentScene = i;
                game->subGameManager.needGameInit = true;
            }
        } else if (game->subGameManager.gameHitGracePeriodActive) {
            game->subGameManager.gameHitGracePeriodActive = false;
        }
    }

    BeginDrawing(); {
        ClearBackground((Color){135, 206, 235, 255}); /* ciel bleu */

        BeginMode2D(game->cam); {
            /* Fond de ciel en world-space (large rectangle derrière tout) */
            DrawRectangle(-2000, -2000, 4000, 2500, (Color){135, 206, 235, 255});
            /* Sol herbe */
            DrawRectangle(-1000, 500, 2000, 1000, (Color){34, 139, 34, 255});

            drawPlayer(game, &game->player);
            drawPlatforms(platforms, platformCount);

            for (u8 i = 1; i < __gameSceneCount; ++i) {
                Rectangle hitbox = game->subGameManager.gameHitboxes[i];

                /* Couleur et nom par jeu */
                const char* gameName = "";
                Color portalColor = RED;
                switch (i) {
                    case GAME_SCENE_TETRIS:    gameName = "Tetris";    portalColor = (Color){0, 120, 220, 200}; break;
                    case GAME_SCENE_SOLITAIRE: gameName = "Solitaire"; portalColor = (Color){0, 160, 80,  200}; break;
                    case GAME_SCENE_SUIKA:     gameName = "Suika";     portalColor = (Color){220, 80, 0,  200}; break;
                    case GAME_SCENE_BOWLING:   gameName = "Bowling";   portalColor = (Color){140, 0, 200, 200}; break;
                }

                bool playerNear = CheckCollisionCircleRec(
                    game->player.position, game->player.radius, hitbox);

                /* Portal: fond coloré + bordure blanche si proche */
                DrawRectangleRec(hitbox, portalColor);
                if (playerNear)
                    DrawRectangleLinesEx(hitbox, 3, WHITE);

                /* Icône centrale (flèche vers le haut) */
                f32 cx = hitbox.x + hitbox.width / 2.0f;
                f32 cy = hitbox.y + hitbox.height / 2.0f;
                DrawTriangle(
                    (Vector2){cx,        cy - 18},
                    (Vector2){cx - 12,   cy + 8},
                    (Vector2){cx + 12,   cy + 8},
                    WHITE);

                /* Nom du jeu au-dessus du portail */
                f32 nameWidth = MeasureText(gameName, 14);
                DrawText(gameName,
                    hitbox.x + (hitbox.width - nameWidth) / 2.0f,
                    hitbox.y - 20, 14, WHITE);

                /* "[ E ]" en dessous si proche */
                if (playerNear) {
                    const char* prompt = "[ E ]";
                    f32 pw = MeasureText(prompt, 12);
                    DrawText(prompt,
                        hitbox.x + (hitbox.width - pw) / 2.0f,
                        hitbox.y + hitbox.height + 4, 12, YELLOW);
                }
            }
        } EndMode2D();

        /* FIX: lobbyTextXPos — local variable, not static. It is recalculated every
         * frame (position depends on window width), keeping it static serves no purpose
         * and keeps an obsolete value if the window is resized. */
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

    for (u32 i = 1; i < __playerTextureCount; ++i) {
        UnloadTexture(gameRef->playerVisuals.textures[i]);
    }

    UnloadTexture(logoSkinButton);

    free(gameRef);
    *game = NULL;

    CloseWindow();

    return OK;
}
