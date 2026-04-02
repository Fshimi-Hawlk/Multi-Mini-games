/**
    @file utils/globals.c
    @author LeandreB8
    @author Fshimi-Hawlk
    @author i-Charlys
    @date 2026-01-12
    @date 2026-03-30
    @brief Definitions of program-wide global variables used in the lobby.
*/

#include "utils/globals.h"
#include "firstparty/progress.h"

/** @brief Array of loaded fonts. */
Font lobby_fonts[__fontSizeCount] = {0};

/** @brief Main lobby game state instance. */
LobbyGame_St lobby_game = {0};

/** @brief Global player progress. */
PlayerProgress_St g_progress = {0};

/** @brief Dynamic array of terrains. */
TerrainVec_St terrains = {0};

/** @brief Predefined interaction zones for each mini-game. */
GameInteractionZone_St gameInteractionZones[__miniGameCount] = {
    [MINI_GAME_BATTLESHIP] = { .hitbox = { .x = 700.0f,  .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Battleship" },
    [MINI_GAME_BINGO]      = { .hitbox = { .x = 900.0f,  .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Bingo" },
    [MINI_GAME_CONNECT_4]  = { .hitbox = { .x = 1100.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Connect 4" },
    [MINI_GAME_KFF]        = { .hitbox = { .x = 1300.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "King For Four" },
    [MINI_GAME_MINIGOLF]   = { .hitbox = { .x = 1500.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Mini Golf" },
    [MINI_GAME_MORPION]    = { .hitbox = { .x = 1700.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Morpion" },
    [MINI_GAME_OTHELLO]    = { .hitbox = { .x = 1900.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Othello" },
    [MINI_GAME_CHESS]      = { .hitbox = { .x = 2100.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Chess" },
    [MINI_GAME_CUBE]       = { .hitbox = { .x = 2300.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Rubik Cube" }
};

/** @brief Global chat state. */
Chat_St gameChat = {0};

/** @brief Screen area for skin selection button. */
Rectangle skinButtonRect = { 10, 10, 50, 50 };

/** @brief Texture for skin button. */
Texture2D logoSkinButton;

/** @brief Debug flags. */
bool showPhysicsDebugPanel = false;
f32 panelScrollY = 0.0f;
