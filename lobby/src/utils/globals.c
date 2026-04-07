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
    [MINI_GAME_KFF]        = { .hitbox = { -350.0f, 340.0f, 100.0f, 60.0f }, .name = "King For Four" },
    [MINI_GAME_CHESS]      = { .hitbox = {  250.0f, 340.0f, 100.0f, 60.0f }, .name = "Chess" },
    [MINI_GAME_CUBE]       = { .hitbox = { -200.0f, 240.0f, 100.0f, 60.0f }, .name = "Rubik Cube" },
    [MINI_GAME_BINGO]      = { .hitbox = {  100.0f, 240.0f, 100.0f, 60.0f }, .name = "Bingo" },
    [MINI_GAME_EDITOR]     = { .hitbox = {  -50.0f, 140.0f, 100.0f, 60.0f }, .name = "Level Editor" },
    
    // Hide others for now
    [MINI_GAME_BATTLESHIP] = { .hitbox = {0}, .name = "Battleship" },
    [MINI_GAME_CONNECT_4]  = { .hitbox = {0}, .name = "Connect 4" },
    [MINI_GAME_MINIGOLF]   = { .hitbox = {0}, .name = "Mini Golf" },
    [MINI_GAME_MORPION]    = { .hitbox = {0}, .name = "Morpion" },
    [MINI_GAME_OTHELLO]    = { .hitbox = {0}, .name = "Othello" }
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
