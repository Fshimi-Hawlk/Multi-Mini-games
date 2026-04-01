/**
    @file lobbyAPI.c
    @author Fshimi-Hawlk
    @author i-Charlys
    @date 2026-02-08
    @date 2026-02-23
    @brief Lobby / hub scene - entry point and central navigation area

    Contributors:
        - LeandreB8:
            - Provided the initial logic for init and the game loop.
        - Fshimi-Hawlk:
            - Moved reworked lobby's initialization, game loop and freeing logic in dedicated `lobbyAPI` files
            - Added documentation

    This module implements the main lobby scene that serves as the central hub
    of the multi mini-game application. The player can:
        - move around a small 2D platformer-style world,
        - open a skin / texture selection menu,
        - collide with designated trigger zones to launch individual mini-games.

    Responsibilities:
        - Setting seed for rand
        - Window & camera initialization
        - Player movement & basic platformer physics (via shared updatePlayer())
        - Skin / player texture selection UI
        - Collision-based mini-game activation with grace period / debounce
        - Debug visualization (hitboxes, origin marker)
        - Resource loading & cleanup for lobby-specific assets

    Important data flow:
      LobbyGame_St -> owns player, camera, sub-game manager state
      SubGameManager -> controls which scene is active and whether initialization
                       is required on next frame

    Mini-game activation is currently performed via rectangular hitbox checks.
    Future directions may include UI buttons, portals with animations, etc.

    @note    Most mini-game specific logic is delegated to the corresponding
             scene modules through the function pointer table in SubGameManager.

    @see `core/game.h`       for `choosePlayerTexture()`, `toggleSkinMenu()`, `updatePlayer()`
    @see `ui/app.h`          for `drawMenuTextures()`, `drawSkinButton()`
    @see `ui/game.h`         for `drawPlatforms()`, `drawPlayer()`
    @see `utils/globals.h`   for `logoSkinButton`, `platformCount`, `platforms`
    @see `APIs/generalAPI.h` for `Error_Et`
*/

#include "APIs/generalAPI.h"
#include "core/game.h"
#include "rand.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/globals.h"

#include "lobbyAPI.h"
#include "systemSettings.h"
#include "utils/userTypes.h"

Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs) {
    Error_Et error = OK;

    u64 seeds[2] = { 0 };
    plat_get_entropy(seeds, sizeof(seeds));
    prng_seed(seeds[0], seeds[1]);

    systemSettings = DEFAULT_SYSTEM_SETTING;
    systemSettings.video.resizable = true;
    systemSettings.video.title = "Lobby";

    // ── Initialization ───────────────────────────────────────────────────────
    InitWindow(systemSettings.video.width, systemSettings.video.height, systemSettings.video.title);
    if (!IsWindowReady()) {
        log_fatal("Failed to initialize Raylib window.");
        return ERROR_WINDOW_INIT;
    }
    SetExitKey(0); // Ne pas fermer sur ECHAP
    SetWindowPosition(100, 50);
    InitAudioDevice();

    (void) configs; // Configs aren't used yet

    (*game) = malloc(sizeof(LobbyGame_St));
    if (*game == NULL) return ERROR_ALLOC;

    LobbyGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    /** Hitbox that triggers the mini-games when player collides */
    Rectangle gameHitboxes[__miniGameCount] = { 
        [MINI_GAME_KFF] = {
            .x      = -350,
            .y      = 400 - 60,
            .width  = 100,
            .height = 60
        },
        [MINI_GAME_CHESS] = {
            .x      = 250,
            .y      = 400 - 60,
            .width  = 100,
            .height = 60
        },
        [MINI_GAME_CUBE] = {
            .x      = -200,
            .y      = 300 - 60,
            .width  = 100,
            .height = 60
        }
    };

    memcpy(gameRef->miniGameManager.gameHitboxes, gameHitboxes, sizeof(gameHitboxes));

    /** Current active scene (lobby or one of the mini-games) */
    gameRef->miniGameManager.currentMiniGame = MINI_GAME_LOBBY;
    
    /** Flag: game needs initialization on next frame */
    gameRef->miniGameManager.needGameInit = false;
    
    /** Player controlled by the user in the lobby */
    gameRef->player = (Player_St) {
        .position   = {0, 250},
        .radius     = 20,
        .coyoteTime = 0.1f,
        .coyoteTimer= 0.1f,
        .jumpBuffer = 0.2f
    };

    gameRef->player.unlockedTextures[PLAYER_TEXTURE_DEFAULT] = 1;
    gameRef->player.unlockedTextures[PLAYER_TEXTURE_EARTH] = 1;
    gameRef->player.unlockedTextures[PLAYER_TEXTURE_TROLL_FACE] = 1;

    /** Camera following the player in 2D mode */
    gameRef->cam = (Camera2D) {
        .offset = {systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f},
        .zoom   = 1.0f,
    };
    
    gameRef->playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    // Load shared UI textures
    gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH] = LoadTexture(IMAGES_PATH "earth.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_EARTH])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "earth.png");
        error =  ERROR_TEXTURE_LOAD;
    }
    
    gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE] = LoadTexture(IMAGES_PATH "trollFace.png");
    if (!IsTextureValid(gameRef->playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "trollFace.png");
        error =  ERROR_TEXTURE_LOAD;
    }
    
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "logoSkin.png");
        error =  ERROR_TEXTURE_LOAD;
    }

    // Load fonts
    u32 fontSize = 4;
    for (u32 fontId = 0; fontId < __fontSizeCount; fontId++) {
        // Use the root assets path for fonts
        lobby_fonts[fontId] = LoadFontEx("assets/fonts/Noto/static/NotoSansMono-Bold.ttf", (int)fontSize, NULL, 0);
        if (!IsFontValid(lobby_fonts[fontId])) {
            log_warn("Lobby font size %d couldn't be loaded.", fontSize);
        }
        fontSize += 2;
    }

    return error;
}

Error_Et lobby_gameLoop(LobbyGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;

    f32 dt = GetFrameTime();
    static f32 lobbyTextXPos;

    if (!gameChat.isOpen) {
        updatePlayer(&game->player, platforms, platformCount, dt);
    }
    game->cam.target = game->player.position;

    toggleSkinMenu(game);

    if (game->playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game->player, game);
    }

    // Collision check with game zone
    for (u8 i = 1; i < __miniGameCount; ++i) {
        if (CheckCollisionCircleRec(game->player.position, game->player.radius, game->miniGameManager.gameHitboxes[i])) {
            if (!game->miniGameManager.gameHitGracePeriodActive) {
                game->miniGameManager.currentMiniGame = i;
                game->miniGameManager.gameHitGracePeriodActive = true;
            }
        } else if (game->miniGameManager.gameHitGracePeriodActive) {
            game->miniGameManager.gameHitGracePeriodActive = false;
        }
    }

    BeginMode2D(game->cam); {
        // Draw other players with simple interpolation
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (game->otherPlayers[i].active) {
                // Smooth interpolation towards targetPosition
                game->otherPlayers[i].position.x += (game->otherPlayers[i].targetPosition.x - game->otherPlayers[i].position.x) * dt * 10.0f;
                game->otherPlayers[i].position.y += (game->otherPlayers[i].targetPosition.y - game->otherPlayers[i].position.y) * dt * 10.0f;
                
                drawPlayer(game, &game->otherPlayers[i]);
                const char* displayName = (game->otherPlayers[i].name[0] != '\0') ? game->otherPlayers[i].name : TextFormat("Player %d", i);
                Vector2 nameSize = MeasureTextEx(lobby_fonts[FONT16], displayName, 16, 0);
                DrawTextEx(lobby_fonts[FONT16], displayName, (Vector2){ game->otherPlayers[i].position.x - nameSize.x/2, game->otherPlayers[i].position.y - 40 }, 16, 0, DARKGRAY);
            }
        }

        drawPlayer(game, &game->player);
        const char* myName = (game->player.name[0] != '\0') ? game->player.name : "Moi";
        Vector2 myNameSize = MeasureTextEx(lobby_fonts[FONT16], myName, 16, 0);
        DrawTextEx(lobby_fonts[FONT16], myName, (Vector2){ game->player.position.x - myNameSize.x/2, game->player.position.y - 40 }, 16, 0, MAROON);
        drawPlatforms(platforms, platformCount);
    } EndMode2D();

    lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
    DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

    drawSkinButton();

    if (game->playerVisuals.isTextureMenuOpen) {
        drawMenuTextures(game);
    }

    return OK;
}

Error_Et lobby_freeGame(LobbyGame_St** gameRef) {
    if (gameRef == NULL || *gameRef == NULL) return ERROR_NULL_POINTER;
    LobbyGame_St* game = *gameRef;

    for (u8 i = 1; i < __miniGameCount; ++i) {
        if (game->miniGameManager.miniGames[i] == NULL) continue;
        BaseGame_St** base = &game->miniGameManager.miniGames[i];
        game->miniGameManager.miniGames[i]->freeGame(base);
        game->miniGameManager.miniGames[i] = NULL;
    }

    for (u32 i = 1; i < __playerTextureCount; ++i) {
        UnloadTexture(game->playerVisuals.textures[i]);
    }

    UnloadTexture(logoSkinButton);

    CloseAudioDevice();
    CloseWindow();

    free(game);
    *gameRef = NULL;

    return OK;
}
