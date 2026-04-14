/**
    @file globals.c
    @author Léandre BAUDET
    @date 2026-01-12
    @date 2026-04-14
    @brief Definitions of program-wide global variables.
*/
#include "utils/globals.h"

// ────────────────────────────────────────────────
// General Globals
// ────────────────────────────────────────────────

LobbyGame_St lobby_game = {0};

Font lobby_fonts[__fontSizeCount] = {0};

f32 gameTime = 0.0f;

PlayerProgress_St g_progress = {0};

// ────────────────────────────────────────────────
// Platforms / Terrains
// ────────────────────────────────────────────────

TerrainVec_St terrains = {0};

Texture2D terrainTextures[__terrainKindCount] = {0};

GameInteractionZone_St gameZones[__miniGameIdCount] = {
    [MINI_GAME_ID_BINGO] = {
        .hitbox = {
            .x      = -1475.00,
            .y      = 425.00,
            .width  = 75,
            .height = 75
        },
        .name = "Bingo"
    },

    [MINI_GAME_ID_CHESS] = {
        .hitbox = { 
            .x      = 651.67,
            .y      = -803.00,
            .width  = 75,
            .height = 75
        },
        .name = "Chess"
    },

    [MINI_GAME_ID_DROP_FOUR]  = {
        .hitbox = {
            .x      = -594.00,
            .y      = -771.67,
            .width  = 75,
            .height = 75
        },
        .name = "Drop Four" 
    },

    [MINI_GAME_ID_TETROMINO_FALL] = {
        .hitbox = {
            .x      = -85.00,
            .y      = -271.67,
            .width  = 75,
            .height = 75
        },
        .name = "Tetromino Fall",
        .color = {0, 120, 220, 200},
    },
    
    [MINI_GAME_ID_EDITOR] = {
        .hitbox = {
            .x      = -161.67,
            .y      = -731.67,
            .width  = 75,
            .height = 75
        },
        .name = "Level Editor",
        .color = {0, 160, 80,  200},
    },

    [MINI_GAME_ID_SUIKA] = {
        .hitbox = {
            .x      = 379.00,
            .y      = -1206.30,
            .width  = 75,
            .height = 75
        },
        .name = "Suika",
        .color = {220, 80, 0,  200},
    },

    [MINI_GAME_ID_BOWLING] = {
        .hitbox = {
            .x      = -416.33,
            .y      = -943.00,
            .width  = 75,
            .height = 75
        },
        .name = "Bowling",
        .color = {140, 0, 200, 200},
    },

    [MINI_GAME_ID_MINI_GOLF] = {
        .hitbox = {
            .x      = -506.67,
            .y      = -258.33,
            .width  = 75,
            .height = 75
        },
        .name = "3D Mini-Golf",
        .color = { 20, 160,  50, 200},
    },

    [MINI_GAME_ID_SNAKE] = {
        .hitbox = {
            .x      = -60.00,
            .y      = -545.00,
            .width  = 75,
            .height = 75
        },
        .name = "Snake",
        .color = {  0, 200,  80, 200},
    },

    [MINI_GAME_ID_POLY_BLAST] = {
        .hitbox = {
            .x      = 311.67,
            .y      = -631.00,
            .width  = 75,
            .height = 75
        },
        .name = "Poly Blast!",
        .color = { 60,  60, 200, 200},
    },

    [MINI_GAME_ID_DISC_REVERSAL]    = {
        .hitbox = {
            .x      = -23.33,
            .y      = -1081.6,
            .width  = 75,
            .height = 75
        },
        .name = "Disc Reversal" 
    },

    [MINI_GAME_ID_KING_FOR_FOUR] = {
        .hitbox = {
            .x      =  63.33,
            .y      = -78.33,
            .width  = 75,
            .height = 75
        },
        .name = "King For Four"
    },

    [MINI_GAME_ID_TWIST_CUBE] = {
        .hitbox = { 
            .x      = -870.00,
            .y      = -538.33,
            .width  = 75,
            .height = 75
        },
        .name = "Twist Cube"
    },

    [MINI_GAME_ID_SOLO_CARDS] = {
        .hitbox = {
            .x      =  218.33,
            .y      = -871.67,
            .width  = 75,
            .height = 75
        },
        .name = "Solo Cards"
    },
};

// ────────────────────────────────────────────────
// Skin stuff
// ────────────────────────────────────────────────

Rectangle skinButtonRect = { WINDOW_WIDTH - 70, WINDOW_HEIGHT / 2.0f - 25, 50, 50 };

Texture2D logoSkinButton = {0};

// ────────────────────────────────────────────────
// Parameters Menu
// ────────────────────────────────────────────────

ParamsMenu_St paramsMenu;

// ────────────────────────────────────────────────
// Textures / Ambiant
// ────────────────────────────────────────────────

Texture2D treeTexture;
Texture2D backgroundTexture;

GrassBlade_St grassBlades[MAX_GRASS_BLADES];
int grassCount = 0;

const Vector2 moonLightDir = {-0.6f, -0.8f};

// ────────────────────────────────────────────────
// Sounds
// ────────────────────────────────────────────────

Sound sound_jump;
Sound sound_doubleJump;
Sound sound_gameLaunch;

Sound sound_doubleJumpMeme;

// ────────────────────────────────────────────────
// Physics Debug Panel
// ────────────────────────────────────────────────

bool showPhysicsDebugPanel = false;
f32 panelScrollY = 0.0f;
